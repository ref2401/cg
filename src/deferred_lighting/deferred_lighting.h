#ifndef DEFERRED_LIGHTING_DEFERRED_LIGHTING_H_
#define DEFERRED_LIGHTING_DEFERRED_LIGHTING_H_

#include "cg/sys/app.h"
#include "cg/opengl/opengl.h"


namespace deferred_lighting {

class Deferred_lighting final : public cg::sys::IGame {
public:

	Deferred_lighting();

	~Deferred_lighting() noexcept override = default;


	void on_window_resize() override {}

	void render(float blend_state) override;

	void update(float dt) override {}

private:

	std::unique_ptr<cg::opengl::Shader_program> _program;
	GLuint _vao_id;
};

} // namespace deferred_lighting

#endif // DEFERRED_LIGHTING_DEFERRED_LIGHTING_H_