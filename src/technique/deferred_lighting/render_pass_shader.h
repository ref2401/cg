#ifndef TECHNIQUE_DEFERRED_LIGHTING_RENDER_PASS_SHADER_H_
#define TECHNIQUE_DEFERRED_LIGHTING_RENDER_PASS_SHADER_H_

#include <array>
#include "cg/data/shader.h"
#include "cg/rnd/opengl/opengl.h"


namespace deferred_lighting {

struct Directional_light_params final {

	// Projection matrix is used in shadow mapping.
	cg::mat4 projection_matrix;

	// View matrix is used in shadow mapping.
	cg::mat4 view_matrix;

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

	explicit Gbuffer_pass_shader_program(const cg::data::Glsl_program_desc& source_code);

	Gbuffer_pass_shader_program(const Gbuffer_pass_shader_program&) = delete;

	Gbuffer_pass_shader_program(Gbuffer_pass_shader_program&&) = delete;

	~Gbuffer_pass_shader_program() noexcept = default;


	void set_uniform_array_model_matrix(const float* ptr, size_t count) noexcept;

	void set_uniform_array_smoothness(const float* ptr, size_t count) noexcept;

	void use(const cg::mat4& projection_matrix, const cg::mat4& view_matrix) noexcept;

private:
	cg::rnd::opengl::Glsl_program _prog;
	GLint _u_projection_matrix_location = cg::rnd::opengl::Blank::uniform_location;
	GLint _u_view_matrix_location = cg::rnd::opengl::Blank::uniform_location;
	GLint _u_arr_model_matrix_location = cg::rnd::opengl::Blank::uniform_location;
	GLint _u_arr_smoothness_location = cg::rnd::opengl::Blank::uniform_location;
	GLint _u_arr_tex_normal_map_location = cg::rnd::opengl::Blank::uniform_location;
};

class Lighting_pass_dir_shader_program final {
public:

	explicit Lighting_pass_dir_shader_program(const cg::data::Glsl_program_desc& dir_source_code);

	Lighting_pass_dir_shader_program(const Lighting_pass_dir_shader_program&) = delete;

	Lighting_pass_dir_shader_program(Lighting_pass_dir_shader_program&&) = delete;

	~Lighting_pass_dir_shader_program() noexcept = default;


	void set_uniform_array_far_plane_coords(const std::array<cg::float3, 4>& far_plane_coords) noexcept;

	void use(const Directional_light_params& dl_params) noexcept;

private:
	cg::rnd::opengl::Glsl_program _prog;
	GLint _u_arr_far_pane_coords_location = cg::rnd::opengl::Blank::uniform_location;
	GLint _u_dlight_direction_to_light_vs_location = cg::rnd::opengl::Blank::uniform_location;
	GLint _u_dlight_irradiance_location = cg::rnd::opengl::Blank::uniform_location;
	GLint _u_dlight_ambient_irradiance_up_location = cg::rnd::opengl::Blank::uniform_location;
	GLint _u_dlight_ambient_irradiance_down_location = cg::rnd::opengl::Blank::uniform_location;
};

class Material_lighting_pass_shader_program final {
public:

	explicit Material_lighting_pass_shader_program(const cg::data::Glsl_program_desc& source_code);

	Material_lighting_pass_shader_program(const Material_lighting_pass_shader_program&) = delete;

	Material_lighting_pass_shader_program(Material_lighting_pass_shader_program&&) = delete;

	~Material_lighting_pass_shader_program() noexcept = default;


	void set_uniform_array_model_matrix(const float* ptr, size_t count) noexcept;

	void use(const cg::mat4& projection_matrix, const cg::mat4& view_matrix,
		const Directional_light_params& dir_light) noexcept;

private:
	cg::rnd::opengl::Glsl_program _prog;
	GLint _u_projection_matrix_location = cg::rnd::opengl::Blank::uniform_location;
	GLint _u_view_matrix_location = cg::rnd::opengl::Blank::uniform_location;
	GLint _u_dir_light_projection_matrix_location = cg::rnd::opengl::Blank::uniform_location;
	GLint _u_dir_light_view_matrix_location = cg::rnd::opengl::Blank::uniform_location;
	GLint _u_arr_model_matrix_location = cg::rnd::opengl::Blank::uniform_location;
	GLint _u_arr_tex_diffuse_rgb_location = cg::rnd::opengl::Blank::uniform_location;
	GLint _u_arr_tex_specular_intensity_location = cg::rnd::opengl::Blank::uniform_location;
};

class Shadow_map_pass_shader_program final {
public:

	explicit Shadow_map_pass_shader_program(const cg::data::Glsl_program_desc& source_code);

	Shadow_map_pass_shader_program(const Shadow_map_pass_shader_program&) = delete;

	Shadow_map_pass_shader_program(Shadow_map_pass_shader_program&&) = delete;

	~Shadow_map_pass_shader_program() noexcept = default;


	void use(const Directional_light_params& dir_light) noexcept;

	void set_uniform_array_model_matrix(const float* ptr, size_t count) noexcept;

private:
	cg::rnd::opengl::Glsl_program _prog;
	GLint _u_projection_matrix_location = cg::rnd::opengl::Blank::uniform_location;
	GLint _u_view_matrix_location = cg::rnd::opengl::Blank::uniform_location;
	GLint _u_arr_model_matrix_location = cg::rnd::opengl::Blank::uniform_location;
};

class Ssao_pass_shader_program final {
public:

	explicit Ssao_pass_shader_program(const cg::data::Glsl_program_desc& source_code);

	Ssao_pass_shader_program(const Ssao_pass_shader_program&) = delete;

	Ssao_pass_shader_program(Ssao_pass_shader_program&&) = delete;

	~Ssao_pass_shader_program() noexcept = default;


	void use(const std::vector<cg::float3>& sample_rays_and_normals,
		size_t sample_ray_count, size_t random_normal_count) noexcept;

private:
	cg::rnd::opengl::Glsl_program _prog;
	GLint _u_arr_sample_ray_location = cg::rnd::opengl::Blank::uniform_location;
	GLint _u_arr_random_normal_location = cg::rnd::opengl::Blank::uniform_location;
};

class Tone_mapping_pass_shader_program final {
public:

	explicit Tone_mapping_pass_shader_program(const cg::data::Glsl_program_desc& source_code);

	Tone_mapping_pass_shader_program(const Tone_mapping_pass_shader_program&) = delete;

	Tone_mapping_pass_shader_program(Tone_mapping_pass_shader_program&&) = delete;

	~Tone_mapping_pass_shader_program() noexcept = default;


	void use() noexcept;

private:

	cg::rnd::opengl::Glsl_program _prog;
};

} // namespace deferred_lighting

#endif // TECHNIQUE_DEFERRED_LIGHTING_RENDER_PASS_SHADER_H_
