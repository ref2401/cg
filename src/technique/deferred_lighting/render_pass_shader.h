#ifndef TECHNIQUE_DEFERRED_LIGHTING_RENDER_PASS_SHADER_H_
#define TECHNIQUE_DEFERRED_LIGHTING_RENDER_PASS_SHADER_H_

#include "cg/data/shader.h"
#include "cg/opengl/opengl.h"


namespace deferred_lighting {

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


	void use(const cg::float3& ambient_up_irradiance, const cg::float3& ambient_down_irradiance) noexcept;

private:
	cg::opengl::Shader_program _prog;
	GLint _u_light_ambient_up_irradiance_location = cg::opengl::Invalid::uniform_location;
	GLint _u_light_ambient_down_irradiance_location = cg::opengl::Invalid::uniform_location;
};

} // namespace deferred_lighting

#endif // TECHNIQUE_DEFERRED_LIGHTING_RENDER_PASS_SHADER_H_
