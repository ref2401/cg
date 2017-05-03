#include "technique/deferred_lighting/deferred_lighting.h"

#include <cmath>
#include <sstream>
#include <type_traits>
#include <utility>
#include "cg/base/base.h"
#include "cg/data/image.h"
#include "cg/data/model.h"
#include "cg/data/shader.h"


using cg::data::image_2d;
using cg::data::Vertex_attribs;
using namespace cg;
using namespace cg::rnd::opengl;


namespace {

using deferred_lighting::Directional_light;
using deferred_lighting::Directional_light_params;
using deferred_lighting::Renderer_config;
using deferred_lighting::Vertex_attrib_layout;


Directional_light_params get_directional_light_params(const float4x4& view_matrix, const Directional_light& dir_light) noexcept
{
	Directional_light_params p;
	p.projection_matrix = dir_light.projection_matrix;
	p.view_matrix = math::view_matrix(dir_light.position, dir_light.target, float3::unit_y);
	p.direction_vs = normalize(mul(static_cast<float3x3>(view_matrix), (dir_light.target - dir_light.position)));
	p.irradiance = dir_light.rgb * dir_light.intensity;
	p.ambient_irradiance_up = dir_light.rgb * dir_light.ambient_intensity;
	p.ambient_irradiance_down = 0.7f * p.ambient_irradiance_up;
	return p;
}

Renderer_config make_render_config(uint2 viewport_size)
{
	using cg::data::Vertex_attribs;
	using cg::kilobytes;

	Renderer_config config;
	config.vertex_attrib_layout = Vertex_attrib_layout(0, 1, 2, 3);
	config.viewport_size = viewport_size;
	config.rect_1x1_mesh_data = cg::data::load_model<Vertex_attribs::p_tc>("../../data/common_data/rect-1x1.obj");
	config.gbuffer_pass_code = cg::data::load_glsl_program_desc("gbuffer-pass-shader", "../../data/deferred_lighting_shaders/gbuffer_pass");
	config.lighting_pass_dir_code = cg::data::load_glsl_program_desc("lighting-pass-dir-shader", "../../data/deferred_lighting_shaders/lighting_pass_dir");
	config.shadow_map_pass_code = cg::data::load_glsl_program_desc("shadow-map-pass-shader", "../../data/deferred_lighting_shaders/shadow_map_pass");
	config.ssao_pass_code = cg::data::load_glsl_program_desc("ssao-pass-shader", "../../data/deferred_lighting_shaders/ssao_pass");
	config.material_lighting_pass_code = cg::data::load_glsl_program_desc("material-pass-shader", "../../data/deferred_lighting_shaders/material_pass");
	config.tone_mapping_pass_code = cg::data::load_glsl_program_desc("tone-mapping-pass-shader", "../../data/deferred_lighting_shaders/tone_mapping_pass");
	return config;
}

} // namespace


namespace deferred_lighting {

// ----- Material -----

Material::Material(float smoothness, Texture_2d_immut tex_diffuse_rgb,
	Texture_2d_immut tex_normal_map, Texture_2d_immut tex_specular_intensity) noexcept :
	smoothness(smoothness),
	tex_diffuse_rgb(std::move(tex_diffuse_rgb)),
	tex_normal_map(std::move(tex_normal_map)),
	tex_specular_intensity(std::move(tex_specular_intensity))
{
	assert(this->smoothness >= 0.0f);
	assert(this->tex_diffuse_rgb.id() != Blank::texture_id);
	assert(this->tex_normal_map.id() != Blank::texture_id);
	assert(this->tex_specular_intensity.id() != Blank::texture_id);
}

// ----- Material_library -----

Material_library::Material_library()
{
	Sampler_desc nearest_clamp_to_edge(GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE);
	Sampler_desc nearest_repeat(GL_NEAREST, GL_NEAREST, GL_REPEAT);
	Sampler_desc bilinear_clamp_to_edge(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE);
	Sampler_desc bilinear_repeat(GL_LINEAR, GL_LINEAR, GL_REPEAT);


	image_2d material_default_normal_map("../../data/common_data/material-default-normal-map.png");
	image_2d specular_intensity_0_18_image("../../data/common_data/material-specular-intensity-0.18f.png");
	image_2d specular_intensity_1_00_image("../../data/common_data/material-specular-intensity-1.00f.png");

	{ // default material
		image_2d diffuse_rgb_image("../../data/common_data/material-default-diffuse-rgb.png");

		_default_material.smoothness = 10.0f;
		_default_material.tex_diffuse_rgb = Texture_2d_immut(GL_RGB8, 1, nearest_clamp_to_edge, diffuse_rgb_image);
		_default_material.tex_normal_map = Texture_2d_immut(GL_RGB8, 1, nearest_clamp_to_edge, material_default_normal_map);
		_default_material.tex_specular_intensity = Texture_2d_immut(GL_R8, 1, nearest_clamp_to_edge, specular_intensity_1_00_image);
	}

	{ // brick wall
		image_2d diffuse_rgb_image("../../data/bricks-red-diffuse-rgb.png");
		image_2d normal_map_image("../../data/bricks-red-normal-map.png");
		image_2d specular_image("../../data/bricks-red-specular-intensity.png");

		_brick_wall_material.smoothness = 5.0f;
		_brick_wall_material.tex_diffuse_rgb = Texture_2d_immut(GL_RGB8, 1, bilinear_clamp_to_edge, diffuse_rgb_image);
		_brick_wall_material.tex_normal_map = Texture_2d_immut(GL_RGB8, 1, nearest_clamp_to_edge, normal_map_image);
		_brick_wall_material.tex_specular_intensity = Texture_2d_immut(GL_R8, 1, bilinear_clamp_to_edge, specular_image);
	}

	{ // chess board
		image_2d diffuse_rgb_image("../../data/chess-board-diffuse-rgb.png");

		_chess_board_material.smoothness = 1.0f;
		_chess_board_material.tex_diffuse_rgb = Texture_2d_immut(GL_RGB8, 1, bilinear_repeat, diffuse_rgb_image);
		_chess_board_material.tex_normal_map = Texture_2d_immut(GL_RGB8, 1, nearest_repeat, material_default_normal_map);
		_chess_board_material.tex_specular_intensity = Texture_2d_immut(GL_R8, 1, bilinear_repeat, specular_intensity_0_18_image);
	}

	{ // teapot material
		image_2d diffuse_rgb_image("../../data/teapot-diffuse-rgb.png");
		image_2d normal_map_image("../../data/teapot-normal-map.png");

		_teapot_material.smoothness = 10.0f;
		_teapot_material.tex_diffuse_rgb = Texture_2d_immut(GL_RGB8, 1, nearest_clamp_to_edge, diffuse_rgb_image);
		_teapot_material.tex_normal_map = Texture_2d_immut(GL_RGB8, 1, bilinear_clamp_to_edge, normal_map_image);
		_teapot_material.tex_specular_intensity = Texture_2d_immut(GL_R8, 1, nearest_clamp_to_edge, specular_intensity_1_00_image);
	}

	{ // wooden box
		image_2d diffuse_rgb_image("../../data/wooden-box-diffuse-rgb.png");
		image_2d normal_map_image("../../data/wooden-box-normal-map.png");
		image_2d specular_image("../../data/wooden-box-specular-intensity.png");

		_wooden_box_material.smoothness = 4.0f;
		_wooden_box_material.tex_diffuse_rgb = Texture_2d_immut(GL_RGB8, 1, bilinear_clamp_to_edge, diffuse_rgb_image);
		_wooden_box_material.tex_normal_map = Texture_2d_immut(GL_RGB8, 1, nearest_clamp_to_edge, normal_map_image);
		_wooden_box_material.tex_specular_intensity = Texture_2d_immut(GL_R8, 1, bilinear_clamp_to_edge, specular_image);
	}
}

// ----- Deferred_lighting -----

Deferred_lighting::Deferred_lighting(const cg::sys::app_context& app_ctx, cg::rnd::rhi_context_i& rhi_ctx) 
	: example(app_ctx, rhi_ctx),
	//_curr_viewpoint(float3(4, 4, -3.5), float3(0, 1, 0)),
	_curr_viewpoint(float3(2, 5, 4), float3(0, 0, 0)),
	_prev_viewpoint(_curr_viewpoint),
	_renderer(make_render_config(_app_ctx.window.viewport_size())),
	_frame(16)
{

	// scene lights
	_dir_light.position = float3(-1000, 1000, -1000);
	_dir_light.target = float3::zero;
	_dir_light.rgb = float3::unit_xyz;
	_dir_light.intensity = 2.0f;
	_dir_light.ambient_intensity = 0.35f;
	float width = 30.0f;
	float height = width / aspect_ratio(_app_ctx.window.viewport_size());
	float distance_to_light = len(_dir_light.position - _dir_light.target);
	_dir_light.projection_matrix = orthographic_matrix_opengl(width, height, -1, 1.5f * distance_to_light);

	update_viewpoint_projection();

	init_geometry();
	init_renderables();
}

void Deferred_lighting::begin_render(float interpolation_factor)
{
	_frame.reset(_vertex_spec0);

	_frame.far_plane_coords = _far_plane_coords;
	_frame.projection_matrix = _projection_matrix;
	_frame.view_matrix = view_matrix(_prev_viewpoint, _curr_viewpoint, interpolation_factor);
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
	using cg::data::load_model;

	struct Load_info {
		Load_info(const char* filename, DE_cmd* cmd) noexcept 
			: filename(filename), cmd(cmd) 
		{}

		const char* filename;
		DE_cmd* cmd;
	};

	Load_info load_info_list[3] = {
		Load_info("../../data/models/teapot.obj", &_cmd_teapot),
		Load_info("../../data/cube.obj", &_cmd_cube),
		Load_info("../../data/rect_2x2_uv_repeat.obj", &_cmd_rect_2x2_repeat)
	};

	_vs_builder.begin<Vertex_attribs::p_n_tc_ts>(megabytes(4));

	for (size_t i = 0; i < std::extent<decltype(load_info_list)>::value; ++i) {
		auto geometry_data = load_model<Vertex_attribs::p_n_tc_ts>(load_info_list[i].filename);
		*load_info_list[i].cmd = _vs_builder.push_back(geometry_data);
	}

	_vertex_spec0 = _vs_builder.end(_renderer.vertex_attrib_layout());
}

void Deferred_lighting::init_renderables()
{
	_rednerable_objects.reserve(16);

	// teapot
	_rednerable_objects.emplace_back(_cmd_teapot,
		ts_matrix(float3(0, 0.965f, 0), float3(0.03f)),
		_material_library.teapot_material());

	// cubes
	_rednerable_objects.emplace_back(_cmd_cube,
		translation_matrix(float3(-0.45f, 0.5f, -0.5)),
		_material_library.brick_wall_material());

	_rednerable_objects.emplace_back(_cmd_cube,
		translation_matrix(float3(0.55f, 0.5f, -0.5)),
		_material_library.wooden_box_material());

	_rednerable_objects.emplace_back(_cmd_cube,
		translation_matrix(float3(-0.45f, 0.5f, 0.5)),
		_material_library.wooden_box_material());

	_rednerable_objects.emplace_back(_cmd_cube,
		translation_matrix(float3(0.55f, 0.5f, 0.5)),
		_material_library.brick_wall_material());

	// chess board
	_rednerable_objects.emplace_back(_cmd_rect_2x2_repeat,
		ts_matrix(float3::zero, float3(5)),
		_material_library.chess_board_material());
}

void Deferred_lighting::on_mouse_move()
{
	float2 pos_ndc = _app_ctx.mouse.get_ndc_position(_app_ctx.window.viewport_size());
	float2 offset_ndc = pos_ndc - _prev_mouse_pos_ndc;
	_prev_mouse_pos_ndc = pos_ndc;


	if (!_app_ctx.mouse.middle_down() || offset_ndc == float2::zero) return;

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

void Deferred_lighting::on_window_resize()
{
	update_viewpoint_projection();
	_renderer.resize_viewport(_app_ctx.window.viewport_size());
}

void Deferred_lighting::render(float interpolation_factor)
{
	assert(0.f <= interpolation_factor && interpolation_factor <= 1.f);

	begin_render(interpolation_factor);
	_renderer.render(_frame);
	end_render();
}

void Deferred_lighting::update(float dt)
{	
	if (_view_roll_angles != float2::zero) {
		float dist = distance(_curr_viewpoint);
		float3 ox = cross(forward(_curr_viewpoint), _curr_viewpoint.up);
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

		_curr_viewpoint.up = normalize(cross(ox, forward(_curr_viewpoint)));
	}

	_view_roll_angles = float2::zero;
}

void Deferred_lighting::update_viewpoint_projection()
{
	float near_z = 0.1f;
	float far_z = 50;
	float vert_fov = pi_3;
	float wh_ratio = aspect_ratio(_app_ctx.window.viewport_size());
	_projection_matrix = perspective_matrix_opengl(vert_fov, wh_ratio, near_z, far_z);

	float far_y = far_z * std::tan(vert_fov * 0.5f);
	float far_x = far_y * wh_ratio;
	_far_plane_coords[0] = float3(-far_x, -far_y, -far_z); // left-bottom
	_far_plane_coords[1] = float3( far_x, -far_y, -far_z); // right-bottom
	_far_plane_coords[2] = float3( far_x,  far_y, -far_z); // right-top
	_far_plane_coords[3] = float3(-far_x,  far_y, -far_z); // left-top
}

} // namespace deferred_lighting