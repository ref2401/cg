#ifndef TECHNIQUE_FUR_SIMULATION_FUR_GLSL_PROGRAM_H_
#define TECHNIQUE_FUR_SIMULATION_FUR_GLSL_PROGRAM_H_

#include "cg/base/math.h"
#include "cg/rnd/opengl/opengl.h"
#include "technique/fur_simulation/fur_model.h"

using namespace cg::rnd::opengl;


namespace fur_simulation {

class Fur_pass_program final {
public:

	Fur_pass_program();

	Fur_pass_program(const Fur_pass_program&) = delete;

	Fur_pass_program(Fur_pass_program&&) = delete;


	Fur_pass_program& operator=(const Fur_pass_program&) = delete;

	Fur_pass_program& operator=(Fur_pass_program&&) = delete;


	void bind(const float4x4& pvm_matrix, const float4x4& model_matrix,
		const float3& view_position_ws, const Strand_properties& strand_props,
		const float3& light_dir_ws) const noexcept;

	void set_shell_index(GLuint index) const noexcept;

private:

	Glsl_program _program;
	GLint _g_pvm_matrix_location = Blank::uniform_location;
	GLint _g_model_matrix_location = Blank::uniform_location;
	GLint _g_view_position_ws_location = Blank::uniform_location;
	GLint _g_strand_props_location = Blank::uniform_location;
	GLint _g_shell_index_location = Blank::uniform_location;
	GLint _g_light_dir_ws_location = Blank::uniform_location;
	GLint _g_fur_mask_uv_factors_location = Blank::uniform_location;
};

class Physics_simulation_pass_program final {
public:

	Physics_simulation_pass_program();

	Physics_simulation_pass_program(const Physics_simulation_pass_program&) = delete;

	Physics_simulation_pass_program(Physics_simulation_pass_program&&) = delete;


	Physics_simulation_pass_program& operator=(const Physics_simulation_pass_program&) = delete;

	Physics_simulation_pass_program& operator=(Physics_simulation_pass_program&&) = delete;


	void bind(const float4& gravity_ms, const float3& angular_velocity_ms, 
		const float4& strand_props) noexcept;

private:

	Glsl_program _program;
	GLint _g_angular_velocity_ms_location = Blank::uniform_location;
	GLint _g_external_accel_ms_location = Blank::uniform_location;
	GLint _g_strand_props = Blank::uniform_location;
};

class Opaque_model_pass_program final {
public:

	Opaque_model_pass_program();

	Opaque_model_pass_program(const Opaque_model_pass_program&) = delete;

	Opaque_model_pass_program(Opaque_model_pass_program&&) = delete;


	Opaque_model_pass_program& operator=(const Opaque_model_pass_program&) = delete;

	Opaque_model_pass_program operator=(Opaque_model_pass_program&&) = delete;


	void bind(const float4x4& projection_view_matrix, 
		const float4x4& model_matrix, const float3& dir_to_light_ws) noexcept;

private:

	Glsl_program _program;
	GLint _g_projection_view_matrix_location = Blank::uniform_location;
	GLint _g_model_matrix_location = Blank::uniform_location;
	GLint _g_dir_to_light_ws_location = Blank::uniform_location;
};

class Strand_debug_pass_program final {
public:

	Strand_debug_pass_program();

	Strand_debug_pass_program(const Strand_debug_pass_program&) = delete;

	Strand_debug_pass_program(Strand_debug_pass_program&&) = delete;


	Strand_debug_pass_program& operator=(const Strand_debug_pass_program&) = delete;

	Strand_debug_pass_program& operator=(Strand_debug_pass_program&&) = delete;


	void bind(const float4x4& pvm_matrix) noexcept;

private:

	Glsl_program _program;
	GLint _g_pvm_matrix_location = Blank::uniform_location;
};

} // namespace fur_simulation

#endif // TECHNIQUE_FUR_SIMULATION_FUR_GLSL_PROGRAM_H_

