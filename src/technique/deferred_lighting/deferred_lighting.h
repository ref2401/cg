#ifndef TECHNIQUE_DEFERRED_LIGHTING_DEFERRED_LIGHTING_H_
#define TECHNIQUE_DEFERRED_LIGHTING_DEFERRED_LIGHTING_H_

#include <vector>
#include "cg/math/math.h"
#include "cg/opengl/opengl.h"
#include "cg/sys/app.h"
#include "technique/deferred_lighting/render.h"


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
	// scene
	cg::opengl::Static_vertex_spec_builder _vs_builder;
	std::unique_ptr<cg::opengl::Static_vertex_spec> _vertex_spec;
	// scene.textures
	cg::opengl::Texture_2d _tex_brick_wall_diffuse_rgb;
	cg::opengl::Texture_2d _tex_wood_fence_diffuse_rgb;
	// renderer stuff
	std::unique_ptr<Renderer> _renderer;
	std::unique_ptr<Frame> _frame;
};

} // namespace deferred_lighting

#endif // TECHNIQUE_DEFERRED_LIGHTING_DEFERRED_LIGHTING_H_