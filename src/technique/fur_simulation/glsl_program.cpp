#include "technique/fur_simulation/glsl_program.h"


namespace fur_simulation {

// ----- Fur_generation_program -----

Fur_generation_program::Fur_generation_program()
{
	auto glsl_source = cg::data::load_glsl_program_data("../data/fur_simulation/fur_generation");
	_program = Glsl_program("fur-generation", glsl_source);

	_g_projection_matrix_location = _program.get_uniform_location("g_projection_matrix");
	_g_view_matrix_location = _program.get_uniform_location("g_view_matrix");
	_g_model_matrix_location = _program.get_uniform_location("g_model_matrix");
	_g_shell_count_location = _program.get_uniform_location("g_shell_count");
	_g_light_dir_ws_locaiton = _program.get_uniform_location("g_light_dir_ws");
}

void Fur_generation_program::bind(const mat4& projection_matrix, const mat4& view_matrix,
	const mat4& model_matrix, GLuint shell_count, const float3& light_dir_ws) noexcept
{
	glUseProgram(_program.id());
	set_uniform(_g_projection_matrix_location, projection_matrix);
	set_uniform(_g_view_matrix_location, view_matrix);
	set_uniform(_g_model_matrix_location, model_matrix);
	set_uniform(_g_shell_count_location, shell_count);
	set_uniform(_g_light_dir_ws_locaiton, light_dir_ws);
}

// ----- Opaque_model_program -----

Opaque_model_program::Opaque_model_program()
{
	auto glsl_source = cg::data::load_glsl_program_data("../data/fur_simulation/opaque_model");
	_program = Glsl_program("opaque-geometry", glsl_source);
	_g_projection_view_matrix_location = _program.get_uniform_location("g_projection_view_matrix");
	_g_model_matrix_location = _program.get_uniform_location("g_model_matrix");
	_g_light_dir_ws_location = _program.get_uniform_location("g_light_dir_ws");
}

void Opaque_model_program::bind(const mat4& projection_view_matrix, const mat4& model_matrix,
	const float3& light_dir_ws) noexcept
{
	glUseProgram(_program.id());
	set_uniform(_g_projection_view_matrix_location, projection_view_matrix);
	set_uniform(_g_model_matrix_location, model_matrix);
	set_uniform(_g_light_dir_ws_location, light_dir_ws);
}

} // namespace fur_simulation {
