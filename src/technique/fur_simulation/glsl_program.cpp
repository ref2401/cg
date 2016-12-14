#include "technique/fur_simulation/glsl_program.h"


namespace fur_simulation {

// ----- Fur_generation_noise_program -----

Fur_generation_noise_program::Fur_generation_noise_program()
{
	auto glsl_source = cg::data::load_glsl_program_data("../data/fur_simulation/fur_generation_noise");
	_program = Glsl_program("fur-generation", glsl_source);
	_g_projection_matrix_location = _program.get_uniform_location("g_projection_matrix");
	_g_view_matrix_location = _program.get_uniform_location("g_view_matrix");
	_g_model_matrix_location = _program.get_uniform_location("g_model_matrix");
	_g_light_dir_ws_location = _program.get_uniform_location("g_light_dir_ws");

	_g_layer_count_location = _program.get_uniform_location("g_layer_count");
	_g_draws_per_layer_location = _program.get_uniform_location("g_draws_per_layer");
	_g_layer_index_location = _program.get_uniform_location("g_layer_index");
	_g_draw_index_location = _program.get_uniform_location("g_draw_index");
	_g_position_step_location = _program.get_uniform_location("g_position_step");
	//_g_view_position_ws_location = _program.get_uniform_location("g_view_position_ws");
}

void Fur_generation_noise_program::bind(const mat4& projection_matrix, const mat4& view_matrix,
	const mat4& model_matrix, const float3& light_dir_ws, GLuint layer_count, GLuint draws_per_layer,
	float position_step, const float3 & view_position_ws) noexcept
{
	glUseProgram(_program.id());
	set_uniform(_g_projection_matrix_location, projection_matrix);
	set_uniform(_g_view_matrix_location, view_matrix);
	set_uniform(_g_model_matrix_location, model_matrix);
	set_uniform(_g_light_dir_ws_location, light_dir_ws);

	set_uniform(_g_layer_count_location, layer_count);
	set_uniform(_g_draws_per_layer_location, draws_per_layer);
	set_uniform(_g_position_step_location, position_step);
	//set_uniform(_g_view_position_ws_location, view_position_ws);
}

void Fur_generation_noise_program::set_draw_indices(GLuint layer_index, GLuint draw_index) noexcept
{
	set_uniform(_g_layer_index_location, layer_index);
	set_uniform(_g_draw_index_location, draw_index);
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
