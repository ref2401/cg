#ifndef TECHNIQUE_FUR_SIMULATION_FUR_GLSL_PROGRAM_H_
#define TECHNIQUE_FUR_SIMULATION_FUR_GLSL_PROGRAM_H_

#include "cg/math/math.h"
#include "cg/rnd/opengl/opengl.h"

using namespace cg::rnd::opengl;


namespace fur_simulation {

struct Material final {
	Texture_2d_immut tex_diffuse_rgb;
	float shadow_factor_power = 1.0f;
	float threshold_power = 1.0f;
	float curl_radius = 0.0f;
	float curl_frequency = 0.0;
	float specular_factor = 0.0;
	float specular_power = 1.0;
	float tex_coord_factor = 1.0f;
	size_t shell_count = 8;
};

class Physics_simulation_pass_program final {
public:

	Physics_simulation_pass_program();

	Physics_simulation_pass_program(const Physics_simulation_pass_program&) = delete;

	Physics_simulation_pass_program(Physics_simulation_pass_program&&) = delete;


	Physics_simulation_pass_program& operator=(const Physics_simulation_pass_program&) = delete;

	Physics_simulation_pass_program& operator=(Physics_simulation_pass_program&&) = delete;


	void bind(const cg::float4& gravity_ms, const cg::float4& strand_props) noexcept;

private:

	Glsl_program _program;
	GLint _g_gravity_ms_location = Invalid::uniform_location;
	GLint _g_strand_props = Invalid::uniform_location;
};

class Strand_debug_pass_program final {
public:

	Strand_debug_pass_program();

	Strand_debug_pass_program(const Strand_debug_pass_program&) = delete;

	Strand_debug_pass_program(Strand_debug_pass_program&&) = delete;


	Strand_debug_pass_program& operator=(const Strand_debug_pass_program&) = delete;

	Strand_debug_pass_program& operator=(Strand_debug_pass_program&&) = delete;


	void bind(const cg::mat4& pvm_matrix) noexcept;

private:

	Glsl_program _program;
	GLint _g_pvm_matrix_location = Invalid::uniform_location;
};

} // namespace fur_simulation

#endif // TECHNIQUE_FUR_SIMULATION_FUR_GLSL_PROGRAM_H_

