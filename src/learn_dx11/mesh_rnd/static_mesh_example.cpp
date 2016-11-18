#include "learn_dx11/mesh_rnd/static_mesh_example.h"

#include <sstream>
#include "cg/base/base.h"
#include "cg/data/mesh.h"
#include "cg/data/vertex.h"
#include "cg/file/file.h"
#include <DirectXMath.h>

using cg::float3;
using cg::float4;
using cg::mat4;
using cg::uint2;
using cg::data::Interleaved_mesh_data;
using cg::data::Vertex_attribs;


namespace learn_dx11 {
namespace mesh_rnd {

// ----- Static_mesh_example -----

Static_mesh_example::Static_mesh_example(Render_context& rnd_ctx) :
	Example(rnd_ctx)
{
	// function call order is important
	init_shaders();
	init_geometry(); // vertex shader bytecode is required to create vertex input layout
	init_cbuffers(rnd_ctx.viewport_size());
	setup_pipeline(); 
}

void Static_mesh_example::init_cbuffers(const uint2& viewport_size)
{
	using cg::perspective_matrix_directx;
	using cg::to_array_column_major_order;
	using cg::view_matrix;

	// projection & view matrices
	mat4 proj_m = perspective_matrix_directx(cg::pi_3, viewport_size.aspect_ratio(), 0.1f, 10.0f);
	mat4 view_m = view_matrix(float3(1, 4, 3), float3::zero);

	float matrix_data[32];
	to_array_column_major_order(proj_m, matrix_data);
	to_array_column_major_order(view_m, &matrix_data[16]);

	_scene_cbuffer = make_cbuffer(_device, 2 * sizeof(mat4));
	_device_ctx->UpdateSubresource(_scene_cbuffer.ptr, 0, nullptr, &matrix_data, 0, 0);

	// model matrix
	_model_matrix = mat4::identity;
	to_array_column_major_order(_model_matrix, matrix_data);
	_model_cbuffer = make_cbuffer(_device,  sizeof(mat4));
	_device_ctx->UpdateSubresource(_model_cbuffer.ptr, 0, nullptr, &matrix_data, 0, 0);
}

void Static_mesh_example::init_geometry()
{
	auto mesh_data = cg::file::load_mesh_wavefront<Vertex_attribs::position>("../data/cube.obj", 24, 36);
	_model_index_count = mesh_data.index_count();

	// vertex buffer
	D3D11_BUFFER_DESC vb_desc = {};
	vb_desc.ByteWidth = mesh_data.vertex_data_byte_count();
	vb_desc.Usage = D3D11_USAGE_IMMUTABLE;
	vb_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	D3D11_SUBRESOURCE_DATA vb_data = {};
	vb_data.pSysMem = mesh_data.vertex_data().data();

	HRESULT hr = _device->CreateBuffer(&vb_desc, &vb_data, &_vertex_buffer.ptr);
	assert(hr == S_OK);

	// input layout
	D3D11_INPUT_ELEMENT_DESC layout_desc = {};
	layout_desc.SemanticName = "POSITION";
	layout_desc.SemanticIndex = 0;
	layout_desc.Format = DXGI_FORMAT_R32G32B32_FLOAT;

	hr = _device->CreateInputLayout(&layout_desc, 1, 
		_shader_set.vertex_shader_bytecode()->GetBufferPointer(),
		_shader_set.vertex_shader_bytecode()->GetBufferSize(),
		&_input_layout.ptr);
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

void Static_mesh_example::setup_pipeline()
{
	size_t offset = 0;
	size_t stride = sizeof(float3);
	_device_ctx->IASetInputLayout(_input_layout.ptr);
	_device_ctx->IASetVertexBuffers(0, 1, &_vertex_buffer.ptr, &stride, &offset);
	_device_ctx->IASetIndexBuffer(_index_buffer.ptr, DXGI_FORMAT_R32_UINT, 0);
	_device_ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	_device_ctx->VSSetShader(_shader_set.vertex_shader(), nullptr, 0);
	_device_ctx->VSSetConstantBuffers(0, 1, &_scene_cbuffer.ptr);
	_device_ctx->VSSetConstantBuffers(1, 1, &_model_cbuffer.ptr);
	_device_ctx->PSSetShader(_shader_set.pixel_shader(), nullptr, 0);

	HRESULT hr = _debug->ValidateContext(_device_ctx);
	assert(hr == S_OK);
}

void Static_mesh_example::render()
{
	float4 clear_color(0.0f, 0.125f, 0.6f, 1.0f);

	clear_color_buffer(clear_color);
	_device_ctx->DrawIndexed(_model_index_count, 0, 0);
	swap_color_buffers();
}

void Static_mesh_example::update()
{
}

} // namespace mesh_rnd
} // namespace learn_dx11