#include "technique/fur_simulation/fur_glsl_program.h"

#include "cg/data/shader.h"

using namespace cg;
using cg::data::load_glsl_program_desc;


namespace fur_simulation {

// ----- Physics_simulation_pass_program -----

Physics_simulation_pass_program::Physics_simulation_pass_program()
{
	auto program_desc = load_glsl_program_desc(
		"../data/fur_simulation/physics_simulation_pass.vertex.glsl", nullptr,
		true, "tf_p_curr", "tf_velocity");

	_program = Glsl_program("physics-simulation-pass", program_desc);
	_g_gravity_ms_location = _program.get_uniform_location("g_gravity_ms");
	_g_strand_props = _program.get_uniform_location("g_strand_props");
}

void Physics_simulation_pass_program::bind(const float3& gravity_ms, const float strand_length) noexcept
{
	glUseProgram(_program.id());

	set_uniform(_g_gravity_ms_location, float4(gravity_ms, 1.0f));
	set_uniform(_g_strand_props, float4(1.0f, 1.0f, 1.0f, 0.1f));
}

// ----- Strand_debug_pass_program -----

Strand_debug_pass_program::Strand_debug_pass_program()
{
	auto program_desc = load_glsl_program_desc("../data/fur_simulation/strand_debug_pass");
	_program = Glsl_program("strand-debug-passs", program_desc);
	_g_pvm_matrix_location = _program.get_uniform_location("g_pvm_matrix");
}

void Strand_debug_pass_program::bind(const cg::mat4& pvm_matrix) noexcept
{
	glUseProgram(_program.id());
	set_uniform(_g_pvm_matrix_location, pvm_matrix);
}

} // namespace fur_simulation {
