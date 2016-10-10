#include "technique/deferred_lighting/deferred_lighting.h"

#include <cmath>
#include <sstream>
#include <type_traits>
#include <utility>
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
	config.gbuffer_pass_code = cg::file::load_glsl_program_source("../data/deferred_lighting_shaders/gbuffer_pass");
	config.lighting_pass_dir_code = cg::file::load_glsl_program_source("../data/deferred_lighting_shaders/lighting_pass_dir");
	config.material_pass_code = cg::file::load_glsl_program_source("../data/deferred_lighting_shaders/material_pass");

	return config;
}

} // namespace


namespace deferred_lighting {

// ----- Material -----

Material::Material(float smoothness,
	cg::opengl::Texture_2d_immut tex_diffuse_rgb,
	cg::opengl::Texture_2d_immut tex_normal_map,
	cg::opengl::Texture_2d_immut tex_specular_intensity) noexcept :
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
		auto normal_map_image = cg::file::load_image_tga("../data/chess-board-normal-map.tga");

		_chess_board_material.smoothness = 1.0f;
		_chess_board_material.tex_diffuse_rgb = Texture_2d_immut(Texture_format::rgb_8, bilinear_repeat, diffuse_rgb_image);
		_chess_board_material.tex_normal_map = Texture_2d_immut(Texture_format::rgb_8, nearest_repeat, normal_map_image);
		_chess_board_material.tex_specular_intensity = Texture_2d_immut(Texture_format::red_8, bilinear_repeat, specular_intensity_0_18_image);
	}
}

// ----- Deferred_lighting -----

Deferred_lighting::Deferred_lighting(cg::sys::Application_context_i& ctx) :
	Game(ctx),
	_projection_matrix(cg::perspective_matrix(cg::pi_3, _ctx.window().size().aspect_ratio(), 1, 50)),
	_curr_viewpoint(float3(2, 3, 3), float3::zero),
	_prev_viewpoint(_curr_viewpoint),
	_renderer(make_render_config(_ctx.window().size())),
	_frame(16)
{
	// scene lights
	_dir_light.position = float3(1000, 1000, 1000);
	_dir_light.target = float3::zero;
	_dir_light.rgb = float3::unit_xyz;
	_dir_light.intensity = 1.f;
	_dir_light.ambient_intensity = 0.25f;

	init_geometry();
	init_renderables();
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

void Deferred_lighting::init_geometry()
{
	// pair.first - filename
	// pair.second - cmd pointer.
	using Load_info = std::pair<const char*, DE_cmd*>;

	auto vertex_attribs = Vertex_attribs::mesh_tangent_h;
	std::vector<Load_info> load_info_list = {
		Load_info("../data/rect_2x2_uv_repeat.obj", &_cmd_rect_2x2_repeat),
		Load_info("../data/cube.obj", &_cmd_cube),
	};

	_vs_builder.begin(vertex_attribs, megabytes(4));

	for (auto& load_info : load_info_list) {
		auto mesh_data = cg::file::load_mesh_wavefront(load_info.first, vertex_attribs);
		*load_info.second = _vs_builder.push_back(mesh_data);
	}

	_vertex_spec0 = _vs_builder.end(_renderer.vertex_attrib_layout());
}

void Deferred_lighting::init_renderables()
{
	_rednerable_objects.reserve(16);

	_rednerable_objects.emplace_back(_cmd_rect_2x2_repeat,
		ts_matrix(float3::zero, float3(5)),
		_material_library.chess_board_material());

	_rednerable_objects.emplace_back(_cmd_cube,
		translation_matrix(float3(0, -1, 0)),
		_material_library.brick_wall_material());
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