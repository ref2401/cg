#include "technique/fur_simulation/fur_simulation_opengl.h"

#include <cassert>
#include "cg/data/image.h"
#include "cg/data/shader.h"

using cg::data::Image_2d;
using cg::data::Image_format;

namespace fur_simulation {

// ----- Fur_simulation_opengl_example -----

Fur_simulation_opengl_example::Fur_simulation_opengl_example(const cg::sys::App_context& app_ctx) :
	Example(app_ctx),
	_curr_viewpoint(float3(0, 0, 5), float3(0, 0, 0)),
	_prev_viewpoint(_curr_viewpoint),
	_light_dir_ws(normalize(float3(1, 1, 100.0)))
{
	init_model();
	init_fur_data();
}

void Fur_simulation_opengl_example::init_fur_data()
{
	const Sampler_desc linear_sampler(GL_LINEAR, GL_LINEAR, GL_REPEAT);

	{
		auto image_diffuse_rgb = cg::data::load_image_tga("../data/fur_simulation/zebra-diffuse-rgb.tga");
		_tex_diffuse_rgb = Texture_2d(GL_RGB8, 1, linear_sampler, image_diffuse_rgb);
	}

	std::vector<cg::data::Image_2d> layer_images;
	layer_images.reserve(_layer_count);
	layer_images.push_back(cg::data::load_image_tga("../data/fur_simulation/noise-texture.tga"));

	for (size_t i = 1; i < _layer_count; ++i) {
		auto& prev_image = layer_images.back();
		assert(prev_image.format() == Image_format::red_8);

		layer_images.emplace_back(prev_image.format(), prev_image.size());
		auto& curr_image = layer_images.back();

		uint8_t threshold = uint8_t(std::numeric_limits<uint8_t>::max() * float(i) / _layer_count);
		for (size_t bi = 0; bi < prev_image.byte_count(); ++bi) {
			uint8_t byte = prev_image.data()[bi];

			if (byte < threshold)
				byte = 0;

			curr_image.data()[bi] = byte;
		}
	}

	_tex_noise_list.reserve(_layer_count);
	for (const auto& noise_image : layer_images) {
		_tex_noise_list.emplace_back(GL_R8, 1, linear_sampler, noise_image);
	}
}

void Fur_simulation_opengl_example::init_model()
{
	using Model_geometry_data_t = cg::data::Model_geometry_data<cg::data::Vertex_attribs::p_n_tc>;

	//_model_matrix = mat4::identity;
	//auto model_data = cg::data::load_model<Model_geometry_data_t::Format::attribs>("../data/sphere-20x20.obj");

	_model_matrix = rotation_matrix_ox<mat4>(cg::pi_2) * scale_matrix<mat4>(float3(2.0f));
	//auto model_data = cg::data::load_model<Model_geometry_data_t::Format::attribs>("../data/cube.obj");
	auto model_data = cg::data::load_model<Model_geometry_data_t::Format::attribs>("../data/rect_2x2.obj");
	_draw_params = model_data.meshes()[0];

	glCreateVertexArrays(1, &_vao_id);
	glBindVertexArray(_vao_id);

	// vertex buffer
	constexpr GLuint vb_binding_index = 0;

	_vertex_buffer = Buffer_gpu(model_data.vertex_data_byte_count(), model_data.vertex_data().data());
	glVertexArrayVertexBuffer(_vao_id, vb_binding_index, _vertex_buffer.id(), 
		0, Model_geometry_data_t::Format::vertex_byte_count);

	// position
	glEnableVertexArrayAttrib(_vao_id, 0);
	glVertexArrayAttribBinding(_vao_id, 0, vb_binding_index);
	glVertexArrayAttribFormat(_vao_id, 0, 3, GL_FLOAT, false, Model_geometry_data_t::Format::position_byte_offset);

	// normal
	glEnableVertexArrayAttrib(_vao_id, 1);
	glVertexArrayAttribBinding(_vao_id, 1, vb_binding_index);
	glVertexArrayAttribFormat(_vao_id, 1, 3, GL_FLOAT, false, Model_geometry_data_t::Format::normal_byte_offset);

	// tex_coord
	glEnableVertexArrayAttrib(_vao_id, 2);
	glVertexArrayAttribBinding(_vao_id, 2, vb_binding_index);
	glVertexArrayAttribFormat(_vao_id, 2, 2, GL_FLOAT, false, Model_geometry_data_t::Format::tex_coord_byte_offset);

	// index buffer
	_index_buffer = Buffer_gpu(model_data.index_data_byte_count(), model_data.index_data().data());
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _index_buffer.id());
}

void Fur_simulation_opengl_example::on_mouse_move()
{
	if (!_app_ctx.mouse.middle_down()) return;

	float2 pos_ndc = _app_ctx.mouse.get_ndc_position(_app_ctx.window.viewport_size());
	float2 offset_ndc = pos_ndc - _prev_mouse_pos_ndc;
	if (offset_ndc == float2::zero) return;

	_prev_mouse_pos_ndc = pos_ndc;

	bool y_offset_satisfies = !approx_equal(offset_ndc.y, 0.f, 0.01f);
	bool x_offset_satisfies = !approx_equal(offset_ndc.x, 0.f, ((y_offset_satisfies) ? 0.01f : 0.001f));

	// mouse offset by x means rotation around OY (yaw)
	if (x_offset_satisfies) {
		_view_roll_angles.y += (offset_ndc.x > 0.f) ? -pi_64 : pi_64;
	}

	// mouse offset by x means rotation around OX (pitch)
	if (y_offset_satisfies) {
		_view_roll_angles.x += (offset_ndc.y > 0.f) ? pi_64 : -pi_64;
	}
}

void Fur_simulation_opengl_example::on_window_resize()
{
	auto vp_size = _app_ctx.window.viewport_size();
	glViewport(0, 0, vp_size.width, vp_size.height);
	update_projection_matrix();
}

void Fur_simulation_opengl_example::render(float interpolation_factor)
{
	mat4 view_matrix = ::view_matrix(_prev_viewpoint, _curr_viewpoint, interpolation_factor);
	mat4 projection_view_matrix = _projection_matrix* view_matrix;

	glClearColor(0.2f, 0.3f, 0.2f, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ZERO, GL_ONE);
	glBlendEquation(GL_FUNC_ADD);


	glBindTextureUnit(0, _tex_diffuse_rgb.id());
	// opaque model
	glDisable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	//glDepthMask(true);
	_glsl_opaque_model.bind(projection_view_matrix, _model_matrix, _light_dir_ws);
	glDrawElements(GL_TRIANGLES, _draw_params.index_count, GL_UNSIGNED_INT, nullptr);

	// fur generation
	const size_t draws_per_layer = 8;
	const size_t draw_count = draws_per_layer * _layer_count;
	const float max_lenght = 0.3f;
	const float position_step = max_lenght / draw_count;

	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	//glDepthMask(false);
	_glsl_fur_generation_noise.bind(_projection_matrix, view_matrix, _model_matrix, 
		_light_dir_ws, _layer_count, draws_per_layer, position_step);
	

	for (size_t i = 0; i < draw_count; ++i) {
		size_t layer_index = i / draws_per_layer;

		if ((i % draws_per_layer) == 0) {
			glBindTextureUnit(1, _tex_noise_list[layer_index].id());
		}

		_glsl_fur_generation_noise.set_draw_indices(layer_index, i);
		glDrawElements(GL_TRIANGLES, _draw_params.index_count, GL_UNSIGNED_INT, nullptr);
	}

	_prev_viewpoint = _curr_viewpoint;
}

void Fur_simulation_opengl_example::update(float dt)
{
	if (_view_roll_angles != float2::zero) {
		float dist = _curr_viewpoint.distance();
		float3 ox = cross(_curr_viewpoint.forward(), _curr_viewpoint.up);
		ox.y = 0.0f; // ox is always parallel the world's OX.
		ox = normalize(ox);

		if (!approx_equal(_view_roll_angles.y, 0.0f)) {
			quat q = from_axis_angle_rotation(float3::unit_y, _view_roll_angles.y);
			_curr_viewpoint.position = dist * normalize(rotate(q, _curr_viewpoint.position));

			ox = rotate(q, ox);
			ox.y = 0.0f;
			ox = normalize(ox);
		}

		if (!approx_equal(_view_roll_angles.x, 0.0f)) {
			quat q = from_axis_angle_rotation(ox, _view_roll_angles.x);
			_curr_viewpoint.position = dist * normalize(rotate(q, _curr_viewpoint.position));
		}

		_curr_viewpoint.up = normalize(cross(ox, _curr_viewpoint.forward()));
	}

	_view_roll_angles = float2::zero;
}

void Fur_simulation_opengl_example::update_projection_matrix()
{
	_projection_matrix = perspective_matrix_opengl(cg::pi_3,
		_app_ctx.window.viewport_size().aspect_ratio(), 0.1f, 50.0f);
}

} // fur_simulation
