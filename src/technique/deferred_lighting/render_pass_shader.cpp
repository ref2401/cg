#include "technique/deferred_lighting/render_pass_shader.h"

using cg::mat3;
using cg::mat4;
using cg::uint2;
using cg::data::Shader_program_source_code;
using cg::opengl::set_uniform;
using cg::opengl::set_uniform_array;


namespace deferred_lighting {

// ----- Gbuffer_pass_shader_program -----

Gbuffer_pass_shader_program::Gbuffer_pass_shader_program(const Shader_program_source_code& source_code) :
	_prog("gbuffer-pass-shader", source_code),
	_u_projection_view_matrix_location(_prog.get_uniform_location("u_projection_view_matrix")),
	_u_view_matrix_location(_prog.get_uniform_location("u_view_matrix")),
	_u_arr_model_matrix_location(_prog.get_uniform_location("u_arr_model_matrix")),
	_u_arr_smoothness_location(_prog.get_uniform_location("u_arr_smoothness")),
	_u_arr_tex_normal_map_location(_prog.get_uniform_location("u_arr_tex_normal_map"))
{}

void Gbuffer_pass_shader_program::set_uniform_array_model_matrix(const float* ptr, size_t count) noexcept
{
	set_uniform_array<mat4>(_u_arr_model_matrix_location, ptr, count);
}

void Gbuffer_pass_shader_program::set_uniform_array_smoothness(const float* ptr, size_t count) noexcept
{
	set_uniform_array<float>(_u_arr_smoothness_location, ptr, count);
}

void Gbuffer_pass_shader_program::use(const mat4& projection_matrix, const mat4& view_matrix) noexcept
{
	glUseProgram(_prog.id());
	set_uniform(_u_projection_view_matrix_location, projection_matrix * view_matrix);
	set_uniform(_u_view_matrix_location, static_cast<mat3>(view_matrix));
}

// ----- Lighting_pass_dir_shader_program -----

Lighting_pass_dir_shader_program::Lighting_pass_dir_shader_program(
	const Shader_program_source_code& dir_source_code) :
	_prog("lighting-pass-dir-shader", dir_source_code),
	_u_viewport_size_location(_prog.get_uniform_location("u_viewport_size")),
	_u_inv_projection_matrix_location(_prog.get_uniform_location("u_inv_projection_matrix")),
	_u_dlight_direction_to_light_vs_location(_prog.get_uniform_location("u_dlight.direction_to_light_vs")),
	_u_dlight_irradiance_location(_prog.get_uniform_location("u_dlight.irradiance")),
	_u_dlight_ambient_irradiance_up_location(_prog.get_uniform_location("u_dlight.ambient_irradiance_up")),
	_u_dlight_ambient_irradiance_down_location(_prog.get_uniform_location("u_dlight.ambient_irradiance_down"))
{}

void Lighting_pass_dir_shader_program::use(const uint2& viewport_size, 
	const mat4& projection_matrix, const Directional_light_params& dl_params) noexcept
{
	using cg::inverse;

	glUseProgram(_prog.id());
	set_uniform(_u_viewport_size_location, viewport_size);
	set_uniform(_u_inv_projection_matrix_location, inverse(projection_matrix));
	set_uniform(_u_dlight_direction_to_light_vs_location, -dl_params.direction_vs);
	set_uniform(_u_dlight_irradiance_location, dl_params.irradiance);
	set_uniform(_u_dlight_ambient_irradiance_up_location, dl_params.ambient_irradiance_up);
	set_uniform(_u_dlight_ambient_irradiance_down_location, dl_params.ambient_irradiance_down);
}

// ----- Material_lighting_pass -----

Material_lighting_pass_shader_program::Material_lighting_pass_shader_program(const Shader_program_source_code& source_code) :
	_prog("material-pass_shader", source_code),
	_u_projection_view_matrix_location(_prog.get_uniform_location("u_projection_view_matrix")),
	_u_arr_model_matrix_location(_prog.get_uniform_location("u_arr_model_matrix")),
	_u_arr_tex_diffuse_rgb_location(_prog.get_uniform_location("u_arr_tex_diffuse_rgb")),
	_u_arr_tex_specular_intensity_location(_prog.get_uniform_location("u_arr_tex_specular_intensity"))
{}

void Material_lighting_pass_shader_program::set_uniform_array_model_matrix(const float* ptr, size_t count) noexcept
{
	set_uniform_array<mat4>(_u_arr_model_matrix_location, ptr, count);
}

void Material_lighting_pass_shader_program::use(const mat4& projection_view_matrix) noexcept
{
	glUseProgram(_prog.id());
	set_uniform(_u_projection_view_matrix_location, projection_view_matrix);
}

// ----- Shadow_map_pass_shader_program -----

Shadow_map_pass_shader_program::Shadow_map_pass_shader_program(const Shader_program_source_code& source_code) :
	_prog("shadow-map-pass-shader", source_code),
	_u_projection_matrix_location(_prog.get_uniform_location("u_projection_matrix")),
	_u_view_matrix_location(_prog.get_uniform_location("u_view_matrix")),
	_u_arr_model_matrix_location(_prog.get_uniform_location("u_arr_model_matrix"))
{}

void Shadow_map_pass_shader_program::set_uniform_array_model_matrix(const float* ptr, size_t count) noexcept
{
	set_uniform_array<mat4>(_u_arr_model_matrix_location, ptr, count);
}

void Shadow_map_pass_shader_program::use(const mat4& projection_matrix, const mat4& view_matrix) noexcept
{
	glUseProgram(_prog.id());
	set_uniform(_u_projection_matrix_location, projection_matrix);
	set_uniform(_u_view_matrix_location, view_matrix);
}

} // namespace deferred_lighting
