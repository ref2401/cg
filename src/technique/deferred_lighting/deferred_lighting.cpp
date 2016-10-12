#include "technique/deferred_lighting/deferred_lighting.h"

#include <cmath>
#include <sstream>
#include <type_traits>
#include <utility>
#include "cg/base/base.h"
#include "cg/file/file.h"

using cg::data::Vertex_attribs;
using namespace cg;
using namespace cg::rnd::opengl;


namespace {

using deferred_lighting::Directional_light;
using deferred_lighting::Directional_light_params;
using deferred_lighting::Renderer_config;


Directional_light_params get_directional_light_params(const mat4& view_matrix, const Directional_light& dir_light) noexcept
{
	Directional_light_params p;
	p.projection_matrix = dir_light.projection_matrix;
	p.view_matrix = cg::view_matrix(dir_light.position, dir_light.target, float3::unit_y);
	p.direction_vs = normalize(mul(static_cast<mat3>(view_matrix), (dir_light.target - dir_light.position)));
	p.irradiance = dir_light.rgb * dir_light.intensity;
	p.ambient_irradiance_up = dir_light.rgb * dir_light.ambient_intensity;
	p.ambient_irradiance_down = 0.7f * p.ambient_irradiance_up;
	return p;
}

Renderer_config make_render_config(uint2 viewport_size)
{
	using cg::kilobytes;

	Renderer_config config;
	config.vertex_attrib_layout = Vertex_attrib_layout(0, 1, 2, 3);
	config.viewport_size = viewport_size;
	config.rect_1x1_mesh_data = cg::file::load_mesh_wavefront("../data/common_data/rect-1x1.obj", Vertex_attribs::position);
	config.gbuffer_pass_code = cg::file::load_glsl_program_source("../data/deferred_lighting_shaders/gbuffer_pass");
	config.shadow_map_pass_code = cg::file::load_glsl_program_source("../data/deferred_lighting_shaders/shadow_map_pass");
	config.lighting_pass_dir_code = cg::file::load_glsl_program_source("../data/deferred_lighting_shaders/lighting_pass_dir");
	config.material_pass_code = cg::file::load_glsl_program_source("../data/deferred_lighting_shaders/material_pass");

	return config;
}

} // namespace


namespace deferred_lighting {

// ----- Material -----

Material::Material(float smoothness,
	cg::rnd::opengl::Texture_2d_immut tex_diffuse_rgb,
	cg::rnd::opengl::Texture_2d_immut tex_normal_map,
	cg::rnd::opengl::Texture_2d_immut tex_specular_intensity) noexcept :
	smoothness(smoothness),
	tex_diffuse_rgb(std::move(tex_diffuse_rgb)),
	tex_normal_map(std::move(tex_normal_map)),
	tex_specular_intensity(std::move(tex_specular_intensity))
{
	assert(this->smoothness >= 0.0f);
	assert(this->tex_diffuse_rgb.id() != Invalid::texture_id);
	assert(this->tex_normal_map.id() != Invalid::texture_id);
	assert(this->tex_specular_intensity.id() != Invalid::texture_id);
}

// ----- Material_library -----

Material_library::Material_library()
{
	Sampler_config nearest_clamp_to_edge(Min_filter::nearest, Mag_filter::nearest, Wrap_mode::clamp_to_edge);
	Sampler_config nearest_repeat(Min_filter::nearest, Mag_filter::nearest, Wrap_mode::repeat);
	Sampler_config bilinear_clamp_to_edge(Min_filter::bilinear, Mag_filter::bilinear, Wrap_mode::clamp_to_edge);
	Sampler_config bilinear_repeat(Min_filter::bilinear, Mag_filter::bilinear, Wrap_mode::repeat);


	auto material_default_normal_map = cg::file::load_image_tga("../data/common_data/material-default-normal-map.tga");
	auto specular_intensity_0_18_image = cg::file::load_image_tga("../data/common_data/material-specular-intensity-0.18f.tga");
	auto specular_intensity_1_00_image = cg::file::load_image_tga("../data/common_data/material-specular-intensity-1.00f.tga");

	{ // default material
		auto diffuse_rgb_image = cg::file::load_image_tga("../data/common_data/material-default-diffuse-rgb.tga");

		_default_material.smoothness = 10.0f;
		_default_material.tex_diffuse_rgb = Texture_2d_immut(Texture_format::rgb_8, nearest_clamp_to_edge, diffuse_rgb_image);
		_default_material.tex_normal_map = Texture_2d_immut(Texture_format::rgb_8, nearest_clamp_to_edge, material_default_normal_map);
		_default_material.tex_specular_intensity = Texture_2d_immut(Texture_format::red_8, nearest_clamp_to_edge, specular_intensity_1_00_image);
	}

	{ // brick wall
		auto diffuse_rgb_image = cg::file::load_image_tga("../data/bricks-red-diffuse-rgb.tga");
		auto normal_map_image = cg::file::load_image_tga("../data/bricks-red-normal-map.tga");
		auto specular_image = cg::file::load_image_tga("../data/bricks-red-specular-intensity.tga");

		_brick_wall_material.smoothness = 8.0f;
		_brick_wall_material.tex_diffuse_rgb = Texture_2d_immut(Texture_format::rgb_8, bilinear_clamp_to_edge, diffuse_rgb_image);
		_brick_wall_material.tex_normal_map = Texture_2d_immut(Texture_format::rgb_8, nearest_clamp_to_edge, normal_map_image);
		_brick_wall_material.tex_specular_intensity = Texture_2d_immut(Texture_format::red_8, bilinear_clamp_to_edge, specular_image);
	}

	{ // chess board
		auto diffuse_rgb_image = cg::file::load_image_tga("../data/chess-board-diffuse-rgb.tga");

		_chess_board_material.smoothness = 1.0f;
		_chess_board_material.tex_diffuse_rgb = Texture_2d_immut(Texture_format::rgb_8, bilinear_repeat, diffuse_rgb_image);
		_chess_board_material.tex_normal_map = Texture_2d_immut(Texture_format::rgb_8, nearest_repeat, material_default_normal_map);
		_chess_board_material.tex_specular_intensity = Texture_2d_immut(Texture_format::red_8, bilinear_repeat, specular_intensity_0_18_image);
	}

	{ // wooden box
		auto diffuse_rgb_image = cg::file::load_image_tga("../data/wooden-box-diffuse-rgb.tga");
		auto normal_map_image = cg::file::load_image_tga("../data/wooden-box-normal-map.tga");
		auto specular_image = cg::file::load_image_tga("../data/wooden-box-specular-intensity.tga");

		_wooden_box_material.smoothness = 5.0f;
		_wooden_box_material.tex_diffuse_rgb = Texture_2d_immut(Texture_format::rgb_8, bilinear_clamp_to_edge, diffuse_rgb_image);
		_wooden_box_material.tex_normal_map = Texture_2d_immut(Texture_format::rgb_8, nearest_clamp_to_edge, normal_map_image);
		_wooden_box_material.tex_specular_intensity = Texture_2d_immut(Texture_format::red_8, bilinear_clamp_to_edge, specular_image);
	}
}

// ----- Deferred_lighting -----

Deferred_lighting::Deferred_lighting(cg::sys::Application_context_i& ctx) :
	Game(ctx),
	_projection_matrix(perspective_matrix(cg::pi_3, _ctx.window().size().aspect_ratio(), 1, 50)),
	_curr_viewpoint(float3(2, 2, 4), float3::zero),
	_prev_viewpoint(_curr_viewpoint),
	_renderer(make_render_config(_ctx.window().size())),
	_frame(16)
{
	// scene lights
	_dir_light.position = float3(-1000, 1000, -1000);
	_dir_light.target = float3::zero;
	_dir_light.rgb = float3::unit_xyz;
	_dir_light.intensity = 1.f;
	_dir_light.ambient_intensity = 0.25f;
	_dir_light.projection_matrix = orthographic_matrix(50, 50, 0, 2 * len(_dir_light.position - _dir_light.target));

	init_geometry();
	init_renderables();
}

void Deferred_lighting::begin_render(float blend_factor)
{
	assert(0.f <= blend_factor && blend_factor <= 1.f);

	_frame.reset(_vertex_spec0);

	_frame.projection_matrix = _projection_matrix;
	_frame.view_matrix = view_matrix(_prev_viewpoint, _curr_viewpoint, blend_factor);
	_frame.directional_light = get_directional_light_params(_frame.view_matrix, _dir_light);

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

void Deferred_lighting::init_geometry()
{
	struct Load_info {
		Load_info(const char* filename, DE_cmd* cmd, 
			size_t approx_vertex_count = 0, size_t approx_index_count = 0) noexcept :
			filename(filename), cmd(cmd), 
			approx_vertex_count(approx_vertex_count), 
			approx_index_count(approx_index_count)
		{}

		const char* filename;
		DE_cmd* cmd;
		size_t approx_vertex_count;
		size_t approx_index_count;
	};

	auto vertex_attribs = Vertex_attribs::mesh_tangent_h;
	std::vector<Load_info> load_info_list = {
		//Load_info("../data/teapot_base.obj", &_cmd_teapot_base, 36456, 36456),
		Load_info("../data/cube.obj", &_cmd_cube),
		Load_info("../data/rect_2x2_uv_repeat.obj", &_cmd_rect_2x2_repeat)
	};

	_vs_builder.begin(vertex_attribs, megabytes(4));

	for (auto& load_info : load_info_list) {
		auto mesh_data = cg::file::load_mesh_wavefront(load_info.filename, 
			vertex_attribs, load_info.approx_vertex_count, load_info.approx_index_count);
		
		*load_info.cmd = _vs_builder.push_back(mesh_data);
	}

	_vertex_spec0 = _vs_builder.end(_renderer.vertex_attrib_layout());
}

void Deferred_lighting::init_renderables()
{
	_rednerable_objects.reserve(16);

	// chess board
	_rednerable_objects.emplace_back(_cmd_rect_2x2_repeat,
		ts_matrix(float3::zero, float3(5)),
		_material_library.chess_board_material());

	// brick cubes
	_rednerable_objects.emplace_back(_cmd_cube,
		translation_matrix(float3(-1, 0.5f, -1)),
		_material_library.brick_wall_material());

	_rednerable_objects.emplace_back(_cmd_cube,
		translation_matrix(float3(-1, 1.5f, -1)),
		_material_library.brick_wall_material());

	// wooden cubes
	_rednerable_objects.emplace_back(_cmd_cube,
		translation_matrix(float3(-1, 2.5f, -1)),
		_material_library.wooden_box_material());

	_rednerable_objects.emplace_back(_cmd_cube,
		translation_matrix(float3(-2, 0.5f, -1)),
		_material_library.wooden_box_material());

	_rednerable_objects.emplace_back(_cmd_cube,
		translation_matrix(float3(-2, 0.5f, 0)),
		_material_library.wooden_box_material());

	_rednerable_objects.emplace_back(_cmd_cube,
		translation_matrix(float3(0, 0.5f, -1)),
		_material_library.wooden_box_material());

	_rednerable_objects.emplace_back(_cmd_cube,
		translation_matrix(float3(1, 0.5f, -1)),
		_material_library.wooden_box_material());
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

	//// mouse offset by x means rotation around OX (pitch)
	//if (y_offset_satisfies) {
	//	_view_roll_angles.x += (offset_ndc.y > 0.f) ? pi_64 : -pi_64;
	//}
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