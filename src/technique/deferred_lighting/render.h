#ifndef TECHNIQUE_DEFERRED_LIGHTING_RENDER_H_
#define TECHNIQUE_DEFERRED_LIGHTING_RENDER_H_

#include <array>
#include <limits>
#include <vector>
#include "cg/base/base.h"
#include "cg/math/math.h"
#include "cg/data/image.h"
#include "cg/data/shader.h"
#include "cg/rnd/opengl/opengl.h"
#include "technique/deferred_lighting/frame.h"
#include "technique/deferred_lighting/render_pass_shader.h"


namespace deferred_lighting {

class Gbuffer final {
public:

	Gbuffer(const cg::uint2& viewport_size,
		const cg::rnd::opengl::Vertex_attrib_layout& vertex_attrib_layout,
		const cg::data::Interleaved_mesh_data& rect_1x1_mesh_data) noexcept;

	Gbuffer(const Gbuffer& gbuffer) = delete;

	Gbuffer(Gbuffer&& gbuffer) = delete;

	~Gbuffer() noexcept = default;


	// Auxiliary depth32f buffer that can be used in an rendering pass.
	// The renderbuffer is used in shadow_map_pass for depth test.
	cg::rnd::opengl::Renderbuffer& aux_depth_renderbuffer() noexcept
	{
		return _aux_depth_renderbuffer;
	}

	// Rect 1x1 (a square) is usually used to perform a full screen pass.
	const cg::rnd::opengl::DE_base_vertex_params& aux_geometry_rect_1x1_params() const noexcept
	{
		return _aux_geometry_rect_1x1_params;
	}

	// Returns the vertex array object's id that must be bound before 
	// auxilary geometry (rect_1x1, ...other...) is renderer.
	GLuint aux_geometry_vao_id() const noexcept
	{
		return _aux_geometry_vertex_spec.vao_id();
	}
	
	// Default bilinear sampler (clamp_to_edge).
	const cg::rnd::opengl::Sampler& bilinear_sampler() const noexcept
	{
		return _bilinear_sampler;
	}

	// Default nearest sampler (clamp_to_edge).
	const cg::rnd::opengl::Sampler& nearest_sampler() const noexcept
	{
		return _nearest_sampler;
	}

	// Resizes all the render target textures.
	void resize(const cg::uint2& viewport_size) noexcept;

	// Contains depth information of the scene. Populated by the first pass (Gbuffer_pass).
	cg::rnd::opengl::Texture_2d& tex_depth_map() noexcept
	{
		return _tex_depth_map;
	}

	// Lighting_pass's render target texture.
	// xyz components contain ambient radiance.
	cg::rnd::opengl::Texture_2d& tex_lighting_ambient_term() noexcept
	{
		return _tex_lighting_ambient_term;
	}

	// Lighting_pass's render target texture.
	// xyz components contain diffuse radiance.
	cg::rnd::opengl::Texture_2d& tex_lighting_diffuse_term() noexcept
	{
		return _tex_lighting_diffuse_term;
	}

	// Lighting_pass's render target texture.
	// xyz components contain specular radiance.
	cg::rnd::opengl::Texture_2d& tex_lighting_specular_term() noexcept
	{
		return _tex_lighting_specular_term;
	}

	// Material_lighting_pass's render target texture.
	// xyz components contain fully lit material color.
	// The computations are made in HDR.
	cg::rnd::opengl::Texture_2d& tex_material_lighting_result() noexcept
	{
		return _tex_material_lighting_result;
	}

	// Gbuffer_pass's render target texture.
	// xyz components contain normal in the view space.
	// w component contains material.smoothness parameter.
	cg::rnd::opengl::Texture_2d& tex_normal_smoothness() noexcept
	{
		return _tex_normal_smoothness;
	}

	// Shadow_map_pass's render target texture.
	// xy components contain linear depth and squared depth values in the directional light's space.
	// xy components are always positive in spite of depth values are negative in the light's space.
	cg::rnd::opengl::Texture_2d& tex_shadow_map() noexcept
	{
		return _tex_shadow_map;
	}

	const cg::rnd::opengl::Vertex_attrib_layout& vertex_attrib_layout() const noexcept
	{
		return _vertex_attrib_layout;
	}

	// The current size of all the render target textures.
	const cg::uint2& viewport_size() const noexcept
	{
		return _viewport_size;
	}

private:
	const cg::rnd::opengl::Vertex_attrib_layout _vertex_attrib_layout;
	cg::rnd::opengl::Sampler _bilinear_sampler;
	cg::rnd::opengl::Sampler _nearest_sampler;
	cg::rnd::opengl::Static_vertex_spec _aux_geometry_vertex_spec;
	cg::rnd::opengl::DE_base_vertex_params _aux_geometry_rect_1x1_params;
	cg::uint2 _viewport_size;
	cg::rnd::opengl::Renderbuffer _aux_depth_renderbuffer;
	cg::rnd::opengl::Texture_2d _tex_depth_map;
	cg::rnd::opengl::Texture_2d _tex_normal_smoothness;
	cg::rnd::opengl::Texture_2d _tex_shadow_map;
	cg::rnd::opengl::Texture_2d _tex_lighting_ambient_term;
	cg::rnd::opengl::Texture_2d _tex_lighting_diffuse_term;
	cg::rnd::opengl::Texture_2d _tex_lighting_specular_term;
	cg::rnd::opengl::Texture_2d _tex_material_lighting_result;
};

class Gbuffer_pass final {
public:

	Gbuffer_pass(Gbuffer& gbuffer, const cg::data::Shader_program_source_code& source_code);

	Gbuffer_pass(const Gbuffer_pass& pass) = delete;

	Gbuffer_pass(Gbuffer_pass&& pass) = delete;

	~Gbuffer_pass() noexcept = default;


	// Performs various preparations before rendering using this pass.
	void begin(const cg::mat4& projection_matrix, const cg::mat4& view_matrix) noexcept;

	// Clears renderer states after rendring using this pass.
	void end() noexcept;

	void set_uniform_arrays(size_t rnd_offset, size_t rnd_count, 
		const std::vector<float>& uniform_array_model_matrix,
		const std::vector<float>& uniform_array_smoothness,
		const std::vector<GLuint>& uniform_array_tex_normal_map) noexcept;

private:
	const float _clear_value_normal_smoothness[4] = { 0, 0, 0, 0 };
	const float _clear_value_depth_map = 1.0f;
	cg::rnd::opengl::Framebuffer _fbo;
	Gbuffer_pass_shader_program _prog;
	Gbuffer& _gbuffer;
};

class Lighting_pass final {
public:

	Lighting_pass(Gbuffer& gbuffer, const cg::data::Shader_program_source_code& dir_source_code);

	Lighting_pass(const Lighting_pass&) = delete;

	Lighting_pass(Lighting_pass&& pass) = delete;

	~Lighting_pass() noexcept = default;


	void begin() noexcept;

	void end() noexcept;

	void perform_directional_light_pass(const cg::mat4& projection_matrix,
		const Directional_light_params& dir_light) noexcept;

private:
	const cg::float4 _clear_value_color = cg::float4::zero;
	Gbuffer& _gbuffer;
	cg::rnd::opengl::Framebuffer _fbo;
	Lighting_pass_dir_shader_program _dir_prog;
};

class Material_lighting_pass final {
public:

	Material_lighting_pass(Gbuffer& gbuffer, const cg::data::Shader_program_source_code& source_code);

	Material_lighting_pass(const Material_lighting_pass&) = delete;

	Material_lighting_pass(Material_lighting_pass&& pass) = delete;

	~Material_lighting_pass() noexcept = default;


	// Performs various preparations before rendering using this pass.
	void begin(const cg::mat4& projection_view_matrix, const Directional_light_params& dir_light) noexcept;

	// Clears renderer states after rendring using this pass.
	void end() noexcept;

	void set_uniform_arrays(size_t rnd_offset, size_t rnd_count,
		const std::vector<float>& uniform_array_model_matrix,
		const std::vector<GLuint>& uniform_array_tex_diffuse_rgb,
		const std::vector<GLuint>& uniform_array_tex_specular_intensity) noexcept;

private:
	const cg::float4 _clear_value_color = cg::rgba(0x9f9dcaff);
	Gbuffer& _gbuffer;
	cg::rnd::opengl::Framebuffer _fbo;
	Material_lighting_pass_shader_program _prog;
};

class Shadow_map_pass final {
public:

	Shadow_map_pass(Gbuffer& gbuffer, const cg::data::Shader_program_source_code& source_code);

	Shadow_map_pass(const Shadow_map_pass&) = delete;

	Shadow_map_pass(Shadow_map_pass&&) = delete;

	~Shadow_map_pass() noexcept = default;


	// Performs various preparations before rendering using this pass.
	void begin(const Directional_light_params& dir_light) noexcept;

	void end() noexcept;

	void set_uniform_arrays(size_t rnd_offset, size_t rnd_count,
		const std::vector<float>& uniform_array_model_matrix) noexcept;

private:
	const float _clear_value_shadow_map[4] = { std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), 0, 0 };
	const float _clear_value_depth = 1.0f;
	Gbuffer& _gbuffer;
	cg::rnd::opengl::Framebuffer _fbo;
	Shadow_map_pass_shader_program _prog;
};

struct Renderer_config final {

	cg::rnd::opengl::Vertex_attrib_layout vertex_attrib_layout;
	cg::uint2 viewport_size;
	cg::data::Interleaved_mesh_data rect_1x1_mesh_data;
	cg::data::Shader_program_source_code gbuffer_pass_code;
	cg::data::Shader_program_source_code shadow_map_pass_code;
	cg::data::Shader_program_source_code lighting_pass_dir_code;
	cg::data::Shader_program_source_code material_pass_code;
};

class Renderer final {
public:

	Renderer(const Renderer_config& config);

	Renderer(const Renderer&) = delete;

	Renderer(Renderer&&) = delete;

	~Renderer() noexcept = default;


	void render(const Frame& frame) noexcept;

	void resize_viewport(const cg::uint2& size) noexcept;

	const cg::rnd::opengl::Vertex_attrib_layout& vertex_attrib_layout() const noexcept
	{
		return _gbuffer.vertex_attrib_layout();
	}

private:

	void perform_gbuffer_pass(const Frame& frame) noexcept;

	void perform_shadow_map_pass(const Frame& frame) noexcept;

	void perform_lighting_pass(const Frame& frame) noexcept;

	void perform_material_lighting_pass(const Frame& frame) noexcept;

	Gbuffer _gbuffer;
	Gbuffer_pass _gbuffer_pass;
	Shadow_map_pass _shadow_map_pass;
	Lighting_pass _lighting_pass;
	Material_lighting_pass _material_lighting_pass;
};

} // namespace deferred_lighting

#endif // TECHNIQUE_DEFERRED_LIGHTING_RENDER_H_
