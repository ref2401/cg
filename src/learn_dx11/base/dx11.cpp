#include "learn_dx11/base/dx11.h"


using cg::uint2;
using cg::greater_than;


namespace learn_dx11 {

// ----- Rendering_context -----

Render_context::Render_context(const uint2& viewport_size, HWND hwnd) noexcept :
	_viewport_size(viewport_size)
{
	assert(greater_than(viewport_size, 0));
	assert(hwnd);

	HRESULT hr;

	DXGI_SWAP_CHAIN_DESC swap_chain_desc = {};
	swap_chain_desc.BufferCount = 2;
	swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_chain_desc.BufferDesc.Width = viewport_size.width;
	swap_chain_desc.BufferDesc.Height = viewport_size.height;
	swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	swap_chain_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swap_chain_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swap_chain_desc.BufferDesc.RefreshRate.Numerator = 60;
	swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;
	swap_chain_desc.SampleDesc.Count = 1;
	swap_chain_desc.SampleDesc.Quality = 0;
	swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swap_chain_desc.Windowed = true;
	swap_chain_desc.OutputWindow = hwnd;

	D3D_FEATURE_LEVEL expected_feature_level = D3D_FEATURE_LEVEL_11_0;
	D3D_FEATURE_LEVEL actual_feature_level;
	
	hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
		D3D11_CREATE_DEVICE_DEBUG, &expected_feature_level, 1, D3D11_SDK_VERSION, &swap_chain_desc,
		&_swap_chain.ptr, &_device.ptr, &actual_feature_level, &_device_ctx.ptr);
	assert(hr == S_OK);
	assert(actual_feature_level == expected_feature_level);
	
	init_render_target_view();
	init_depth_stencil_state();
}

void Render_context::init_depth_stencil_state() noexcept
{
	D3D11_DEPTH_STENCIL_DESC desc = {};
	
	Unique_com_ptr<ID3D11DepthStencilState> state = nullptr;
	HRESULT hr =_device->CreateDepthStencilState(&desc, &state.ptr);
	assert(hr == S_OK);

	_device_ctx->OMSetDepthStencilState(state.ptr, 1);
}

void Render_context::init_render_target_view() noexcept
{
	Unique_com_ptr<ID3D11Texture2D> tex_back_buffer;
	HRESULT hr = _swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&tex_back_buffer));
	assert(hr == S_OK);

	hr = _device->CreateRenderTargetView(tex_back_buffer.ptr, nullptr, &_rtv_back_buffer.ptr);
	assert(hr == S_OK);

	_device_ctx->OMSetRenderTargets(1, &_rtv_back_buffer.ptr, nullptr);
}

} // namespace learn_dx11