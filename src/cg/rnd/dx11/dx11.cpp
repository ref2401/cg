#include "cg/rnd/dx11/dx11.h"


using namespace cg;
using cg::rnd::Depth_stencil_format;


namespace {

DXGI_FORMAT dxgi_format(Depth_stencil_format format)
{
	switch (format) {
		default:
		case Depth_stencil_format::none:				return DXGI_FORMAT_UNKNOWN;
		case Depth_stencil_format::depth_24_stencil_8:	return DXGI_FORMAT_D24_UNORM_S8_UINT;
		case Depth_stencil_format::depth_32f:			return DXGI_FORMAT_D32_FLOAT;
		
		case Depth_stencil_format::depth_32:			
			assert(false); // format is not supported
			return DXGI_FORMAT_UNKNOWN;
	}
}

} // namespace


namespace cg {
namespace rnd {
namespace dx11 {

// ----- DX11_rhi_context -----
DX11_rhi_context::DX11_rhi_context(HWND hwnd, const cg::uint2& viewport_size, 
	Depth_stencil_format depth_stencil_format)
	: _viewport_size(viewport_size)
{
	assert(hwnd);
	assert(greater_than(viewport_size, 0));
	assert(depth_stencil_format == Depth_stencil_format::none
		|| depth_stencil_format == Depth_stencil_format::depth_24_stencil_8); // NOTE(ref2401): the other options are not implemented

	init_device(hwnd, viewport_size);
	init_depth_stencil_buffer(viewport_size, depth_stencil_format);
	update_render_target_buffer();
}

void DX11_rhi_context::init_device(HWND hwnd, const cg::uint2& viewport_size)
{
	DXGI_SWAP_CHAIN_DESC swap_chain_desc = {};
	swap_chain_desc.BufferCount = 2;
	swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_chain_desc.BufferDesc.Width = viewport_size.width;
	swap_chain_desc.BufferDesc.Height = viewport_size.height;
	swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swap_chain_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swap_chain_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swap_chain_desc.BufferDesc.RefreshRate.Numerator = 60;
	swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;
	swap_chain_desc.SampleDesc.Count = 1;
	swap_chain_desc.SampleDesc.Quality = 0;
	swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swap_chain_desc.Windowed = true;
	swap_chain_desc.OutputWindow = hwnd;

	const D3D_FEATURE_LEVEL expected_feature_level = D3D_FEATURE_LEVEL_11_0;
	D3D_FEATURE_LEVEL actual_feature_level;

	HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
		D3D11_CREATE_DEVICE_DEBUG, &expected_feature_level, 1, D3D11_SDK_VERSION, &swap_chain_desc,
		&_swap_chain.ptr, &_device.ptr, &actual_feature_level, &_device_ctx.ptr);
	assert(hr == S_OK);
	assert(actual_feature_level == expected_feature_level);

#if !defined(NDEBUG)
	hr = _device->QueryInterface<ID3D11Debug>(&_debug.ptr);
	assert(hr == S_OK);
#endif
}

void DX11_rhi_context::init_depth_stencil_buffer(const cg::uint2& viewport_size,
	Depth_stencil_format depth_stencil_format)
{
	if (depth_stencil_format == cg::rnd::Depth_stencil_format::none) return;

	D3D11_TEXTURE2D_DESC tex_desc = {};
	tex_desc.Width = viewport_size.width;
	tex_desc.Height = viewport_size.height;
	tex_desc.MipLevels = 1;
	tex_desc.ArraySize = 1;
	tex_desc.Format = dxgi_format(depth_stencil_format);
	tex_desc.SampleDesc.Count = 1;
	tex_desc.SampleDesc.Quality = 0;
	tex_desc.Usage = D3D11_USAGE_DEFAULT;
	tex_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	HRESULT hr = _device->CreateTexture2D(&tex_desc, nullptr, &_tex_depth_stencil.ptr);
	assert(hr == S_OK);

	D3D11_DEPTH_STENCIL_VIEW_DESC view_desc = {};
	view_desc.Format = tex_desc.Format;
	view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	view_desc.Texture2D.MipSlice = 0;

	hr = _device->CreateDepthStencilView(_tex_depth_stencil.ptr, &view_desc, &_dsv_depth_stencil.ptr);
	assert(hr == S_OK);
}

void DX11_rhi_context::resize_viewport(const cg::uint2& viewport_size)
{
	assert(greater_than(viewport_size, 0));

	_device_ctx->OMSetRenderTargets(0, nullptr, nullptr);
	_viewport_size = viewport_size;

	DXGI_SWAP_CHAIN_DESC swap_chain_desc;
	_swap_chain->GetDesc(&swap_chain_desc);
	_swap_chain->ResizeBuffers(swap_chain_desc.BufferCount,
		viewport_size.width, viewport_size.height,
		swap_chain_desc.BufferDesc.Format, swap_chain_desc.Flags);

	update_render_target_buffer();
	update_depth_stencil_buffer();
	//bind_default_render_targets();
}

void DX11_rhi_context::swap_color_buffers() noexcept
{
	_swap_chain->Present(0, 0);
}

void DX11_rhi_context::update_depth_stencil_buffer()
{
	// if depth stencil view was not created then there is no point in updating it.
	if (!_dsv_depth_stencil) return;

	D3D11_TEXTURE2D_DESC tex_desc;
	_tex_depth_stencil->GetDesc(&tex_desc);
	tex_desc.Width = _viewport_size.width;
	tex_desc.Height = _viewport_size.height;


	_tex_depth_stencil.dispose();
	_dsv_depth_stencil.dispose();

	HRESULT hr = _device->CreateTexture2D(&tex_desc, nullptr, &_tex_depth_stencil.ptr);
	assert(hr == S_OK);

	D3D11_DEPTH_STENCIL_VIEW_DESC view_desc = {};
	view_desc.Format = tex_desc.Format;
	view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	view_desc.Texture2D.MipSlice = 0;

	hr = _device->CreateDepthStencilView(_tex_depth_stencil.ptr, &view_desc, &_dsv_depth_stencil.ptr);
	assert(hr == S_OK);
}

void DX11_rhi_context::update_render_target_buffer()
{
	// render target view
	_rtv_window.dispose();

	Com_ptr<ID3D11Texture2D> tex_back_buffer;
	HRESULT hr = _swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&tex_back_buffer));
	assert(hr == S_OK);

	hr = _device->CreateRenderTargetView(tex_back_buffer.ptr, nullptr, &_rtv_window.ptr);
	assert(hr == S_OK);
}

} // namespace dx11
} // namespace rnd
} // namesapce cg
