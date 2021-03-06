#include "learn_dx11/mesh_rnd/static_mesh_example.h"

#include <sstream>
#include "cg/base/base.h"
#include "cg/data/image.h"
#include "cg/data/model.h"
#include <DirectXMath.h>

using cg::data::image_2d;
using cg::data::vertex_attribs;


namespace learn_dx11 {
namespace mesh_rnd {

// ----- Static_mesh_example -----

Static_mesh_example::Static_mesh_example(Render_context& rnd_ctx) 
	: Example(rnd_ctx)
{
	init_shaders();
	init_geometry(); // vertex shader bytecode is required to create vertex input layout
	init_cbuffers();
	init_material();
	init_render_states();
	update_projection_view_matrices(aspect_ratio(_rnd_ctx.viewport_size()));
	setup_pipeline_state();
}

void Static_mesh_example::init_cbuffers()
{
	_scene_cbuffer = make_cbuffer(_device, 2 * sizeof(float4x4));
	_model_cbuffer = make_cbuffer(_device, sizeof(float4x4));
	
	// model matrix
	_model_matrix = scale_matrix<float4x4>(float3(2));
	float matrix_data[16];
	to_array_column_major_order(_model_matrix, matrix_data);
	_device_ctx->UpdateSubresource(_model_cbuffer.ptr, 0, nullptr, &matrix_data, 0, 0);
}

void Static_mesh_example::init_geometry()
{
	auto model_geometry = cg::data::load_model<vertex_attribs::p_n_tc>("../../data/cube.obj");
	_model_index_count = UINT(model_geometry.index_count());

	// vertex buffer
	D3D11_BUFFER_DESC vb_desc = {};
	vb_desc.ByteWidth = UINT(model_geometry.vertex_data_byte_count());
	vb_desc.Usage = D3D11_USAGE_IMMUTABLE;
	vb_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	D3D11_SUBRESOURCE_DATA vb_data = {};
	vb_data.pSysMem = model_geometry.vertex_data().data();

	HRESULT hr = _device->CreateBuffer(&vb_desc, &vb_data, &_vertex_buffer.ptr);
	assert(hr == S_OK);

	// input layout
	D3D11_INPUT_ELEMENT_DESC layout_desc[3] = {
		{ "V_POSITION" , 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "V_NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(float3), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "V_TEX_COORD" , 0, DXGI_FORMAT_R32G32_FLOAT, 0, 2 * sizeof(float3), D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	hr = _device->CreateInputLayout(layout_desc, 3, 
		_shader_set.vertex_shader_bytecode()->GetBufferPointer(),
		_shader_set.vertex_shader_bytecode()->GetBufferSize(),
		&_input_layout.ptr);
	assert(hr == S_OK);

	// index buffer
	D3D11_BUFFER_DESC ib_desc = {};
	ib_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ib_desc.ByteWidth = UINT(model_geometry.index_data_byte_count());
	ib_desc.Usage = D3D11_USAGE_IMMUTABLE;
	D3D11_SUBRESOURCE_DATA ib_data = {};
	ib_data.pSysMem = model_geometry.index_data().data();

	hr = _device->CreateBuffer(&ib_desc, &ib_data, &_index_buffer.ptr);
	assert(hr == S_OK);
}

void Static_mesh_example::init_material()
{
	image_2d image("../../data/bricks-red-diffuse-rgb.png", 4);

	// texture
	D3D11_TEXTURE2D_DESC tex_desc = {};
	tex_desc.Width = image.size.x;
	tex_desc.Height = image.size.y;
	tex_desc.MipLevels = 1;
	tex_desc.ArraySize = 1;
	tex_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	tex_desc.SampleDesc.Count = 1;
	tex_desc.SampleDesc.Quality = 0;
	tex_desc.Usage = D3D11_USAGE_DEFAULT;
	tex_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	
	D3D11_SUBRESOURCE_DATA data = {};
	data.pSysMem = image.data;
	data.SysMemPitch = UINT(image.size.x * cg::data::byte_count(image.pixel_format));
	data.SysMemSlicePitch = UINT(byte_count(image));

	HRESULT hr = _device->CreateTexture2D(&tex_desc, &data, &_tex_diffuse_rgb.ptr);
	assert(hr == S_OK);

	// shader resource view
	D3D11_SHADER_RESOURCE_VIEW_DESC view_desc = {};
	view_desc.Format = tex_desc.Format;
	view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	view_desc.Texture2D.MostDetailedMip = 0;
	view_desc.Texture2D.MipLevels = 1;

	hr = _device->CreateShaderResourceView(_tex_diffuse_rgb.ptr, &view_desc, &_tex_diffuse_rgb_view.ptr);
	assert(hr == S_OK);

	// sampler
	D3D11_SAMPLER_DESC sampler_desc = {};
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	hr = _device->CreateSamplerState(&sampler_desc, &_sampler_state.ptr);
	assert(hr == S_OK);
}

void Static_mesh_example::init_render_states()
{
	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc = {};
	depth_stencil_desc.DepthEnable = true;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;

	HRESULT hr = _device->CreateDepthStencilState(&depth_stencil_desc, &_depth_stencil_state.ptr);
	assert(hr == S_OK);

	D3D11_RASTERIZER_DESC rasterizer_desc = {};
	rasterizer_desc.FillMode = D3D11_FILL_SOLID;
	rasterizer_desc.CullMode = D3D11_CULL_BACK;
	rasterizer_desc.FrontCounterClockwise = true;

	hr = _device->CreateRasterizerState(&rasterizer_desc, &_rasterizer_state.ptr);
	assert(hr == S_OK);
}

void Static_mesh_example::init_shaders()
{
	auto hlsl_data = cg::rnd::dx11::load_hlsl_shader_set_desc("../../data/learn_dx11/mesh_rnd/static_mesh.hlsl");
	hlsl_data.vertex_shader_entry_point = "vs_main";
	hlsl_data.pixel_shader_entry_point = "ps_main";

	_shader_set = Hlsl_shader_set(_device, hlsl_data);
}

void Static_mesh_example::setup_pipeline_state()
{
	// input assembler
	UINT offset = 0;
	UINT stride = sizeof(float2) + 2 * sizeof(float3);
	_device_ctx->IASetInputLayout(_input_layout.ptr);
	_device_ctx->IASetVertexBuffers(0, 1, &_vertex_buffer.ptr, &stride, &offset);
	_device_ctx->IASetIndexBuffer(_index_buffer.ptr, DXGI_FORMAT_R32_UINT, 0);
	_device_ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// vertex shaer
	_device_ctx->VSSetShader(_shader_set.vertex_shader(), nullptr, 0);
	_device_ctx->VSSetConstantBuffers(0, 1, &_scene_cbuffer.ptr);
	_device_ctx->VSSetConstantBuffers(1, 1, &_model_cbuffer.ptr);
	
	// pixel shader
	_device_ctx->PSSetShader(_shader_set.pixel_shader(), nullptr, 0);
	_device_ctx->PSSetSamplers(0, 1, &_sampler_state.ptr);
	_device_ctx->PSSetShaderResources(0, 1, &_tex_diffuse_rgb_view.ptr);

	// rasterizer stage
	_device_ctx->RSSetState(_rasterizer_state.ptr);

	// output merger stage
	_device_ctx->OMSetDepthStencilState(_depth_stencil_state.ptr, 1);

	HRESULT hr = _debug->ValidateContext(_device_ctx);
	assert(hr == S_OK);
}

void Static_mesh_example::update_projection_view_matrices(float wh_aspect_ratio)
{
	using cg::view_matrix;

	_projection_matrix = perspective_matrix_directx(pi_3, wh_aspect_ratio, 0.1f, 10.0f);
	_view_matrix = view_matrix(float3(2.5f, 3.0f, 1.7f), float3::zero);

	float matrix_data[32];
	to_array_column_major_order(_projection_matrix, matrix_data);
	to_array_column_major_order(_view_matrix, &matrix_data[16]);

	_device_ctx->UpdateSubresource(_scene_cbuffer.ptr, 0, nullptr, &matrix_data, 0, 0);
}

void Static_mesh_example::on_viewport_resize(const uint2& viewport_size)
{
	update_projection_view_matrices(aspect_ratio(viewport_size));
}

void Static_mesh_example::render()
{
	static const float4 clear_color = rgba(0xbca8ffff);

	clear_color_buffer(clear_color);
	clear_depth_stencil_buffer(1.0f);

	_device_ctx->DrawIndexed(_model_index_count, 0, 0);
}

} // namespace mesh_rnd
} // namespace learn_dx11