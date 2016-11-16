#include "learn_dx11/mesh_rnd/static_mesh_example.h"

using cg::float3;
using cg::float4;


namespace learn_dx11 {
namespace mesh_rnd {

// ----- Static_mesh_example -----

Static_mesh_example::Static_mesh_example(Render_context& rnd_ctx) :
	Example(rnd_ctx)
{}

void Static_mesh_example::render()
{
	float4 clear_color(0.0f, 0.125f, 0.6f, 1.0f);

	clear_color_buffer(clear_color);
	swap_color_buffers();
}

void Static_mesh_example::update()
{
}

} // namespace mesh_rnd
} // namespace learn_dx11