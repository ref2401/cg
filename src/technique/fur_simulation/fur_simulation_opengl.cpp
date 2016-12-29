#include "technique/fur_simulation/fur_simulation_opengl.h"

#include <cassert>
#include "cg/data/image.h"
#include "cg/data/shader.h"

using cg::data::Image_2d;
using cg::data::Image_format;
using cg::sys::Key;


namespace fur_simulation {

// ----- Fur_simulation_opengl_example -----

Fur_simulation_opengl_example::Fur_simulation_opengl_example(const cg::sys::App_context& app_ctx) :
	Example(app_ctx),
	_curr_viewpoint(float3(0, 0, 7), float3(0, 0, 0)),
	_prev_viewpoint(_curr_viewpoint),
	_light_dir_ws(normalize(float3(50, 1, 100.0)))
{
	init_model();
	init_materials();
	_curr_material = &_cat_material;

	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ZERO, GL_ONE);
	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
}

void Fur_simulation_opengl_example::init_materials()
{
	const Sampler_desc linear_sampler(GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT, GL_CLAMP_TO_EDGE);

	auto image_fur_mask = cg::data::load_image_tga("../data/fur_simulation/noise-texture-00.tga");
	_tex_fur_mask = Texture_2d(GL_R8, 1, linear_sampler, image_fur_mask);

	
	//{ // cat material
	//	auto image_diffuse_rgb = cg::data::load_image_tga("../data/fur_simulation/cat-diffuse-rgb.tga");
	//	_cat_material.tex_diffuse_rgb = Texture_2d(GL_RGB8, 1, linear_sampler, image_diffuse_rgb);
	//	_cat_material.shadow_factor_power = 1.1f;
	//	_cat_material.threshold_power = 0.6f;
	//	_cat_material.curl_radius = 0.01f;
	//	_cat_material.curl_frequency = 0.0f;
	//	_cat_material.tex_coord_factor = 15.0f;
	//	_cat_material.shell_count = 16;
	//}

	//{ // curly red material
	//	auto image_diffuse_rgb = cg::data::load_image_tga("../data/fur_simulation/red-diffuse-rgb.tga");
	//	_curly_red_material.tex_diffuse_rgb = Texture_2d(GL_RGB8, 1, linear_sampler, image_diffuse_rgb);
	//	_curly_red_material.shadow_factor_power = 0.6f;
	//	_curly_red_material.threshold_power = 0.6f;
	//	_curly_red_material.curl_radius = 0.2f;
	//	_curly_red_material.curl_frequency = 4.0f;
	//	_curly_red_material.tex_coord_factor = 8.0f;
	//	_curly_red_material.shell_count = 32;
	//}

	//{ // sheep material
	//	auto image_diffuse_rgb = cg::data::load_image_tga("../data/common_data/material-default-diffuse-rgb.tga");
	//	_sheep_material.tex_diffuse_rgb = Texture_2d(GL_RGB8, 1, linear_sampler, image_diffuse_rgb);
	//	_sheep_material.shadow_factor_power = 1.0f;
	//	_sheep_material.threshold_power = 1.0f;
	//	_sheep_material.curl_radius = 0.16f;
	//	_sheep_material.curl_frequency = 5.0f;
	//	_sheep_material.tex_coord_factor = 4.0f;
	//	_sheep_material.shell_count = 32;
	//}

	{ // cat material
		auto image_diffuse_rgb = cg::data::load_image_tga("../data/fur_simulation/cat-diffuse-rgb.tga");
		_cat_material.tex_diffuse_rgb = Texture_2d(GL_RGB8, 1, linear_sampler, image_diffuse_rgb);
		_cat_material.shadow_factor_power = 1.1f;
		_cat_material.threshold_power = 0.6f;
		_cat_material.curl_radius = 0.01f;
		_cat_material.curl_frequency = 0.0f;
		_cat_material.tex_coord_factor = 3.0f;
		_cat_material.shell_count = 32;
	}

	{ // curly red material
		auto image_diffuse_rgb = cg::data::load_image_tga("../data/fur_simulation/red-diffuse-rgb.tga");
		_curly_red_material.tex_diffuse_rgb = Texture_2d(GL_RGB8, 1, linear_sampler, image_diffuse_rgb);
		_curly_red_material.shadow_factor_power = 0.6f;
		_curly_red_material.threshold_power = 0.6f;
		_curly_red_material.curl_radius = 0.01f;
		_curly_red_material.curl_frequency = 4.0f;
		_curly_red_material.tex_coord_factor = 2.0f;
		_curly_red_material.shell_count = 32;
	}

	{ // hair material
		auto image_diffuse_rgb = cg::data::load_image_tga("../data/fur_simulation/cat-diffuse-rgb.tga");
		_hair_material.tex_diffuse_rgb = Texture_2d(GL_RGB8, 1, linear_sampler, image_diffuse_rgb);
		_hair_material.shadow_factor_power = 2.0f;
		_hair_material.threshold_power = 0.05f;
		_hair_material.curl_radius = 0.07f;
		_hair_material.curl_frequency = 1.0f;
		_hair_material.tex_coord_factor = 1.0f;
		_hair_material.shell_count = 32;
	}

	//{ // sheep material
	//	auto image_diffuse_rgb = cg::data::load_image_tga("../data/common_data/material-default-diffuse-rgb.tga");
	//	_sheep_material.tex_diffuse_rgb = Texture_2d(GL_RGB8, 1, linear_sampler, image_diffuse_rgb);
	//	_sheep_material.shadow_factor_power = 1.0f;
	//	_sheep_material.threshold_power = 1.5f;
	//	_sheep_material.curl_radius = 0.006f;
	//	_sheep_material.curl_frequency = 10.0f;
	//	_sheep_material.tex_coord_factor = 1.0f;
	//	_sheep_material.shell_count = 32;
	//}
}

void Fur_simulation_opengl_example::init_model()
{
	_model_matrix = scale_matrix<mat4>(float3(2.0f));

	//Square_model model;
	Arbitrary_model model(0.3f, "../data/sphere-20x20.obj");
	//Arbitrary_model model(5.0f, "c:/dev/meshes/mod_head_default.obj");
	auto geometry_data = model.get_geometry_data();
	_model_index_count = geometry_data.index_data().size();

	glCreateVertexArrays(1, &_vao_id);
	glBindVertexArray(_vao_id);

	// vertex buffer
	constexpr GLuint vb_binding_index = 0;

	_vertex_buffer = Buffer_gpu(geometry_data.vertex_data_byte_count(), geometry_data.vertex_data().data());
	glVertexArrayVertexBuffer(_vao_id, vb_binding_index, _vertex_buffer.id(), 0,
		geometry_data.vertex_byte_count());

	// position
	glEnableVertexArrayAttrib(_vao_id, 0);
	glVertexArrayAttribBinding(_vao_id, 0, vb_binding_index);
	glVertexArrayAttribFormat(_vao_id, 0, 3, GL_FLOAT, false, geometry_data.position_byte_offset());

	// normal
	glEnableVertexArrayAttrib(_vao_id, 1);
	glVertexArrayAttribBinding(_vao_id, 1, vb_binding_index);
	glVertexArrayAttribFormat(_vao_id, 1, 3, GL_FLOAT, false, geometry_data.normal_byte_offset());

	// tex_coord
	glEnableVertexArrayAttrib(_vao_id, 2);
	glVertexArrayAttribBinding(_vao_id, 2, vb_binding_index);
	glVertexArrayAttribFormat(_vao_id, 2, 2, GL_FLOAT, false, geometry_data.tex_coord_byte_offset());

	// tangent_h
	glEnableVertexArrayAttrib(_vao_id, 3);
	glVertexArrayAttribBinding(_vao_id, 3, vb_binding_index);
	glVertexArrayAttribFormat(_vao_id, 3, 4, GL_FLOAT, false, geometry_data.tangent_h_byte_offset());

	// strand_rest_position
	glEnableVertexArrayAttrib(_vao_id, 4);
	glVertexArrayAttribBinding(_vao_id, 4, vb_binding_index);
	glVertexArrayAttribFormat(_vao_id, 4, 3, GL_FLOAT, false, geometry_data.strand_rest_position_byte_offset());

	// strand_curr_position
	glEnableVertexArrayAttrib(_vao_id, 5);
	glVertexArrayAttribBinding(_vao_id, 5, vb_binding_index);
	glVertexArrayAttribFormat(_vao_id, 5, 3, GL_FLOAT, false, geometry_data.strand_curr_position_byte_offset());

	// index buffer
	_index_buffer = Buffer_gpu(geometry_data.index_data_byte_count(), geometry_data.index_data().data());
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _index_buffer.id());
}

void Fur_simulation_opengl_example::on_keyboard()
{
	if (_app_ctx.keyboard.is_down(Key::digit_1)) {
		_curr_material = &_cat_material;
	}
	else if(_app_ctx.keyboard.is_down(Key::digit_2)) {
		_curr_material = &_curly_red_material;
	}
	else if (_app_ctx.keyboard.is_down(Key::digit_3)) {
		_curr_material = &_hair_material;
	}
	else if (_app_ctx.keyboard.is_down(Key::digit_4)) {
		_curr_material = &_sheep_material;
	}
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
	float3 view_position = lerp(_prev_viewpoint.position, _curr_viewpoint.position, interpolation_factor);
	mat4 view_matrix = ::view_matrix(_prev_viewpoint, _curr_viewpoint, interpolation_factor);
	mat4 projection_view_matrix = _projection_matrix* view_matrix;

	const static float3 color = rgb(0x817c6e);
	glClearColor(color.r, color.g, color.b, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindVertexArray(_vao_id);
	glBindTextureUnit(0, _curr_material->tex_diffuse_rgb.id());
	glBindTextureUnit(1, _tex_fur_mask.id());

	_glsl_fur_generation.bind(_projection_matrix, view_matrix, _model_matrix,
		*_curr_material, _light_dir_ws, view_position);

	//glDrawElementsInstanced(GL_TRIANGLES, _model_index_count, GL_UNSIGNED_INT, 
	//	nullptr, _curr_material->shell_count);
	for (size_t i = 0; i < _curr_material->shell_count; ++i) {
		_glsl_fur_generation.set_params(i);
		glDrawElements(GL_TRIANGLES, _model_index_count, GL_UNSIGNED_INT, nullptr);
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

	//static float rot_angle = 0;

	//if (!approx_equal(_view_roll_angles.y, 0.0f)) {
	//	rot_angle += _view_roll_angles.y;
	//	_model_matrix = rotation_matrix_oy<mat4>(rot_angle) * scale_matrix<mat4>(float3(2.0f));
	//}

	_view_roll_angles = float2::zero;
}

void Fur_simulation_opengl_example::update_projection_matrix()
{
	_projection_matrix = perspective_matrix_opengl(cg::pi_3,
		_app_ctx.window.viewport_size().aspect_ratio(), 0.1f, 1000.0f);
}

} // fur_simulation
