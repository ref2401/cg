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
	_u_arr_tex_normal_map_location(_prog.get_uniform_location("u_arr_tex_normal_map")),
	_u_arr_tex_specular_intensity_location(_prog.get_uniform_location("u_arr_tex_specular_intensity"))
{}

void Gbuffer_pass_shader_program::set_uniform_array_model_matrix(const float* ptr, size_t count) noexcept
{
	set_uniform_array<mat4>(_u_arr_model_matrix_location, ptr, count);
}

void Gbuffer_pass_shader_program::use(const mat4& projection_matrix, const mat4& view_matrix) noexcept
{
	glUseProgram(_prog.id());
	set_uniform(_u_projection_view_matrix_location, projection_matrix * view_matrix);
	set_uniform(_u_view_matrix_location, static_cast<mat3>(view_matrix));
}

void Gbuffer_pass_shader_program::foo() 
{
	GLint data0[16];
	glGetnUniformiv(_prog.id(), _u_arr_tex_normal_map_location, 24, data0);

	GLint data1[16];
	glGetnUniformiv(_prog.id(), _u_arr_tex_specular_intensity_location, 24, data1);
	auto err = glGetError();
}

} // namespace deferred_lighting