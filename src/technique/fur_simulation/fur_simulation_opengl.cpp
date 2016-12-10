#include "technique/fur_simulation/fur_simulation_opengl.h"


namespace fur_simulation {

Fur_simulation_opengl::Fur_simulation_opengl(const cg::sys::App_context& app_ctx) :
	Example(app_ctx)
{

}

void Fur_simulation_opengl::on_window_resize()
{
}

void Fur_simulation_opengl::render(float interpolation_factor)
{
	glClearColor(0.2f, 0.3f, 0.2f, 1);
	glClear(GL_COLOR_BUFFER_BIT);
}

void Fur_simulation_opengl::update(float dt)
{
}

} // fur_simulation
