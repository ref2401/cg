#ifndef TECHNIQUE_DEFERRED_LIGHTING_RENDER_H_
#define TECHNIQUE_DEFERRED_LIGHTING_RENDER_H_

#include <array>
#include <vector>
#include "cg/base/base.h"
#include "cg/math/math.h"
#include "cg/data/image.h"
#include "cg/data/shader.h"
#include "cg/opengl/opengl.h"
#include "technique/deferred_lighting/frame.h"
#include "technique/deferred_lighting/render_pass_shader.h"


namespace deferred_lighting {

class Gbuffer final {
public:

	Gbuffer(cg::uint2 viewport_size) noexcept;

	Gbuffer(const Gbuffer& gbuffer) = delete;

	Gbuffer(Gbuffer&& gbuffer) = delete;

	~Gbuffer() noexcept = default;

	
	// Default bilinear sampler (clamp_to_edge).
	const cg::opengl::Sampler& bilinear_sampler() const noexcept
	{
		return _bilinear_sampler;
	}

	// Default nearest sampler (clamp_to_edge).
	const cg::opengl::Sampler& nearest_sampler() const noexcept
	{
		return _nearest_sampler;
	}

	// Resizes all the render target textures.
	void resize(cg::uint2 viewport_size) noexcept;

	// Contains depth information of the scene. Populated by the first pass (Gbuffer_pass).
	cg::opengl::Texture_2d& tex_depth_map() noexcept
	{
		return _tex_depth_map;
	}

	// Lighting_pass's render target texture.
	// xyz components contain ambient lighting intensity.
	cg::opengl::Texture_2d& tex_lighting_ambient_term() noexcept
	{
		return _tex_lighting_ambient_term;
	}

	// Gbuffer_pass's render target texture.
	// xyz components contain normal in the view space.
	// w component contains material.smoothness parameter.
	cg::opengl::Texture_2d& tex_normal_smoothness() noexcept
	{
		return _tex_normal_smoothness;
	}

	const cg::opengl::Vertex_attrib_layout& vertex_attrib_layout() const noexcept
	{
		return _vertex_attrib_layout;
	}

	// The current size of all the render target textures.
	const cg::uint2& viewport_size() const noexcept
	{
		return _viewport_size;
	}

private:
	const cg::opengl::Vertex_attrib_layout _vertex_attrib_layout;
	cg::opengl::Sampler _bilinear_sampler;
	cg::opengl::Sampler _nearest_sampler;
	cg::opengl::Texture_2d _tex_depth_map;
	cg::opengl::Texture_2d _tex_normal_smoothness;
	cg::opengl::Texture_2d _tex_lighting_ambient_term;
	cg::uint2 _viewport_size;
};

class Gbuffer_pass final {
public:

	Gbuffer_pass(Gbuffer& gbuffer, const cg::data::Shader_program_source_code& source_code);

	Gbuffer_pass(const Gbuffer_pass& pass) = delete;

	Gbuffer_pass(Gbuffer_pass&& pass) = delete;

	~Gbuffer_pass() noexcept = default;


	// Perform various preparations before rendering using this pass.
	void begin(const cg::mat4& projection_matrix, const cg::mat4& view_matrix) noexcept;

	// Clear renderer states after rendring using this pass.
	void end() noexcept;

	void set_uniform_arrays(size_t rnd_offset, size_t rnd_count, 
		const std::vector<float>& uniform_array_model_matrix,
		const std::vector<float>& uniform_array_smoothness,
		const std::vector<GLuint>& uniform_array_tex_normal_map) noexcept;

private:
	
	float _clear_value_normal_smoothness[4] = { 0, 0, 0, 0 };
	float _clear_value_depth_map = 1.f;
	cg::opengl::Framebuffer _fbo;
	Gbuffer_pass_shader_program _prog;
	Gbuffer& _gbuffer;
};

class Lighting_pass final {
public:

	Lighting_pass(Gbuffer& gbuffer, const cg::data::Shader_program_source_code& dir_source_code);

	Lighting_pass(const Lighting_pass&) = delete;

	Lighting_pass(Lighting_pass&& pass) = delete;

	~Lighting_pass() noexcept = default;

private:
	Gbuffer& _gbuffer;
	Lighting_pass_dir_shader_program _dir_prog;
};

struct Renderer_config final {

	cg::uint2 viewport_size;
	cg::opengl::Static_vertex_spec shapes_vertex_space;
	cg::opengl::DE_cmd rect_1x1_cmd;
	cg::data::Shader_program_source_code gbuffer_pass_code;
	cg::data::Shader_program_source_code lighting_pass_dir_code;
};

class Renderer final {
public:

	Renderer(const Renderer_config& config);

	Renderer(const Renderer& rnd) = delete;

	Renderer(Renderer&& rnd) noexcept = delete;

	~Renderer() noexcept = default;


	void render(const Frame& frame) noexcept;

	void resize_viewport(cg::uint2 size) noexcept;

	const cg::opengl::Vertex_attrib_layout& vertex_attrib_layout() const noexcept
	{
		return _gbuffer.vertex_attrib_layout();
	}

private:

	void perform_gbuffer_pass(const Frame& frame) noexcept;

	void perform_lighting_pass(const Frame& frame) noexcept;

	Gbuffer _gbuffer;
	Gbuffer_pass _gbuffer_pass;
	Lighting_pass _lighting_pass;
};

} // namespace deferred_lighting

#endif // TECHNIQUE_DEFERRED_LIGHTING_RENDER_H_
