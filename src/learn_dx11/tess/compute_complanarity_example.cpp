#include "learn_dx11/tess/compute_complanarity_example.h"

#include <cassert>

using namespace cg;


namespace learn_dx11 {
namespace tess {

Compute_complanarity_example::Compute_complanarity_example(Render_context& rnd_ctx)
	: Example(rnd_ctx),
	_viewpoint_position(0.0f, 8.0f, 16.0f)
{
	_model_matrix = scale_matrix<mat4>(float3(20.0f));
	_view_matrix = view_matrix(_viewpoint_position, float3::zero);

	_pvm_cbuffer = make_cbuffer(_device, sizeof(mat4));
	on_viewport_resize(rnd_ctx.viewport_size());

	setup_pipeline_state();
}

void Compute_complanarity_example::init_pipeline_state()
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

void Compute_complanarity_example::on_viewport_resize(const cg::uint2& viewport_size)
{
	update_projection_matrix(viewport_size.aspect_ratio());
	setup_pvm_matrix();
}

void Compute_complanarity_example::render()
{
	const float4 clear_color = cg::rgba(0xd1d7ffff);

	clear_color_buffer(clear_color);
	clear_depth_stencil_buffer(1.0f);
}

void Compute_complanarity_example::setup_pipeline_state()
{
}

void Compute_complanarity_example::setup_pvm_matrix()
{
	const mat4 pvm_matrix = _projection_matrix * _view_matrix * _model_matrix;

	float arr[16];
	to_array_column_major_order(pvm_matrix, arr);
	_device_ctx->UpdateSubresource(_pvm_cbuffer.ptr, 0, nullptr, arr, 0, 0);
}

void Compute_complanarity_example::update_projection_matrix(float aspect_ratio)
{
	assert(aspect_ratio > 0.0f);
	_projection_matrix = cg::perspective_matrix_directx(cg::pi_3, aspect_ratio, 0.1f, 100.0f);
}



} // namespace tess
} // namespace learn_dx11
