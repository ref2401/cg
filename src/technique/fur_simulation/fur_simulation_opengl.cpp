#include "technique/fur_simulation/fur_simulation_opengl.h"

#include "cg/data/image.h"


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

	_buffer_gpu = Buffer_gpu(sizeof(float) * 4, nullptr);
	copy(_buffer, 0, _buffer_gpu, 0, _buffer_gpu.byte_count());

	glBindBuffer(GL_ARRAY_BUFFER, _buffer_gpu.id());

	auto image = cg::data::load_image_tga("../data/bricks-red-diffuse-rgb.tga");
	Sampler_desc desc;
	_tex = Texture_2d(GL_RGB8, 1, desc, image);
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
