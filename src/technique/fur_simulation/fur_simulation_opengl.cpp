#include "technique/fur_simulation/fur_simulation_opengl.h"

#include <cassert>
#include <limits>
#include <utility>
#include "cg/base/container.h"
#include "cg/data/image.h"
#include "cg/data/shader.h"


using namespace cg;
using cg::data::Image_2d;
using cg::data::Image_format;
using cg::sys::Key;


namespace fur_simulation {

// ----- Geometry_buffers -----

Geometry_buffers::Geometry_buffers(float strand_lenght, const char* geometry_filename)
{
	Model_geometry_data geometry(strand_lenght, geometry_filename);

	_tbo_position_buffer = Texture_buffer<Buffer_gpu>(GL_RGB32F, geometry.position_buffer);
	_tbo_simulation_buffer_0 = Texture_buffer<Buffer_gpu>(GL_RGB32F, geometry.simulation_buffer);
	_tbo_simulation_buffer_1 = Texture_buffer<Buffer_gpu>(GL_RGB32F, byte_count(geometry.simulation_buffer), nullptr);
	_model_attribs_buffer = Buffer_gpu(geometry.model_attribs_buffer);
	_index_buffer = Buffer_gpu(geometry.index_buffer);
	_meshes = std::move(geometry.meshes);

	for (const auto& mesh : _meshes)
		_vertex_count += mesh.vertex_count;
	assert(_vertex_count <= size_t(std::numeric_limits<GLsizei>::max()));

	glCreateVertexArrays(1, &_blank_vao_id);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, _tbo_simulation_buffer_1.buffer().id());
	init_physics_simulation_0_vao(geometry.layout);
	init_physics_simulation_1_vao(geometry.layout);
	// index buffer glVetexArrayElementBuffer
}

void Geometry_buffers::init_physics_simulation_0_vao(const Model_geometry_layout& layout)
{
	constexpr GLuint position_buffer_binding_index = 0;
	constexpr GLuint simulation_buffer_binding_index = 1;

	glCreateVertexArrays(1, &_physics_0_vao_id);

	// position_buffer
	glVertexArrayVertexBuffer(_physics_0_vao_id, position_buffer_binding_index, 
		_tbo_position_buffer.buffer().id(), 0, layout.position_buffer_byte_stride);


	// p_base
	glEnableVertexArrayAttrib(_physics_0_vao_id, 0);
	glVertexArrayAttribBinding(_physics_0_vao_id, 0, position_buffer_binding_index);
	glVertexArrayAttribFormat(_physics_0_vao_id, 0, 3, GL_FLOAT, false, layout.p_base_byte_offset);

	// p_rest
	glEnableVertexArrayAttrib(_physics_0_vao_id, 1);
	glVertexArrayAttribBinding(_physics_0_vao_id, 1, position_buffer_binding_index);
	glVertexArrayAttribFormat(_physics_0_vao_id, 1, 3, GL_FLOAT, false, layout.p_rest_byte_offset);
	
	// simulation_buffer
	glVertexArrayVertexBuffer(_physics_0_vao_id, simulation_buffer_binding_index,
		_tbo_simulation_buffer_0.buffer().id(), 0, layout.simulation_buffer_byte_stride);


	// p_curr
	glEnableVertexArrayAttrib(_physics_0_vao_id, 2);
	glVertexArrayAttribBinding(_physics_0_vao_id, 2, simulation_buffer_binding_index);
	glVertexArrayAttribFormat(_physics_0_vao_id, 2, 3, GL_FLOAT, false, layout.p_curr_byte_offset);

	// velocity
	glEnableVertexArrayAttrib(_physics_0_vao_id, 3);
	glVertexArrayAttribBinding(_physics_0_vao_id, 3, simulation_buffer_binding_index);
	glVertexArrayAttribFormat(_physics_0_vao_id, 3, 3, GL_FLOAT, false, layout.velocity_byte_offset);
}

void Geometry_buffers::init_physics_simulation_1_vao(const Model_geometry_layout& layout)
{
	constexpr GLuint position_buffer_binding_index = 0;
	constexpr GLuint simulation_buffer_binding_index = 1;

	glCreateVertexArrays(1, &_physics_1_vao_id);

	// position_buffer
	glVertexArrayVertexBuffer(_physics_1_vao_id, position_buffer_binding_index,
		_tbo_position_buffer.buffer().id(), 0, layout.position_buffer_byte_stride);

	// p_base
	glEnableVertexArrayAttrib(_physics_1_vao_id, 0);
	glVertexArrayAttribBinding(_physics_1_vao_id, 0, position_buffer_binding_index);
	glVertexArrayAttribFormat(_physics_1_vao_id, 0, 3, GL_FLOAT, false, layout.p_base_byte_offset);

	// p_rest
	glEnableVertexArrayAttrib(_physics_1_vao_id, 1);
	glVertexArrayAttribBinding(_physics_1_vao_id, 1, position_buffer_binding_index);
	glVertexArrayAttribFormat(_physics_1_vao_id, 1, 3, GL_FLOAT, false, layout.p_rest_byte_offset);

	
	// simulation_buffer
	glVertexArrayVertexBuffer(_physics_1_vao_id, simulation_buffer_binding_index,
		_tbo_simulation_buffer_1.buffer().id(), 0, layout.simulation_buffer_byte_stride);

	// p_curr
	glEnableVertexArrayAttrib(_physics_1_vao_id, 2);
	glVertexArrayAttribBinding(_physics_1_vao_id, 2, simulation_buffer_binding_index);
	glVertexArrayAttribFormat(_physics_1_vao_id, 2, 3, GL_FLOAT, false, layout.p_curr_byte_offset);

	// velocity
	glEnableVertexArrayAttrib(_physics_1_vao_id, 3);
	glVertexArrayAttribBinding(_physics_1_vao_id, 3, simulation_buffer_binding_index);
	glVertexArrayAttribFormat(_physics_1_vao_id, 3, 3, GL_FLOAT, false, layout.velocity_byte_offset);
}

void Geometry_buffers::swap_physics_source_dest_buffers() noexcept
{ 
	_read_from_physics_0 = !_read_from_physics_0; 

	// if data is read from #0 then tb points to #1
	GLuint tf_buffer_id = (_read_from_physics_0)
		? (_tbo_simulation_buffer_1.buffer().id())
		: (_tbo_simulation_buffer_0.buffer().id());

	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, tf_buffer_id);
}

// ----- Physics_simulation_pass -----

Physics_simulation_pass::Physics_simulation_pass() 
{

}

void Physics_simulation_pass::perform(Geometry_buffers& geometry_buffers, 
	const cg::float4& graviy_ms, const cg::float4& strand_props) noexcept
{
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_RASTERIZER_DISCARD);

	glBindVertexArray(geometry_buffers.physics_vao_id());
	
	_program.bind(graviy_ms, strand_props);
	glBeginTransformFeedback(GL_POINTS);
		glDrawArrays(GL_POINTS, 0, geometry_buffers.vertex_count());
	glEndTransformFeedback();
	
	geometry_buffers.swap_physics_source_dest_buffers();
	glBindVertexArray(Invalid::vao_id);
	
	glDisable(GL_RASTERIZER_DISCARD);
	glEnable(GL_DEPTH_TEST);
}

// ----- Strand_debug_pass -----

void Strand_debug_pass::perform(Geometry_buffers& geometry_buffers, const cg::mat4& pvm_matrix) noexcept
{
	glEnable(GL_DEPTH_TEST);

	glBindTextureUnit(0, geometry_buffers.tbo_position_buffer().id());
	glBindTextureUnit(1, geometry_buffers.tbo_simulation_buffer().id());
	glBindVertexArray(geometry_buffers.blank_vao_id());
	
	_program.bind(pvm_matrix);
	glDrawArrays(GL_LINES, 0, 2 * geometry_buffers.vertex_count());

	glDisable(GL_DEPTH_TEST);
	glBindTextureUnit(0, Invalid::texture_id);
	glBindTextureUnit(1, Invalid::texture_id);
	glBindVertexArray(Invalid::vao_id);
}

// ----- Fur_simulation_opengl_example -----

Fur_simulation_opengl_example::Fur_simulation_opengl_example(const cg::sys::App_context& app_ctx) :
	Example(app_ctx),
	_curr_viewpoint(float3(0, 0, 7), float3(0, 0, 0)),
	_prev_viewpoint(_curr_viewpoint),
	// materil
	_geometry_buffers(0.3f /*material.strand_lenght*/, "../data/sphere-20x20.obj")
	//_geometry_buffers(0.3f /*material.strand_lenght*/, "../data/rect_2x2.obj")
{
	_model_matrix = scale_matrix<mat4>(float3(2.0f));

	update_projection_matrix();
}

void Fur_simulation_opengl_example::on_keyboard()
{
	/*if (_app_ctx.keyboard.is_down(Key::digit_1)) {
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
	}*/
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
	const float3 view_position = lerp(_prev_viewpoint.position, _curr_viewpoint.position, interpolation_factor);
	const mat4 view_matrix = ::view_matrix(_prev_viewpoint, _curr_viewpoint, interpolation_factor);
	const mat4 projection_view_matrix = _projection_matrix* view_matrix;
	const mat4 pvm_matrix = projection_view_matrix * _model_matrix;
	_prev_viewpoint = _curr_viewpoint;


	const float4 gravity_ms(
		normalize(mul(inverse(_model_matrix), -float3::unit_y).xyz()), 0.01f);
	const float4 strand_props(0.3f, 0.1f, 0.3f, 0.5f);

	_physics_pass.perform(_geometry_buffers, gravity_ms, strand_props);

	const static float3 color = rgb(0x817c6e);
	glClearColor(color.r, color.g, color.b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	_strand_debug_pass.perform(_geometry_buffers, pvm_matrix);

	
	/*glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ZERO, GL_ONE);
	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);*/
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
