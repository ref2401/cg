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


	// Resizes all the render target textures.
	void resize(cg::uint2 viewport_size) noexcept;

	// Contains depth information of the scene. Populated by the first pass (Gbuffer_pass).
	cg::opengl::Texture_2d& tex_depth_map() noexcept
	{
		return _tex_depth_map;
	}

	// The first pass render target texture.
	cg::opengl::Texture_2d& tex_normal_smoothness() noexcept
	{
		return _tex_normal_smoothness;
	}

	// Returns an ordered sequence of numbers which represents all the available texture units.
	// [0, GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS)
	const std::vector<GLint> texture_units() const noexcept
	{
		return _texture_units;
	}

	// The current size of all the render target textures.
	const cg::uint2& viewport_size() const noexcept
	{
		return _viewport_size;
	}

private:
	cg::opengl::Texture_2d _tex_depth_map;
	cg::opengl::Texture_2d _tex_normal_smoothness;
	cg::uint2 _viewport_size;
	std::vector<GLint> _texture_units;
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

struct Renderer_config final {
	Renderer_config(cg::uint2 viewport_size, const cg::data::Shader_program_source_code& gbuffer_pass_code) noexcept;

	cg::uint2 viewport_size;
	cg::data::Shader_program_source_code gbuffer_pass_code;
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
		return _vertex_attrib_layout;
	}


private:

	void perform_gbuffer_pass(const Frame& frame) noexcept;

	cg::opengl::Vertex_attrib_layout _vertex_attrib_layout;
	Gbuffer _gbuffer;
	Gbuffer_pass _gbuffer_pass;
	cg::opengl::Sampler _default_sampler;
};

} // namespace deferred_lighting

#endif // TECHNIQUE_DEFERRED_LIGHTING_RENDER_H_
