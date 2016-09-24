#ifndef DEFERRED_LIGHTING_DEFERRED_LIGHTING_H_
#define DEFERRED_LIGHTING_DEFERRED_LIGHTING_H_

#include "cg/math/math.h"
#include "cg/opengl/opengl.h"
#include "cg/sys/app.h"


namespace deferred_lighting {

class Deferred_lighting final : public cg::sys::IGame {
public:

	Deferred_lighting(cg::uint2 window_size);

	~Deferred_lighting() noexcept override = default;


	void on_window_resize() override {}

	void render(float blend_state) override;

	void update(float dt) override {}

private:
	std::unique_ptr<cg::opengl::Static_vertex_spec> _vertex_spec;
	std::unique_ptr<cg::opengl::Shader_program> _prog;
	cg::opengl::DE_cmd _de_cmd;
};

} // namespace deferred_lighting

#endif // DEFERRED_LIGHTING_DEFERRED_LIGHTING_H_