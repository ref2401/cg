#include "technique/fur_simulation/fur_simulation_opengl.h"


namespace fur_simulation {

Fur_simulation_opengl::Fur_simulation_opengl(const cg::sys::App_context& app_ctx) :
	Example(app_ctx),
	_buffer(sizeof(float) * 3)
{
	float data[3] = { 1, 2, 3 };
	float data2[5] = { 4, 5, 6, 7, 8 };

	_buffer.write(sizeof(float) * 3, data);
	_buffer.write(sizeof(float) * 5, data2);
	glBindBuffer(GL_ARRAY_BUFFER, _buffer.id());
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
