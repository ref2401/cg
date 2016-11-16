#include "learn_dx11/mesh_rnd/static_mesh_example.h"

#include "cg/base/base.h"
#include "cg/data/mesh.h"
#include "cg/data/vertex.h"
#include "cg/file/file.h"

using cg::float3;
using cg::float4;
using cg::data::Interleaved_mesh_data;
using cg::data::Vertex_attribs;


namespace learn_dx11 {
namespace mesh_rnd {

// ----- Static_mesh_example -----

Static_mesh_example::Static_mesh_example(Render_context& rnd_ctx) :
	Example(rnd_ctx)
{
	init_geometry();
}

void Static_mesh_example::init_geometry()
{
	using cg::file::load_mesh_wavefront;

	auto mesh_data = load_mesh_wavefront<Vertex_attribs::position>("../data/cube.obj", 24, 36);

	/*D3D11_BUFFER_DESC vb_desc = {};
	vb_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vb_desc.ByteWidth = ;
	vb_desc.Usage = D3D11_USAGE_IMMUTABLE;

	D3D11_BUFFER_DESC ib_desc = {};
	ib_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ib_desc.ByteWidth = kilobytes(32);
	ib_desc.Usage = D3D11_USAGE_IMMUTABLE;*/


	

}

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