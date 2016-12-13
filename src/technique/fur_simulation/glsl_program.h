#ifndef TECHNIQUE_FUR_SIMULATION_FUR_GLSL_PROGRAM_H_
#define TECHNIQUE_FUR_SIMULATION_FUR_GLSL_PROGRAM_H_

#include "cg/rnd/opengl/opengl.h"

using namespace cg;
using namespace cg::rnd::opengl;


namespace fur_simulation {

class Fur_generation_noise_program final {
public:

	Fur_generation_noise_program();

	Fur_generation_noise_program(const Fur_generation_noise_program&) = delete;

	Fur_generation_noise_program(Fur_generation_noise_program&&) = delete;

	~Fur_generation_noise_program() noexcept = default;


	void bind(const mat4& projection_matrix, const mat4& view_matrix,
		const mat4& model_matrix, const float3& light_dir_ws,
		GLuint layer_count, GLuint draws_per_layer,
		float position_step) noexcept;

	void set_draw_indices(GLuint layer_index, GLuint draw_index) noexcept;

private:

	Glsl_program _program;
	GLint _g_projection_matrix_location = Invalid::uniform_location;
	GLint _g_view_matrix_location = Invalid::uniform_location;
	GLint _g_model_matrix_location = Invalid::uniform_location;
	GLint _g_light_dir_ws_location = Invalid::uniform_location;

	GLint _g_layer_count_location = Invalid::uniform_location;
	GLint _g_draws_per_layer_location = Invalid::uniform_location;
	GLint _g_layer_index_location = Invalid::uniform_location;
	GLint _g_draw_index_location = Invalid::uniform_location;
	GLint _g_position_step_location = Invalid::uniform_location;
};

class Opaque_model_program final {
public:

	Opaque_model_program();

	Opaque_model_program(const Opaque_model_program&) = delete;

	Opaque_model_program(Opaque_model_program&&) = delete;

	~Opaque_model_program() noexcept = default;


	void bind(const mat4& projection_view_matrix, const mat4& model_matrix,
		const float3& light_dir_ws) noexcept;

private:

	Glsl_program _program;
	GLint _g_projection_view_matrix_location = Invalid::uniform_location;
	GLint _g_model_matrix_location = Invalid::uniform_location;
	GLint _g_light_dir_ws_location = Invalid::uniform_location;
};

} // namespace fur_simulation

#endif // TECHNIQUE_FUR_SIMULATION_FUR_GLSL_PROGRAM_H_

