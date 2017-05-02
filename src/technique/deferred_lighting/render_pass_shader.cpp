#include "technique/deferred_lighting/render_pass_shader.h"


using cg::data::Glsl_program_desc;
using cg::rnd::opengl::set_uniform;
using cg::rnd::opengl::set_uniform_array_int;
using cg::rnd::opengl::set_uniform_array_float;
using cg::rnd::opengl::set_uniform_array_float3;
using cg::rnd::opengl::set_uniform_array_mat3;
using cg::rnd::opengl::set_uniform_array_mat4;


namespace deferred_lighting {

// ----- Gbuffer_pass_shader_program -----

Gbuffer_pass_shader_program::Gbuffer_pass_shader_program(const Glsl_program_desc& source_code) :
	_prog(source_code),
	_u_projection_matrix_location(uniform_location(_prog, "u_projection_matrix")),
	_u_view_matrix_location(uniform_location(_prog, "u_view_matrix")),
	_u_arr_model_matrix_location(uniform_location(_prog, "u_arr_model_matrix")),
	_u_arr_smoothness_location(uniform_location(_prog, "u_arr_smoothness")),
	_u_arr_tex_normal_map_location(uniform_location(_prog, "u_arr_tex_normal_map"))
{}

void Gbuffer_pass_shader_program::set_uniform_array_model_matrix(const float* ptr, GLsizei count) noexcept
{
	set_uniform_array_mat4(_u_arr_model_matrix_location, ptr, count);
}

void Gbuffer_pass_shader_program::set_uniform_array_smoothness(const float* ptr, GLsizei count) noexcept
{
	set_uniform_array_float(_u_arr_smoothness_location, ptr, count);
}

void Gbuffer_pass_shader_program::use(const float4x4& projection_matrix, const float4x4& view_matrix) noexcept
{
	glUseProgram(_prog.id());
	set_uniform(_u_projection_matrix_location, projection_matrix);
	set_uniform(_u_view_matrix_location, view_matrix);
}

// ----- Lighting_pass_dir_shader_program -----

Lighting_pass_dir_shader_program::Lighting_pass_dir_shader_program(
	const Glsl_program_desc& dir_source_code) :
	_prog(dir_source_code),
	_u_arr_far_pane_coords_location(uniform_location(_prog, "u_arr_far_plane_coords")),
	_u_dlight_direction_to_light_vs_location(uniform_location(_prog, "u_dlight.direction_to_light_vs")),
	_u_dlight_irradiance_location(uniform_location(_prog, "u_dlight.irradiance")),
	_u_dlight_ambient_irradiance_up_location(uniform_location(_prog, "u_dlight.ambient_irradiance_up")),
	_u_dlight_ambient_irradiance_down_location(uniform_location(_prog, "u_dlight.ambient_irradiance_down"))
{}

void Lighting_pass_dir_shader_program::set_uniform_array_far_plane_coords(
	const std::array<float3, 4>& far_plane_coords) noexcept
{
	float arr[12] = {
		far_plane_coords[0].x, far_plane_coords[0].y, far_plane_coords[0].z,
		far_plane_coords[1].x, far_plane_coords[1].y, far_plane_coords[1].z,
		far_plane_coords[2].x, far_plane_coords[2].y, far_plane_coords[2].z,
		far_plane_coords[3].x, far_plane_coords[3].y, far_plane_coords[3].z,
	};

	set_uniform_array_float3(_u_arr_far_pane_coords_location, arr, GLsizei(far_plane_coords.size()));
}

void Lighting_pass_dir_shader_program::use(const Directional_light_params& dl_params) noexcept
{
	glUseProgram(_prog.id());
	set_uniform(_u_dlight_direction_to_light_vs_location, -dl_params.direction_vs);
	set_uniform(_u_dlight_irradiance_location, dl_params.irradiance);
	set_uniform(_u_dlight_ambient_irradiance_up_location, dl_params.ambient_irradiance_up);
	set_uniform(_u_dlight_ambient_irradiance_down_location, dl_params.ambient_irradiance_down);
}

// ----- Material_lighting_pass -----

Material_lighting_pass_shader_program::Material_lighting_pass_shader_program(const Glsl_program_desc& source_code) :
	_prog(source_code),
	_u_projection_matrix_location(uniform_location(_prog, "u_projection_matrix")),
	_u_view_matrix_location(uniform_location(_prog, "u_view_matrix")),
	_u_dir_light_projection_matrix_location(uniform_location(_prog, "u_dir_light_projection_matrix")),
	_u_dir_light_view_matrix_location(uniform_location(_prog, "u_dir_light_view_matrix")),
	_u_arr_model_matrix_location(uniform_location(_prog, "u_arr_model_matrix")),
	_u_arr_tex_diffuse_rgb_location(uniform_location(_prog, "u_arr_tex_diffuse_rgb")),
	_u_arr_tex_specular_intensity_location(uniform_location(_prog, "u_arr_tex_specular_intensity"))
{}

void Material_lighting_pass_shader_program::set_uniform_array_model_matrix(const float* ptr, size_t count) noexcept
{
	set_uniform_array_mat4(_u_arr_model_matrix_location, ptr, GLsizei(count));
}

void Material_lighting_pass_shader_program::use(const float4x4& projection_matrix, const float4x4& view_matrix,
	const Directional_light_params& dir_light) noexcept
{
	glUseProgram(_prog.id());
	set_uniform(_u_projection_matrix_location, projection_matrix);
	set_uniform(_u_view_matrix_location, view_matrix);
	set_uniform(_u_dir_light_projection_matrix_location, dir_light.projection_matrix);
	set_uniform(_u_dir_light_view_matrix_location, dir_light.view_matrix);
}

// ----- Shadow_map_pass_shader_program -----

Shadow_map_pass_shader_program::Shadow_map_pass_shader_program(const Glsl_program_desc& source_code) :
	_prog(source_code),
	_u_projection_matrix_location(uniform_location(_prog, "u_projection_matrix")),
	_u_view_matrix_location(uniform_location(_prog, "u_view_matrix")),
	_u_arr_model_matrix_location(uniform_location(_prog, "u_arr_model_matrix"))
{}

void Shadow_map_pass_shader_program::set_uniform_array_model_matrix(const float* ptr, size_t count) noexcept
{
	set_uniform_array_mat4(_u_arr_model_matrix_location, ptr, GLsizei(count));
}

void Shadow_map_pass_shader_program::use(const Directional_light_params& dir_light) noexcept
{
	glUseProgram(_prog.id());
	set_uniform(_u_projection_matrix_location, dir_light.projection_matrix);
	set_uniform(_u_view_matrix_location, dir_light.view_matrix);
}

// ----- Ssao_pass_shader_program -----

Ssao_pass_shader_program::Ssao_pass_shader_program(const Glsl_program_desc& source_code) :
	_prog(source_code),
	_u_arr_sample_ray_location(uniform_location(_prog, "u_arr_sample_ray")),
	_u_arr_random_normal_location(uniform_location(_prog, "u_arr_random_normal"))//,
{}

void Ssao_pass_shader_program::use(const std::vector<float3>& sample_rays_and_normals, 
	size_t sample_ray_count, size_t random_normal_count) noexcept
{
	glUseProgram(_prog.id());

	set_uniform_array_float3(_u_arr_sample_ray_location,
		reinterpret_cast<const float*>(sample_rays_and_normals.data()), GLsizei(sample_ray_count));

	set_uniform_array_float3(_u_arr_random_normal_location,
		reinterpret_cast<const float*>(sample_rays_and_normals.data() + sample_ray_count), GLsizei(random_normal_count));
}

// ----- Tone_mapping_pass_shader_program -----

Tone_mapping_pass_shader_program::Tone_mapping_pass_shader_program(const cg::data::Glsl_program_desc& source_code) :
	_prog(source_code)
{}

void Tone_mapping_pass_shader_program::use() noexcept
{
	glUseProgram(_prog.id());
}

} // namespace deferred_lighting
