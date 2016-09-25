#ifndef TECHNIQUE_DEFERRED_LIGHTING_DEFERRED_LIGHTING_H_
#define TECHNIQUE_DEFERRED_LIGHTING_DEFERRED_LIGHTING_H_

#include <vector>
#include "cg/math/math.h"
#include "cg/opengl/opengl.h"
#include "cg/sys/app.h"


namespace deferred_lighting {

// ...
// Deferred_lighting renderer (embedded) uses persistent mapped buffer technique
// for multi-indirect rendering and gl_DrawID simulation. Indirect buffer and draw index buffer use
// tripple buffering technique to metigate synchronization with OpenGL command queue.
class Deferred_lighting final : public cg::sys::IGame {
public:

	Deferred_lighting(cg::uint2 window_size);

	~Deferred_lighting() noexcept override = default;


	void on_window_resize() override {}

	void render(float blend_state) override;

	void update(float dt) override {}

private:
	void load_data();

	// scene
	cg::opengl::Static_vertex_spec_builder _vs_builder;
	std::unique_ptr<cg::opengl::Static_vertex_spec> _vertex_spec;
	std::vector<cg::opengl::DE_cmd> _renderable_objects;
	// rendering
	cg::opengl::Vertex_attrib_layout _vertex_attrib_layout;
	std::unique_ptr<cg::opengl::Persistent_buffer> _indirect_buffer;
	std::unique_ptr<cg::opengl::Persistent_buffer> _draw_index_buffer; // simulates gl_DrawID
	GLsync _frame_sync_obj;

	std::unique_ptr<cg::opengl::Shader_program> _prog;
};

} // namespace deferred_lighting

#endif // TECHNIQUE_DEFERRED_LIGHTING_DEFERRED_LIGHTING_H_