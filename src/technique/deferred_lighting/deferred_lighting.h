#ifndef TECHNIQUE_DEFERRED_LIGHTING_DEFERRED_LIGHTING_H_
#define TECHNIQUE_DEFERRED_LIGHTING_DEFERRED_LIGHTING_H_

#include <vector>
#include "cg/math/math.h"
#include "cg/opengl/opengl.h"
#include "cg/sys/app.h"
#include "technique/deferred_lighting/frame.h"
#include "technique/deferred_lighting/render.h"


namespace deferred_lighting {

struct Renderable final {

	Renderable(const cg::opengl::DE_cmd& cmd, const cg::mat4& model_matrix) noexcept;

	cg::opengl::DE_cmd cmd;
	cg::mat4 model_matrix;
};

class Deferred_lighting final : public cg::sys::IGame {
public:

	Deferred_lighting(cg::uint2 window_size);

	~Deferred_lighting() noexcept override = default;


	void on_window_resize() override {}

	void render(float blend_state) override;

	void update(float dt) override {}

private:
	// scene data
	cg::opengl::Static_vertex_spec_builder _vs_builder;
	std::unique_ptr<cg::opengl::Static_vertex_spec> _vertex_spec0;
	// scene
	std::vector<Renderable> _rednerable_objects;
	cg::mat4 _projection_matrix;
	cg::mat4 _view_matrix;
	// renderer stuff
	Renderer _renderer;
	Frame _frame;
};

} // namespace deferred_lighting

#endif // TECHNIQUE_DEFERRED_LIGHTING_DEFERRED_LIGHTING_H_