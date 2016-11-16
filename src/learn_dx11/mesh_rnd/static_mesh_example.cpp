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
	init_shaders();
}

void Static_mesh_example::init_geometry()
{
	auto mesh_data = cg::file::load_mesh_wavefront<Vertex_attribs::position>("../data/cube.obj", 24, 36);

	// vertex buffer
	D3D11_BUFFER_DESC vb_desc = {};
	vb_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vb_desc.ByteWidth = mesh_data.vertex_data_byte_count();
	vb_desc.Usage = D3D11_USAGE_IMMUTABLE;
	D3D11_SUBRESOURCE_DATA vb_data = {};
	vb_data.pSysMem = mesh_data.vertex_data().data();

	HRESULT hr = _device->CreateBuffer(&vb_desc, &vb_data, &_vertex_buffer.ptr);
	assert(hr == S_OK);

	// index buffer
	D3D11_BUFFER_DESC ib_desc = {};
	ib_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ib_desc.ByteWidth = mesh_data.index_data_byte_count();
	ib_desc.Usage = D3D11_USAGE_IMMUTABLE;
	D3D11_SUBRESOURCE_DATA ib_data = {};
	ib_data.pSysMem = mesh_data.index_data().data();

	hr = _device->CreateBuffer(&ib_desc, &ib_data, &_index_buffer.ptr);
	assert(hr == S_OK);
}

void Static_mesh_example::init_shaders()
{
	auto hlsl_data = cg::file::load_hlsl_shader_set_data("../data/learn_dx11/mesh_rnd/static_mesh.hlsl");
	hlsl_data.vertex_shader_entry_point = "vs_main";
	hlsl_data.pixel_shader_entry_point = "ps_main";

	_shader_set = Hlsl_shader_set(_device, hlsl_data);
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