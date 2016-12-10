#ifndef TECHNIQUE_FUR_SIMULATION_FUR_SIMULATION_OPENGL_H_
#define TECHNIQUE_FUR_SIMULATION_FUR_SIMULATION_OPENGL_H_

#include "cg/rnd/opengl/opengl.h"
#include "cg/sys/app.h"

using namespace cg;
using namespace cg::rnd::opengl;


namespace fur_simulation {

class Fur_simulation_opengl final : public cg::sys::Example {
public:
	
	explicit Fur_simulation_opengl(const cg::sys::App_context& app_ctx);


	void on_window_resize() override;

	void render(float interpolation_factor) override;

	void update(float dt) override;

private:
};

} // fur_simulation

#endif // TECHNIQUE_FUR_SIMULATION_FUR_SIMULATION_OPENGL_H_
