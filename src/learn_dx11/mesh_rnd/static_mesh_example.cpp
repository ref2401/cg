#include "learn_dx11/mesh_rnd/static_mesh_example.h"



namespace learn_dx11 {
namespace mesh_rnd {

// ----- Static_mesh_example -----

Static_mesh_example::Static_mesh_example(Render_context& rnd_ctx) :
	Example(rnd_ctx)
{}

void Static_mesh_example::render()
{
	float clear_color[4] = { 0.0f, 0.125f, 0.6f, 1.0f };

	_rnd_ctx.device_ctx()->ClearRenderTargetView(_rnd_ctx.rtv_back_buffer(), clear_color);
	_rnd_ctx.swap_chain()->Present(0, 0);
}

void Static_mesh_example::update()
{
}

} // namespace mesh_rnd
} // namespace learn_dx11