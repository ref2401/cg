#include "technique/deferred_lighting/render_pass_shader.h"

using cg::mat3;
using cg::mat4;
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
	_u_dir_light_dir_to_light_vs_location(_prog.get_uniform_location("u_dir_light.dir_to_light_vs")),
	_u_dir_light_irradiance_location(_prog.get_uniform_location("u_dir_light.irradiance")),
	_u_dir_light_ambient_irradiance_up_location(_prog.get_uniform_location("u_dir_light.ambient_irradiance_up")),
	_u_dir_light_ambient_irradiance_down_location(_prog.get_uniform_location("u_dir_light.ambient_irradiance_down"))
{}

void Lighting_pass_dir_shader_program::use(const Directional_light_params& dl_params) noexcept
{
	glUseProgram(_prog.id());
	set_uniform(_u_dir_light_dir_to_light_vs_location, -dl_params.direction_vs);
	set_uniform(_u_dir_light_irradiance_location, dl_params.irradiance);
	set_uniform(_u_dir_light_ambient_irradiance_up_location, dl_params.ambient_irradiance_up);
	set_uniform(_u_dir_light_ambient_irradiance_down_location, dl_params.ambient_irradiance_down);
}

} // namespace deferred_lighting