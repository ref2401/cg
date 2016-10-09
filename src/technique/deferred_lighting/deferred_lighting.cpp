#include "technique/deferred_lighting/deferred_lighting.h"

#include <cmath>
#include <sstream>
#include <type_traits>
#include "cg/base/base.h"
#include "cg/file/file.h"

using cg::data::Vertex_attribs;
using namespace cg;
using namespace cg::opengl;


namespace {

using deferred_lighting::Renderer_config;

Renderer_config make_render_config(uint2 viewport_size)
{
	using cg::kilobytes;

	Renderer_config config;
	config.vertex_attrib_layout = Vertex_attrib_layout(0, 1, 2, 3);
	config.viewport_size = viewport_size;
	config.rect_1x1_mesh_data = cg::file::load_mesh_wavefront("../data/common_data/rect-1x1.obj", Vertex_attribs::position);
	config.gbuffer_pass_code = cg::file::load_glsl_program_source("../data/deferred_lighting/gbuffer_pass");
	config.lighting_pass_dir_code = cg::file::load_glsl_program_source("../data/deferred_lighting/lighting_pass_dir");
	config.material_pass_code = cg::file::load_glsl_program_source("../data/deferred_lighting/material_pass");

	return config;
}

} // namespace


namespace deferred_lighting {

// ----- Deferred_lighting -----

Deferred_lighting::Deferred_lighting(cg::sys::Application_context_i& ctx) :
	Game(ctx),
	_projection_matrix(cg::perspective_matrix(cg::pi_3, _ctx.window().size().aspect_ratio(), 1, 50)),
	_curr_viewpoint(float3(1, 0, 5), float3::zero),
	_prev_viewpoint(_curr_viewpoint),
	_renderer(make_render_config(_ctx.window().size())),
	_frame(16)
{
	using cg::from_axis_angle_rotation;
	using cg::megabytes;
	using cg::tr_matrix;
	using cg::translation_matrix;


	// scene
	{ // default material
		auto diffuse_rgb_image = cg::file::load_image_tga("../data/common_data/material_default_diffuse_rgb.tga");
		auto normal_map_image = cg::file::load_image_tga("../data/common_data/material_default_normal_map.tga");
		auto specular_image = cg::file::load_image_tga("../data/common_data/material_default_specular_rgb.tga");

		_material_default.smoothness = 10.0f;
		_material_default.tex_diffuse_rgb = Texture_2d_immut(Texture_format::rgb_8, diffuse_rgb_image);
		_material_default.tex_normal_map = Texture_2d_immut(Texture_format::rgb_8, normal_map_image);
		_material_default.tex_specular_rgb = Texture_2d_immut(Texture_format::rgb_8, specular_image);
	}

	{ // brick wall
		auto diffuse_rgb_image = cg::file::load_image_tga("../data/bricks-red-diffuse.tga");
		auto normal_map_image = cg::file::load_image_tga("../data/bricks-red-normal-map.tga");
		auto specular_image = cg::file::load_image_tga("../data/bricks-red-specular.tga");

		_material_brick_wall.smoothness = 3.0f;
		_material_brick_wall.tex_diffuse_rgb = Texture_2d_immut(Texture_format::rgb_8, diffuse_rgb_image);
		_material_brick_wall.tex_normal_map = Texture_2d_immut(Texture_format::rgb_8, normal_map_image);
		_material_brick_wall.tex_specular_rgb = Texture_2d_immut(Texture_format::rgb_8, specular_image);
	}

	// cube geometry
	auto vertex_attribs = Vertex_attribs::mesh_textured | Vertex_attribs::normal;
	auto cube_mesh_data = cg::file::load_mesh_wavefront("../data/cube.obj", vertex_attribs);
	auto square_mesh_data = cg::file::load_mesh_wavefront("../data/square_03.obj", vertex_attribs);
	_vs_builder.begin(vertex_attribs, megabytes(4));
	DE_cmd cube_cmd = _vs_builder.push_back(cube_mesh_data);
	DE_cmd square_cmd = _vs_builder.push_back(square_mesh_data);
	// ... load other geometry ...
	_vertex_spec0 = _vs_builder.end(_renderer.vertex_attrib_layout());

	// scene objects
	_dir_light.position = float3(1000);
	_dir_light.target = float3::zero;
	_dir_light.rgb = float3::unit_xyz;
	_dir_light.intensity = 1.f;
	_dir_light.ambient_intensity = 0.35f;

	_rednerable_objects.reserve(16);
	_rednerable_objects.emplace_back(cube_cmd, 
		trs_matrix(float3::zero, from_axis_angle_rotation(float3::unit_y, cg::pi_4), float3(2)),
		_material_brick_wall);
	
	_rednerable_objects.emplace_back(square_cmd, 
		translation_matrix(float3(-2.f, 1, 1)),
		_material_brick_wall);

	_rednerable_objects.emplace_back(square_cmd, 
		tr_matrix(float3(2.f, -1, 1), from_axis_angle_rotation(float3::unit_y, cg::pi_8)),
		_material_default);
}

void Deferred_lighting::begin_render(float blend_factor)
{
	assert(0.f <= blend_factor && blend_factor <= 1.f);

	_frame.reset(_vertex_spec0);

	_frame.set_projection_matrix(_projection_matrix);
	auto viewpoint = lerp(_prev_viewpoint, _curr_viewpoint, blend_factor);
	_frame.set_view_matrix(viewpoint.view_matrix());
	_frame.set_directional_light(_dir_light);

	for (const auto& rnd : _rednerable_objects) {
		_frame.push_back_renderable(rnd);
	}

	_frame.begin_rendering();
}

void Deferred_lighting::end_render()
{
	_frame.end_rendering();
	_prev_viewpoint = _curr_viewpoint;
}

void Deferred_lighting::on_mouse_move()
{
	float2 pos_ndc = _ctx.mouse().get_ndc_position(_ctx.window().size());
	float2 offset_ndc = pos_ndc - _prev_mouse_pos_ndc;
	_prev_mouse_pos_ndc = pos_ndc;


	if (!_ctx.mouse().middle_down() || offset_ndc == float2::zero) return;

	bool y_offset_satisfies = !approx_equal(offset_ndc.y, 0.f, 0.01f);
	bool x_offset_satisfies = !approx_equal(offset_ndc.x, 0.f, (y_offset_satisfies) ? 0.01f : 0.001f);

	// mouse offset by x means rotation around OY (yaw)
	if (x_offset_satisfies) {
		_view_roll_angles.y += (offset_ndc.x > 0.f) ? -pi_64 : pi_64;
	}

	// mouse offset by x means rotation around OX (pitch)
	if (y_offset_satisfies) {
		_view_roll_angles.x += (offset_ndc.y > 0.f) ? pi_64 : -pi_64;
	}
}

void Deferred_lighting::on_window_resize()
{
	_projection_matrix = cg::perspective_matrix(cg::pi_3, _ctx.window().size().aspect_ratio(), 1, 50);
	_renderer.resize_viewport(_ctx.window().size());
}

void Deferred_lighting::render()
{
	_renderer.render(_frame);
}

void Deferred_lighting::update(float dt)
{
	float3 forward = _curr_viewpoint.forward();
	
	if (_view_roll_angles != float2::zero) {

		if (!approx_equal(_view_roll_angles.y, 0.0f)) {
			quat q = from_axis_angle_rotation(_curr_viewpoint.up, _view_roll_angles.y);
			
			float3 new_forward = normalize(rotate(q, forward));
			_curr_viewpoint.position = -new_forward * _curr_viewpoint.distance();
			
			forward = new_forward;
		}

		if (!approx_equal(_view_roll_angles.x, 0.0f)) {
			float3 ox = cross(forward, _curr_viewpoint.up);
			ox.y = 0; // ox is always parallel the world's OX.
			ox = normalize(ox);

			quat q = from_axis_angle_rotation(ox, _view_roll_angles.x);
			float3 new_forward = normalize(rotate(q, forward));
			_curr_viewpoint.up = normalize(cross(ox, new_forward));
			_curr_viewpoint.position = -new_forward * _curr_viewpoint.distance();
		}
	}

	_view_roll_angles = float2::zero;
}

} // namespace deferred_lighting