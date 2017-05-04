#include "learn_dx11/tess/terrain_tessellation_example.h"

#include <cassert>
#include <iterator>
#include <type_traits>
#include <vector>
#include "cg/base/container.h"
#include "learn_dx11/tess/terrain_grid_model.h"

using namespace cg;


namespace learn_dx11 {
namespace tess {


// ----- Terrain_tessellation_example -----

Terrain_tessellation_example::Terrain_tessellation_example(Render_context& rnd_ctx) 
	: Example(rnd_ctx),
	_viewpoint_position(-8, 8, 16)
{
	_model_matrix = scale_matrix<float4x4>(float3(20.0f));
	_view_matrix = view_matrix(_viewpoint_position, float3::zero);
	
	init_cbuffers();
	init_shaders();
	init_geometry(); // vertex shader bytecode is required to create vertex input layout
	init_pipeline_state();

	update_projection_matrix(aspect_ratio(rnd_ctx.viewport_size()));
	setup_pvm_matrix();
	setup_pipeline_state();
}

void Terrain_tessellation_example::init_cbuffers()
{
	// pvm_matrix_cbuffer
	_pvm_matrix_cbuffer = make_cbuffer(_device, sizeof(float4x4));

	// tess control cbuffer
	// 8 = 3 (camera position) + 2 (lod_min_max) + 2(distance_min_max) + 1 (stub float)
	const float4 camepa_position_ms = mul(inverse(_model_matrix), _viewpoint_position);
	const float distance = len(camepa_position_ms);
	const float tess_arr[12] = {
		camepa_position_ms.x, camepa_position_ms.y, camepa_position_ms.z, camepa_position_ms.w,
		0.1f * distance, 1.5f * distance, 0.0f, 0.0f,	// distance_min_max
		1.0f, 32.0f, 0.0f, 0.0f,						// lod_min_max
	};
	_tess_control_cbuffer = make_cbuffer(_device, sizeof(float) * std::extent<decltype(tess_arr)>::value);
	_device_ctx->UpdateSubresource(_tess_control_cbuffer.ptr, 0, nullptr, tess_arr, 0, 0);
}

void Terrain_tessellation_example::init_geometry()
{
	Terrain_grid_model terrain_model(8, 8);
	_index_count = UINT(terrain_model.index_count());

	// vertex buffer
	D3D11_BUFFER_DESC vb_desc = {};
	vb_desc.ByteWidth = UINT(terrain_model.vertex_buffer_byte_count());
	vb_desc.Usage = D3D11_USAGE_IMMUTABLE;
	vb_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	D3D11_SUBRESOURCE_DATA vb_data = {};
	vb_data.pSysMem = terrain_model.vertex_buffer_data();

	HRESULT hr = _device->CreateBuffer(&vb_desc, &vb_data, &_vertex_buffer.ptr);
	assert(hr == S_OK);

	// index buffer
	D3D11_BUFFER_DESC ib_desc = {};
	ib_desc.ByteWidth = UINT(terrain_model.index_buffer_byte_count());
	ib_desc.Usage = D3D11_USAGE_IMMUTABLE;
	ib_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	D3D11_SUBRESOURCE_DATA ib_data = {};
	ib_data.pSysMem = terrain_model.index_buffer_data();

	hr = _device->CreateBuffer(&ib_desc, &ib_data, &_index_buffer.ptr);
	assert(hr == S_OK);

	// input layout
	D3D11_INPUT_ELEMENT_DESC layout_desc[2] = {
		{ "V_POSITION" , 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "V_TEX_COORD" , 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(float3), D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	hr = _device->CreateInputLayout(layout_desc, 2,
		_shader_set.vertex_shader_bytecode()->GetBufferPointer(),
		_shader_set.vertex_shader_bytecode()->GetBufferSize(),
		&_input_layout.ptr);
	assert(hr == S_OK);
}

void Terrain_tessellation_example::init_pipeline_state()
{
	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc = {};
	depth_stencil_desc.DepthEnable = true;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;

	HRESULT hr = _device->CreateDepthStencilState(&depth_stencil_desc, &_depth_stencil_state.ptr);
	assert(hr == S_OK);

	// default rastr state
	D3D11_RASTERIZER_DESC default_rastr_desc = {};
	default_rastr_desc.FillMode = D3D11_FILL_SOLID;
	default_rastr_desc.CullMode = D3D11_CULL_BACK;
	default_rastr_desc.FrontCounterClockwise = true;
	hr = _device->CreateRasterizerState(&default_rastr_desc, &_default_rasterizer_state.ptr);
	assert(hr == S_OK);

	// wireframe rastr state
	D3D11_RASTERIZER_DESC wireframe_rastr_desc = default_rastr_desc;
	wireframe_rastr_desc.FillMode = D3D11_FILL_WIREFRAME;
	hr = _device->CreateRasterizerState(&wireframe_rastr_desc, &_wireframe_rasterizer_state.ptr);
	assert(hr == S_OK);
}

void Terrain_tessellation_example::init_shaders()
{
	auto shader_desc = cg::rnd::dx11::load_hlsl_shader_set_desc("../../data/learn_dx11/tess/terrain_tessellation.hlsl");

	shader_desc.vertex_shader_entry_point = "vs_main";
	shader_desc.hull_shader_entry_point = "hs_main";
	shader_desc.domain_shader_entry_point = "ds_main";
	shader_desc.pixel_shader_entry_point = "ps_main";

	_shader_set = Hlsl_shader_set(_device, shader_desc);
}

void Terrain_tessellation_example::on_viewport_resize(const uint2& viewport_size)
{
	update_projection_matrix(aspect_ratio(viewport_size));
	setup_pvm_matrix();
}

void Terrain_tessellation_example::render()
{
	const float4 clear_color = rgba(0xd1d7ffff);

	clear_color_buffer(clear_color);
	clear_depth_stencil_buffer(1.0f);

	_device_ctx->DrawIndexed(_index_count, 0, 0);
}

void Terrain_tessellation_example::setup_pipeline_state()
{
	// input assembler
	constexpr UINT offset = 0;
	constexpr UINT vertex_byte_count = sizeof(float) * Terrain_grid_model::vertex_component_count;
	_device_ctx->IASetInputLayout(_input_layout.ptr);
	_device_ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_12_CONTROL_POINT_PATCHLIST);
	_device_ctx->IASetVertexBuffers(0, 1, &_vertex_buffer.ptr, &vertex_byte_count, &offset);
	_device_ctx->IASetIndexBuffer(_index_buffer.ptr, DXGI_FORMAT_R32_UINT, 0);

	// vertex shader
	_device_ctx->VSSetShader(_shader_set.vertex_shader(), nullptr, 0);
	// hull shader
	_device_ctx->HSSetShader(_shader_set.hull_shader(), nullptr, 0);
	_device_ctx->HSSetConstantBuffers(0, 1, &_tess_control_cbuffer.ptr);
	// domain shader
	_device_ctx->DSSetShader(_shader_set.domain_shader(), nullptr, 0);
	_device_ctx->DSSetConstantBuffers(0, 1, &_pvm_matrix_cbuffer.ptr);
	// pixel shader
	_device_ctx->PSSetShader(_shader_set.pixel_shader(), nullptr, 0);

	_device_ctx->RSSetState(_wireframe_rasterizer_state.ptr);
	//_device_ctx->RSSetState(_default_rasterizer_state.ptr);

	// output merger
	_device_ctx->OMSetDepthStencilState(_depth_stencil_state.ptr, 0);

	HRESULT hr = _debug->ValidateContext(_device_ctx);
	assert(hr == S_OK);
}

void Terrain_tessellation_example::setup_pvm_matrix()
{
	const float4x4 mat = _projection_matrix * _view_matrix * _model_matrix;

	float arr[16];
	to_array_column_major_order(mat, arr);
	_device_ctx->UpdateSubresource(_pvm_matrix_cbuffer.ptr, 0, nullptr, arr, 0, 0);
}

void Terrain_tessellation_example::update_projection_matrix(float aspect_ratio)
{
	assert(aspect_ratio > 0.0f);

	_projection_matrix = perspective_matrix_directx(pi_3, aspect_ratio, 0.1f, 100.0f);
}

} // namespace tess
} // namespace learn_dx11
