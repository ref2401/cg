#include "technique/fur_simulation/fur_glsl_program.h"

#include "cg/data/shader.h"

using namespace cg;
using cg::data::load_glsl_program_desc;


namespace fur_simulation {

// ----- Fur_pass_program -----

Fur_pass_program::Fur_pass_program()
{
	auto prog_desc = load_glsl_program_desc("../data/fur_simulation/fur_pass");
	prog_desc.name = "fur-pass";
	_program = Glsl_program(prog_desc);

	_g_pvm_matrix_location = uniform_location(_program, "g_pvm_matrix");
	_g_model_matrix_location = uniform_location(_program, "g_model_matrix");
	_g_view_position_ws_location = uniform_location(_program, "g_view_position_ws");
	_g_strand_props_location = uniform_location(_program, "g_strand_props");
	_g_light_dir_ws_location = uniform_location(_program, "g_light_dir_ws");
	_g_fur_mask_uv_factors_location = uniform_location(_program, "g_fur_mask_uv_factors");
}

void Fur_pass_program::bind(const mat4& pvm_matrix, const mat4& model_matrix,
	const float3& view_position_ws, const Strand_properties& strand_props,
	const float3& light_dir_ws) const noexcept
{
	glUseProgram(_program.id());
	set_uniform(_g_pvm_matrix_location, pvm_matrix);
	set_uniform(_g_model_matrix_location, model_matrix);
	set_uniform(_g_view_position_ws_location, float4(view_position_ws, float(strand_props.shell_count)));
	set_uniform(_g_strand_props_location, float4(
		strand_props.curl_radius,
		strand_props.curl_frequency,
		strand_props.shadow_factor_power,
		strand_props.threshold_power
	));
	set_uniform(_g_light_dir_ws_location, light_dir_ws);
	set_uniform(_g_fur_mask_uv_factors_location, float2(
		strand_props.fur_mask_uv_min_factor, 
		strand_props.fur_mask_uv_max_factor));
}

// ----- Opaque_model_pass_program -----

Opaque_model_pass_program::Opaque_model_pass_program()
{
	auto prog_desc = load_glsl_program_desc("../data/fur_simulation/opaque_model");
	prog_desc.name = "opaque-model-pass";
	_program = Glsl_program(prog_desc);

	_g_projection_view_matrix_location = uniform_location(_program, "g_projection_view_matrix");
	_g_model_matrix_location = uniform_location(_program, "g_model_matrix");
	_g_dir_to_light_ws_location = uniform_location(_program, "g_dir_to_light_ws");
}

void Opaque_model_pass_program::bind(const cg::mat4& projection_view_matrix,
	const cg::mat4& model_matrix, const cg::float3& dir_to_light_ws) noexcept
{
	glUseProgram(_program.id());

	set_uniform(_g_projection_view_matrix_location, projection_view_matrix);
	set_uniform(_g_model_matrix_location, model_matrix);
	set_uniform(_g_dir_to_light_ws_location, dir_to_light_ws);
}

// ----- Physics_simulation_pass_program -----

Physics_simulation_pass_program::Physics_simulation_pass_program()
{
	auto program_desc = load_glsl_program_desc(
		"../data/fur_simulation/physics_simulation_pass.vertex.glsl", nullptr,
		true, "tf_p_curr", "tf_velocity", "gl_NextBuffer", "tf_debug_slot");
	program_desc.name = "physics-simulation-pass";

	_program = Glsl_program(program_desc);
	_g_angular_velocity_ms_location = uniform_location(_program, "g_angular_velocity_ms");
	_g_external_accel_ms_location = uniform_location(_program, "g_external_acceleration_ms");
	_g_strand_props = uniform_location(_program, "g_strand_props");
}

void Physics_simulation_pass_program::bind(const cg::float4& gravity_ms, 
	const cg::float3& angular_velocity_ms, const cg::float4& strand_props) noexcept
{
	glUseProgram(_program.id());

	set_uniform(_g_angular_velocity_ms_location, angular_velocity_ms);
	set_uniform(_g_external_accel_ms_location, gravity_ms);
	set_uniform(_g_strand_props, strand_props);
}

// ----- Strand_debug_pass_program -----

Strand_debug_pass_program::Strand_debug_pass_program()
{
	auto program_desc = load_glsl_program_desc("../data/fur_simulation/strand_debug_pass");
	program_desc.name = "strand-debug-pass";
	_program = Glsl_program(program_desc);
	_g_pvm_matrix_location = uniform_location(_program, "g_pvm_matrix");
}

void Strand_debug_pass_program::bind(const cg::mat4& pvm_matrix) noexcept
{
	glUseProgram(_program.id());
	set_uniform(_g_pvm_matrix_location, pvm_matrix);
}

} // namespace fur_simulation {
