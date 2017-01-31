#include "technique/fur_simulation/fur_simulation_opengl.h"

#include <cassert>
#include <limits>
#include <utility>
#include "cg/base/container.h"
#include "cg/data/image.h"
#include "cg/data/shader.h"


using namespace cg;
using cg::data::Image_2d;
using cg::data::Pixel_format;
using cg::sys::Key;


namespace fur_simulation {

// ----- Geometry_buffers -----

Geometry_buffers::Geometry_buffers(float strand_lenght, const char* geometry_filename)
{
	Model_geometry_data geometry(strand_lenght, geometry_filename);

	_tbo_position_buffer = Texture_buffer<Buffer_gpu>(GL_RGB32F, geometry.position_buffer);
	_tbo_physics_buffer_0 = Texture_buffer<Buffer_gpu>(GL_RGB32F, geometry.simulation_buffer);
	_tbo_physics_buffer_1 = Texture_buffer<Buffer_gpu>(GL_RGB32F, byte_count(geometry.simulation_buffer), nullptr);
	_model_attribs_buffer = Buffer_gpu(geometry.model_attribs_buffer);
	_index_buffer = Buffer_gpu(geometry.index_buffer);
	_meshes = std::move(geometry.meshes);

	for (const auto& mesh : _meshes)
		_vertex_count += mesh.vertex_count;
	assert(_vertex_count <= size_t(std::numeric_limits<GLsizei>::max()));

	glCreateVertexArrays(1, &_blank_vao_id);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, _tbo_physics_buffer_1.buffer().id());
	init_physics_simulation_0_vao(geometry.layout);
	init_physics_simulation_1_vao(geometry.layout);
	init_render_0_vao(geometry.layout);
	init_render_1_vao(geometry.layout);

	// debug_slot_buffer settings
	_tbo_debug_slot = Texture_buffer<Buffer_gpu>(GL_RGBA32F, _vertex_count * 4 * sizeof(float), nullptr);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, _tbo_debug_slot.buffer().id());
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
		_tbo_physics_buffer_0.buffer().id(), 0, layout.simulation_buffer_byte_stride);

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
		_tbo_physics_buffer_1.buffer().id(), 0, layout.simulation_buffer_byte_stride);

	// p_curr
	glEnableVertexArrayAttrib(_physics_1_vao_id, 2);
	glVertexArrayAttribBinding(_physics_1_vao_id, 2, simulation_buffer_binding_index);
	glVertexArrayAttribFormat(_physics_1_vao_id, 2, 3, GL_FLOAT, false, layout.p_curr_byte_offset);

	// velocity
	glEnableVertexArrayAttrib(_physics_1_vao_id, 3);
	glVertexArrayAttribBinding(_physics_1_vao_id, 3, simulation_buffer_binding_index);
	glVertexArrayAttribFormat(_physics_1_vao_id, 3, 3, GL_FLOAT, false, layout.velocity_byte_offset);
}

void Geometry_buffers::init_render_0_vao(const Model_geometry_layout& layout)
{
	constexpr GLuint position_buffer_binding_index = 0;
	constexpr GLuint simulation_buffer_binding_index = 1;
	constexpr GLuint model_attribs_buffer_binding_index = 2;

	glCreateVertexArrays(1, &_render_vao_0_id);

	// position_buffer
	glVertexArrayVertexBuffer(_render_vao_0_id, position_buffer_binding_index,
		_tbo_position_buffer.buffer().id(), 0, layout.position_buffer_byte_stride);

	// p_base
	glEnableVertexArrayAttrib(_render_vao_0_id, 0);
	glVertexArrayAttribBinding(_render_vao_0_id, 0, position_buffer_binding_index);
	glVertexArrayAttribFormat(_render_vao_0_id, 0, 3, GL_FLOAT, false, layout.p_base_byte_offset);

	// p_rest
	glEnableVertexArrayAttrib(_render_vao_0_id, 1);
	glVertexArrayAttribBinding(_render_vao_0_id, 1, position_buffer_binding_index);
	glVertexArrayAttribFormat(_render_vao_0_id, 1, 3, GL_FLOAT, false, layout.p_rest_byte_offset);


	// simulation_buffer
	glVertexArrayVertexBuffer(_render_vao_0_id, simulation_buffer_binding_index,
		_tbo_physics_buffer_0.buffer().id(), 0, layout.simulation_buffer_byte_stride);

	// p_curr
	glEnableVertexArrayAttrib(_render_vao_0_id, 2);
	glVertexArrayAttribBinding(_render_vao_0_id, 2, simulation_buffer_binding_index);
	glVertexArrayAttribFormat(_render_vao_0_id, 2, 3, GL_FLOAT, false, layout.p_curr_byte_offset);


	// model_attribs_buffer
	glVertexArrayVertexBuffer(_render_vao_0_id, model_attribs_buffer_binding_index,
		_model_attribs_buffer.id(), 0, layout.model_attribs_byte_stride);

	// normal
	glEnableVertexArrayAttrib(_render_vao_0_id, 3);
	glVertexArrayAttribBinding(_render_vao_0_id, 3, model_attribs_buffer_binding_index);
	glVertexArrayAttribFormat(_render_vao_0_id, 3, 3, GL_FLOAT, false, layout.normal_byte_offset);

	// tex_coord
	glEnableVertexArrayAttrib(_render_vao_0_id, 4);
	glVertexArrayAttribBinding(_render_vao_0_id, 4, model_attribs_buffer_binding_index);
	glVertexArrayAttribFormat(_render_vao_0_id, 4, 2, GL_FLOAT, false, layout.tex_coord_byte_offset);

	// tangent_h
	glEnableVertexArrayAttrib(_render_vao_0_id, 5);
	glVertexArrayAttribBinding(_render_vao_0_id, 5, model_attribs_buffer_binding_index);
	glVertexArrayAttribFormat(_render_vao_0_id, 5, 4, GL_FLOAT, false, layout.tangent_h_byte_offset);


	// index_buffer
	glVertexArrayElementBuffer(_render_vao_0_id, _index_buffer.id());
}

void Geometry_buffers::init_render_1_vao(const Model_geometry_layout& layout)
{
	constexpr GLuint position_buffer_binding_index = 0;
	constexpr GLuint simulation_buffer_binding_index = 1;
	constexpr GLuint model_attribs_buffer_binding_index = 2;

	glCreateVertexArrays(1, &_render_vao_1_id);

	// position_buffer
	glVertexArrayVertexBuffer(_render_vao_1_id, position_buffer_binding_index,
		_tbo_position_buffer.buffer().id(), 0, layout.position_buffer_byte_stride);

	// p_base
	glEnableVertexArrayAttrib(_render_vao_1_id, 0);
	glVertexArrayAttribBinding(_render_vao_1_id, 0, position_buffer_binding_index);
	glVertexArrayAttribFormat(_render_vao_1_id, 0, 3, GL_FLOAT, false, layout.p_base_byte_offset);

	// p_rest
	glEnableVertexArrayAttrib(_render_vao_1_id, 1);
	glVertexArrayAttribBinding(_render_vao_1_id, 1, position_buffer_binding_index);
	glVertexArrayAttribFormat(_render_vao_1_id, 1, 3, GL_FLOAT, false, layout.p_rest_byte_offset);


	// simulation_buffer
	glVertexArrayVertexBuffer(_render_vao_1_id, simulation_buffer_binding_index,
		_tbo_physics_buffer_1.buffer().id(), 0, layout.simulation_buffer_byte_stride);

	// p_curr
	glEnableVertexArrayAttrib(_render_vao_1_id, 2);
	glVertexArrayAttribBinding(_render_vao_1_id, 2, simulation_buffer_binding_index);
	glVertexArrayAttribFormat(_render_vao_1_id, 2, 3, GL_FLOAT, false, layout.p_curr_byte_offset);


	// model_attribs_buffer
	glVertexArrayVertexBuffer(_render_vao_1_id, model_attribs_buffer_binding_index,
		_model_attribs_buffer.id(), 0, layout.model_attribs_byte_stride);

	// normal
	glEnableVertexArrayAttrib(_render_vao_1_id, 3);
	glVertexArrayAttribBinding(_render_vao_1_id, 3, model_attribs_buffer_binding_index);
	glVertexArrayAttribFormat(_render_vao_1_id, 3, 3, GL_FLOAT, false, layout.normal_byte_offset);

	// tex_coord
	glEnableVertexArrayAttrib(_render_vao_1_id, 4);
	glVertexArrayAttribBinding(_render_vao_1_id, 4, model_attribs_buffer_binding_index);
	glVertexArrayAttribFormat(_render_vao_1_id, 4, 2, GL_FLOAT, false, layout.tex_coord_byte_offset);

	// tangent_h
	glEnableVertexArrayAttrib(_render_vao_1_id, 5);
	glVertexArrayAttribBinding(_render_vao_1_id, 5, model_attribs_buffer_binding_index);
	glVertexArrayAttribFormat(_render_vao_1_id, 5, 4, GL_FLOAT, false, layout.tangent_h_byte_offset);


	// index_buffer
	glVertexArrayElementBuffer(_render_vao_1_id, _index_buffer.id());
}

void Geometry_buffers::swap_physics_source_dest_buffers() noexcept
{ 
	_read_from_physics_0 = !_read_from_physics_0; 

	// if data is read from #0 then tb points to #1
	GLuint tf_buffer_id = (_read_from_physics_0) 
		? _tbo_physics_buffer_1.buffer().id()
		: _tbo_physics_buffer_0.buffer().id();

	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, tf_buffer_id);
}

// ----- Material_gallery -----

Material_gallery::Material_gallery()
{
	const Sampler_desc fur_mask_sampler(GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT, GL_CLAMP_TO_EDGE);
	
	{ // fur mask
		Image_2d image_fur_mask("../data/fur_simulation/noise-texture.png");
		_tex_fur_mask = Texture_2d_immut(GL_R8, 1, fur_mask_sampler, image_fur_mask);
	}

	const Sampler_desc diffuse_rgb_sampler(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE);
	{ // cat material
		Image_2d image_diffuse_rgb("../data/fur_simulation/cat-diffuse-rgb.png");
		_tex_car_diffuse_rgb = Texture_2d_immut(GL_RGB8, 1, diffuse_rgb_sampler, image_diffuse_rgb);
		
		_cat_material = std::make_unique<Material>(_tex_car_diffuse_rgb, _tex_fur_mask,
			Strand_properties(
			/* curl_radius */				0.0f,
			/* curl_frequency */			0.0,
			/* shadow_factor_power */		0.6f,
			/* shell_count */				32,
			/* threshold_power */			0.8f,
			/* fur_mask_uv_min_factor */	1.0f,
			/* fur_mask_uv_max_factor */	3.0f,
			/* mass */						0.05f,
			/* k */							4.0f,
			/* damping */					0.5f));
	}

	{ // curly red material
		Image_2d image_diffuse_rgb("../data/fur_simulation/red-diffuse-rgb.png");
		_tex_red_curl_material = Texture_2d_immut(GL_RGB8, 1, diffuse_rgb_sampler, image_diffuse_rgb);

		_curly_red_material = std::make_unique<Material>(_tex_red_curl_material, _tex_fur_mask,
			Strand_properties(
			/* curl_radius */				0.007f,
			/* curl_frequency */			4.0,
			/* shadow_factor_power */		0.6f,
			/* shell_count */				32,
			/* threshold_power */			0.6f,
			/* fur_mask_uv_min_factor */	2.0f,
			/* fur_mask_uv_max_factor */	2.0f,
			/* mass */						0.1f,
			/* k */							100.5f,
			/* damping */					5.5f));
	}

	{ // bunny material
		Image_2d image_diffuse_rgb("../data/fur_simulation/bunny-diffuse-rgb.png");
		_tex_bunny_diffuse_rgb = Texture_2d_immut(GL_RGB8, 1, diffuse_rgb_sampler, image_diffuse_rgb);

		_bunny_material = std::make_unique<Material>(_tex_bunny_diffuse_rgb, _tex_fur_mask,
			Strand_properties(
			/* curl_radius */				0.07f,
			/* curl_frequency */			8.0,
			/* shadow_factor_power */		1.0f,
			/* shell_count */				64,
			/* threshold_power */			1.1f,
			/* fur_mask_uv_min_factor */	0.5f,
			/* fur_mask_uv_max_factor */	3.0f,
			/* mass */						0.1f,
			/* k */							1.0f,
			/* damping */					0.5f));
	}
}

// ----- Fur_pass -----

Fur_pass::Fur_pass(const uint2& viewport_size)
{
	_tex_rt = Texture_2d_immut(GL_RGBA32F, 1, viewport_size);
	_fbo.attach_color_target(GL_COLOR_ATTACHMENT0, _tex_rt);
	_fbo.set_draw_buffer(GL_COLOR_ATTACHMENT0);
	_fbo.set_read_buffer(GL_COLOR_ATTACHMENT0);
	_fbo.validate();
}

void Fur_pass::perform(const Geometry_buffers& geometry_buffers, const Material& material,
	const mat4& pvm_matrix, const mat4& model_matrix,
	const float3& view_position_ws, const float3& light_dir_ws) noexcept
{
	glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ZERO, GL_ONE);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);

	glBindTextureUnit(0, material.tex_diffuse_rgb().id());
	glBindTextureUnit(1, material.tex_fur_mask().id());
	glBindVertexArray(geometry_buffers.render_vao_id());

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbo.id());
	//glViewport(0, 0, _tex_rt.size().width, _tex_rt.size().height);
	_program.bind(pvm_matrix, model_matrix, view_position_ws, material.strand_props(), light_dir_ws);

	for (size_t mi = 0; mi < geometry_buffers.meshes().size(); ++mi) {
		glDrawElementsInstancedBaseVertex(GL_TRIANGLES, geometry_buffers.meshes()[mi].index_count,
			GL_UNSIGNED_INT, nullptr, material.strand_props().shell_count, geometry_buffers.meshes()[mi].base_vertex);
	}

	/*size_t draw_count = material.strand_props().shell_count;
	for (size_t si = 0; si < draw_count; ++si) {
		_program.set_shell_index(si);

		for (size_t mi = 0; mi < geometry_buffers.meshes().size(); ++mi) {
			glDrawElements(GL_TRIANGLES, geometry_buffers.meshes()[mi].index_count, GL_UNSIGNED_INT, nullptr);
		}
	}*/

	glBindTextureUnit(0, Invalid::texture_id);
	glBindTextureUnit(1, Invalid::texture_id);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, Invalid::framebuffer_id);
	/*glBlitNamedFramebuffer(_fbo.id(), Invalid::framebuffer_id,
		0, 0, _tex_rt.size().width, _tex_rt.size().height,
		0, 0, _tex_rt.size().width, _tex_rt.size().height,
		GL_COLOR_BUFFER_BIT, GL_LINEAR);*/

	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
}

// ----- Opaque_model_pass -----

void Opaque_model_pass::perform(const Geometry_buffers& geometry_buffers, const Material& material,
	const cg::mat4& projection_view_matrix, const cg::mat4& model_matrix, const cg::float3& dir_to_light_ws) noexcept
{
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	glBindTextureUnit(0, material.tex_diffuse_rgb().id());
	
	glBindVertexArray(geometry_buffers.render_vao_id());
	_program.bind(projection_view_matrix, model_matrix, dir_to_light_ws);

	for (size_t mi = 0; mi < geometry_buffers.meshes().size(); ++mi) {
		glDrawElementsBaseVertex(GL_TRIANGLES, geometry_buffers.meshes()[mi].index_count,
			GL_UNSIGNED_INT, nullptr, geometry_buffers.meshes()[mi].base_vertex);
	}

	glBindTextureUnit(0, Invalid::texture_id);

	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
}

// ----- Physics_simulation_pass -----

void Physics_simulation_pass::perform(Geometry_buffers& geometry_buffers, 
	const cg::float4& extern_accel_ms, const cg::float3& angular_accel_ms,
	float strand_length, const Strand_properties& strand_props) noexcept
{
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_RASTERIZER_DISCARD);

	glBindVertexArray(geometry_buffers.physics_vao_id());
	_program.bind(extern_accel_ms, angular_accel_ms,
		float4(strand_length, strand_props.mass, strand_props.k, strand_props.c));


	glBeginTransformFeedback(GL_POINTS);
		glDrawArrays(GL_POINTS, 0, geometry_buffers.vertex_count());
	glEndTransformFeedback();
	
	glBindVertexArray(Invalid::vao_id);
	geometry_buffers.swap_physics_source_dest_buffers();
	
	glDisable(GL_RASTERIZER_DISCARD);
	glEnable(GL_DEPTH_TEST);
}

// ----- Strand_debug_pass -----

void Strand_debug_pass::perform(Geometry_buffers& geometry_buffers, const cg::mat4& pvm_matrix) noexcept
{
	glEnable(GL_DEPTH_TEST);

	glBindTextureUnit(0, geometry_buffers.tbo_position_buffer().id());
	glBindTextureUnit(1, geometry_buffers.tbo_physics_buffer().id());
	glBindTextureUnit(2, geometry_buffers.tbo_debug_slot_buffer().id());
	glBindVertexArray(geometry_buffers.blank_vao_id());
	
	_program.bind(pvm_matrix);
	glDrawArrays(GL_LINES, 0, 4 * geometry_buffers.vertex_count());

	glDisable(GL_DEPTH_TEST);
	glBindTextureUnit(0, Invalid::texture_id);
	glBindTextureUnit(1, Invalid::texture_id);
	glBindTextureUnit(2, Invalid::texture_id);
	glBindVertexArray(Invalid::vao_id);
}

// ----- Fur_simulation_opengl_example -----

Fur_simulation_opengl_example::Fur_simulation_opengl_example(const cg::sys::App_context& app_ctx) :
	Example(app_ctx),
	_curr_viewpoint(float3(0, 3, 7), float3(0, 0, 0)),
	_prev_viewpoint(_curr_viewpoint),
	_model_transform(float3(0, -0.5, 0), normalize(float3(0, 1, 0)), float3(2.0f)),
	//_geometry_buffers(0.3f, "../data/rect_2x2.obj"),
	//_geometry_buffers(0.3f, "../data/sphere-20x20.obj"),
	_geometry_buffers(0.1f, "../data/models/bunny.obj"),
	_dir_to_light_ws(normalize(float3(50, 1, 100.0))),
	_fur_pass(_app_ctx.window.viewport_size())
{
	update_projection_matrix();
	_curr_material = &_material_gallery.cat_material();
}

void Fur_simulation_opengl_example::on_keyboard()
{
	/*if (_app_ctx.keyboard.is_down(Key::f)) {
		_wind_acceleration = 10.0f * normalize(-float3(0, 1, -1));
	}
	else {
		_wind_acceleration = float3::zero;
	}*/

	if (_app_ctx.keyboard.is_down(Key::r)) {
		_model_transform.rotation_angle_total += pi_8;
		_model_transform.rotation_angle = 5.6f * pi_8;
	}
	else {
		_model_transform.rotation_angle = 0.0f;
	}
	
	float3 movement_direction;
	if (_app_ctx.keyboard.is_down(Key::w))	movement_direction += float3::unit_y;
	if (_app_ctx.keyboard.is_down(Key::s))	movement_direction -= float3::unit_y;
	if (_app_ctx.keyboard.is_down(Key::a))	movement_direction -= float3::unit_x;
	if (_app_ctx.keyboard.is_down(Key::d))	movement_direction += float3::unit_x;
	_movement_acceleration = 5.0f * normalize(movement_direction);

	if (_app_ctx.keyboard.is_down(Key::space)) {
		_movement_speed = float3::zero;
		_movement_acceleration = float3::zero;
		_model_transform.position = float3(0, -0.5, 0);
	}

	if (_app_ctx.keyboard.is_down(Key::digit_1)) {
		_curr_material = &_material_gallery.cat_material();
	}
	else if(_app_ctx.keyboard.is_down(Key::digit_2)) {
		_curr_material = &_material_gallery.curly_red_material();
	}
	else if (_app_ctx.keyboard.is_down(Key::digit_3)) {
		_curr_material = &_material_gallery.bunny_material();
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
	const float3 view_position = lerp(_prev_viewpoint.position, _curr_viewpoint.position, interpolation_factor);
	const mat4 view_matrix = ::view_matrix(_prev_viewpoint, _curr_viewpoint, interpolation_factor);
	const mat4 projection_view_matrix = _projection_matrix* view_matrix;
	const mat4 pvm_matrix = projection_view_matrix * _model_matrix;
	_prev_viewpoint = _curr_viewpoint;

	static const float3 color = rgb(0x817c6e);
	glClearColor(color.r, color.g, color.b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//_opaque_model_pass.perform(_geometry_buffers, *_curr_material,
	//	projection_view_matrix, _model_matrix, _dir_to_light_ws);

	_fur_pass.perform(_geometry_buffers, *_curr_material,
		pvm_matrix, _model_matrix, view_position, _dir_to_light_ws);
	//_strand_debug_pass.perform(_geometry_buffers, pvm_matrix);
}

void Fur_simulation_opengl_example::update(float dt_msec)
{
	const float dt = dt_msec / 1000.0f;
	const float3 angular_velocity = _model_transform.rotation_angle * _model_transform.rotation_axis;
	update_curr_viewpoint();

	_movement_speed += _movement_acceleration * dt;
	_model_transform.position += 0.008f * _movement_speed;
	_model_matrix = trs_matrix(_model_transform.position,
		from_axis_angle_rotation(_model_transform.rotation_axis, 0.1f * _model_transform.rotation_angle_total),
		_model_transform.scale);

	const float3 external_accelerations = float3(0.0f, -9.81f, 0.0f) - _movement_acceleration;
	const mat3 inv_model_matrix(_model_matrix);
	const float4 extern_accel_ms(mul(inv_model_matrix, external_accelerations), dt);
	const float3 angular_ms(mul(inv_model_matrix, -angular_velocity));

	_physics_pass.perform(_geometry_buffers, extern_accel_ms, angular_ms,
		0.1f, _curr_material->strand_props());
}

void Fur_simulation_opengl_example::update_curr_viewpoint()
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
		_app_ctx.window.viewport_size().aspect_ratio(), 0.1f, 1000.0f);
}

} // fur_simulation
