#ifndef TECHNIQUE_DEFERRED_LIGHTING_RENDER_H_
#define TECHNIQUE_DEFERRED_LIGHTING_RENDER_H_

#include <vector>
#include "cg/base/base.h"
#include "cg/math/math.h"
#include "cg/opengl/opengl.h"




namespace deferred_lighting {

struct Renderable {

	Renderable(const cg::opengl::DE_cmd& cmd, const cg::mat4& model_matrix) noexcept
	:	cmd(cmd), model_matrix(model_matrix)
	{}

	cg::opengl::DE_cmd cmd;
	cg::mat4 model_matrix;
};

// ...
// Implementation notes: Frame temporary contains an vao id and assumes 
// that all the DE_cmd objects added to it refere to the vao id.
class Frame final {
public:

	Frame(GLuint vao_id);

	Frame(const Frame& frame) = delete;

	Frame(Frame&& frame) noexcept;


	Frame& operator=(Frame&& frame) noexcept;


	void clear() noexcept;

	const cg::mat4& projection_matrix() const noexcept
	{
		return _projection_matrix;
	}

	void set_projection_matrix(const cg::mat4& mat) noexcept
	{
		_projection_matrix = mat;
	}

	void push_back_renderable(const cg::opengl::DE_cmd& cmd, const cg::mat4& model_matrix);

	const std::vector<Renderable>& renderable_objects() const noexcept
	{
		return _renderable_objects;
	}

	GLuint vao_id() const noexcept
	{
		return _vao_id;
	}

	const cg::mat4& view_matrix() const noexcept
	{
		return _view_matrix;
	}

	void set_view_matrix(const cg::mat4& mat) noexcept
	{
		_view_matrix = mat;
	}

private:
	GLuint _vao_id; // temporary here.
	std::vector<Renderable> _renderable_objects;
	cg::mat4 _projection_matrix = cg::mat4::identity;
	cg::mat4 _view_matrix = cg::mat4::identity;
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
	cg::opengl::Persistent_buffer _indirect_buffer;
	cg::opengl::Persistent_buffer _draw_index_buffer; // simulates gl_DrawID
	std::vector<cg::mat4> _model_matrices;
	GLsync _frame_sync_obj = nullptr;
	// temporary here
	cg::opengl::Shader_program _prog;
	GLint _u_pv_matrix = cg::opengl::Invalid::uniform_location;
	GLint _u_model_matrix_array = cg::opengl::Invalid::uniform_location;
};

} // namespace deferred_lighting

#endif // TECHNIQUE_DEFERRED_LIGHTING_RENDER_H_
