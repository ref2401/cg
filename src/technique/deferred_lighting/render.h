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


namespace deferred_lighting {

class Gbuffer final {
public:

	Gbuffer(cg::uint2 viewport_size) noexcept;

	Gbuffer(const Gbuffer& gbuffer) = delete;

	Gbuffer(Gbuffer&& gbuffer) = delete;

	~Gbuffer() noexcept = default;


	// Maximum supported texture unites that can be used to access texture object from vertex/fragment shaders.
	size_t max_tex_unit_count() const noexcept
	{
		return _max_tex_unit_count;
	}

	// The current size of all the render target textures.
	const cg::uint2& pixel_size() const noexcept
	{
		return _pixel_size;
	}

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

private:
	cg::opengl::Texture_2d _tex_depth_map;
	cg::opengl::Texture_2d _tex_normal_smoothness;
	cg::uint2 _pixel_size;
	const size_t _max_tex_unit_count;
};

class Gbuffer_pass final {
public:

	Gbuffer_pass(Gbuffer& gbuffer) noexcept;

	Gbuffer_pass(const Gbuffer_pass& pass) = delete;

	Gbuffer_pass(Gbuffer_pass&& pass) = delete;

	~Gbuffer_pass() noexcept = default;

private:
	cg::opengl::Framebuffer _fbo;
	Gbuffer& _gbuffer;
	
};

class Renderer final {
public:

	Renderer(cg::uint2 viewport_size, const cg::data::Shader_program_source_code& src);

	Renderer(const Renderer& rnd) = delete;

	Renderer(Renderer&& rnd) noexcept = delete;


	// The method adds additional bindings to the specified vao object hence makeing it sutable for rendering.
	// Each vao that is goint to be rendered by this renderer has to be registered.
	// Side effect: before method returns it resets vertex array binding to zero.
	void register_vao(GLuint vao_id, GLuint draw_index_binding_index) noexcept;

	void render(const Frame& frame) noexcept;

	void resize_viewport(cg::uint2 size) noexcept;

	const cg::opengl::Vertex_attrib_layout& vertex_attrib_layout() const noexcept
	{
		return _vertex_attrib_layout;
	}


private:
	cg::opengl::Vertex_attrib_layout _vertex_attrib_layout;
	Gbuffer _gbuffer;
	Gbuffer_pass _gbuffer_pass;
	// indirect rendering gears
	cg::opengl::Partitioned_buffer<cg::opengl::Persistent_buffer> _indirect_buffer;
	cg::opengl::Static_buffer _draw_index_buffer;  // simulates gl_DrawID
	std::vector<cg::mat4> _model_matrices;
	std::vector<cg::mat3> _normal_matrices;
	std::array<GLsync, 3> _sync_objects;
};

} // namespace deferred_lighting

#endif // TECHNIQUE_DEFERRED_LIGHTING_RENDER_H_
