#ifndef TECHNIQUE_DEFERRED_LIGHTING_RENDER_PASS_SHADER_H_
#define TECHNIQUE_DEFERRED_LIGHTING_RENDER_PASS_SHADER_H_

#include "cg/data/shader.h"
#include "cg/opengl/opengl.h"


namespace deferred_lighting {

struct Directional_light_params final {
	// The direction in which the light travels. 
	// The value is normalized and in the view space.
	cg::float3 direction_vs;

	// Irradinace is computed as rgb * intensity and it's value can exceed float3::unit_xyz.
	cg::float3 irradiance;

	// Ambient irradinace is computed as rgb * ambient_intensity and it's value can exceed float3::unit_xyz.
	cg::float3 ambient_irradiance_up;

	// Additional ambient irradiance value that is used in the hemispheric ambient lighting.
	// The value is a portion of the ambient_irradinace_up value.
	cg::float3 ambient_irradiance_down;
};

// Gbuffer_pass_shader_program is used by the Gbuffer_pass to populate Gbuffer's tex_normal_smoothness.
class Gbuffer_pass_shader_program final {
public:

	Gbuffer_pass_shader_program(const cg::data::Shader_program_source_code& source_code);

	Gbuffer_pass_shader_program(const Gbuffer_pass_shader_program&) = delete;

	Gbuffer_pass_shader_program(Gbuffer_pass_shader_program&&) = delete;

	~Gbuffer_pass_shader_program() noexcept = default;


	void set_uniform_array_model_matrix(const float* ptr, size_t count) noexcept;

	void set_uniform_array_smoothness(const float* ptr, size_t count) noexcept;

	void use(const cg::mat4& projection_matrix, const cg::mat4& view_matrix) noexcept;

private:
	cg::opengl::Shader_program _prog;
	GLint _u_projection_view_matrix_location = cg::opengl::Invalid::uniform_location;
	GLint _u_view_matrix_location = cg::opengl::Invalid::uniform_location;
	GLint _u_arr_model_matrix_location = cg::opengl::Invalid::uniform_location;
	GLint _u_arr_smoothness_location = cg::opengl::Invalid::uniform_location;
	GLint _u_arr_tex_normal_map_location = cg::opengl::Invalid::uniform_location;
};

class Lighting_pass_dir_shader_program final {
public:

	Lighting_pass_dir_shader_program(const cg::data::Shader_program_source_code& dir_source_code);

	Lighting_pass_dir_shader_program(const Lighting_pass_dir_shader_program&) = delete;

	Lighting_pass_dir_shader_program(Lighting_pass_dir_shader_program&&) = delete;

	~Lighting_pass_dir_shader_program() noexcept = default;


	void use(const cg::uint2& viewport_size, const cg::mat4& projection_matrix,
		const Directional_light_params& dl_params) noexcept;

private:
	cg::opengl::Shader_program _prog;
	GLint _u_viewport_size_location = cg::opengl::Invalid::uniform_location;
	GLint _u_inv_projection_matrix_location = cg::opengl::Invalid::uniform_location;
	GLint _u_dlight_direction_to_light_vs_location = cg::opengl::Invalid::uniform_location;
	GLint _u_dlight_irradiance_location = cg::opengl::Invalid::uniform_location;
	GLint _u_dlight_ambient_irradiance_up_location = cg::opengl::Invalid::uniform_location;
	GLint _u_dlight_ambient_irradiance_down_location = cg::opengl::Invalid::uniform_location;
};

class Material_lighting_pass_shader_program final {
public:

	Material_lighting_pass_shader_program(const cg::data::Shader_program_source_code& source_code);

	Material_lighting_pass_shader_program(const Material_lighting_pass_shader_program&) = delete;

	Material_lighting_pass_shader_program(Material_lighting_pass_shader_program&&) = delete;

	~Material_lighting_pass_shader_program() noexcept = default;


	void set_uniform_array_model_matrix(const float* ptr, size_t count) noexcept;

	void use(const cg::mat4& projection_view_matrix) noexcept;

private:
	cg::opengl::Shader_program _prog;
	GLint _u_projection_view_matrix_location = cg::opengl::Invalid::uniform_location;
	GLint _u_arr_model_matrix_location = cg::opengl::Invalid::uniform_location;
	GLint _u_arr_tex_diffuse_rgb_location = cg::opengl::Invalid::uniform_location;
	GLint _u_arr_tex_specular_intensity_location = cg::opengl::Invalid::uniform_location;
};

class Shadow_map_pass_shader_program final {
public:

	Shadow_map_pass_shader_program(const cg::data::Shader_program_source_code& source_code);

	Shadow_map_pass_shader_program(const Shadow_map_pass_shader_program&) = delete;

	Shadow_map_pass_shader_program(Shadow_map_pass_shader_program&&) = delete;

	~Shadow_map_pass_shader_program() noexcept = default;


	void use(const cg::mat4& projection_matrix, const cg::mat4& view_matrix) noexcept;

	void set_uniform_array_model_matrix(const float* ptr, size_t count) noexcept;

private:
	cg::opengl::Shader_program _prog;
	GLint _u_projection_matrix_location = cg::opengl::Invalid::uniform_location;
	GLint _u_view_matrix_location = cg::opengl::Invalid::uniform_location;
	GLint _u_arr_model_matrix_location = cg::opengl::Invalid::uniform_location;
};

} // namespace deferred_lighting

#endif // TECHNIQUE_DEFERRED_LIGHTING_RENDER_PASS_SHADER_H_
