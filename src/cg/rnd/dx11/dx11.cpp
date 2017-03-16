#include "cg/rnd/dx11/dx11.h"

#include "cg/base/base.h"


using namespace cg;
using cg::data::Hlsl_shader_desc;
using cg::rnd::Depth_stencil_format;


namespace {

using namespace cg::rnd::dx11;


Com_ptr<ID3DBlob> compile_shader(const std::string& source_code,
	const std::string& source_filename, const std::string& entry_point_name,
	const char* shader_model, uint32_t compile_flags)
{
	Com_ptr<ID3DBlob> bytecode;
	Com_ptr<ID3DBlob> error_blob;

	HRESULT hr = D3DCompile(
		source_code.c_str(),
		source_code.size(),
		source_filename.c_str(),
		nullptr,					// defines
		nullptr,					// includes
		entry_point_name.c_str(),
		shader_model,
		compile_flags,
		0,							// effect compilation flags
		&bytecode.ptr,
		&error_blob.ptr
	);

	if (hr != S_OK) {
		std::string error(static_cast<char*>(error_blob->GetBufferPointer()), error_blob->GetBufferSize());
		throw std::runtime_error(error);
	}

	return bytecode;
}

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
{
	assert(hwnd);
	assert(greater_than(viewport_size, 0));
	assert(depth_stencil_format == Depth_stencil_format::none
		|| depth_stencil_format == Depth_stencil_format::depth_24_stencil_8); // NOTE(ref2401): the other options are not implemented

	update_viewport(viewport_size);
	init_device(hwnd, viewport_size);
	init_depth_stencil_buffer(viewport_size, depth_stencil_format);
	update_render_target_buffer();
	bind_window_render_targets();
}

void DX11_rhi_context::bind_window_render_targets()
{
	_device_ctx->OMSetRenderTargets(1, &_rtv_window.ptr, _dsv_depth_stencil.ptr);
	_device_ctx->RSSetViewports(1, &_viewport);
}

void DX11_rhi_context::init_device(HWND hwnd, const uint2& viewport_size)
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

void DX11_rhi_context::init_depth_stencil_buffer(const uint2& viewport_size,
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

void DX11_rhi_context::resize_viewport(const uint2& viewport_size)
{
	assert(greater_than(viewport_size, 0));
	if (viewport_size == uint2(uint32_t(_viewport.Width), uint32_t(_viewport.Height))) return;

	_device_ctx->OMSetRenderTargets(0, nullptr, nullptr);

	DXGI_SWAP_CHAIN_DESC swap_chain_desc;
	_swap_chain->GetDesc(&swap_chain_desc);
	_swap_chain->ResizeBuffers(swap_chain_desc.BufferCount,
		viewport_size.width, viewport_size.height,
		swap_chain_desc.BufferDesc.Format, swap_chain_desc.Flags);

	update_viewport(viewport_size);
	update_render_target_buffer();
	update_depth_stencil_buffer(viewport_size);
	bind_window_render_targets();
}

void DX11_rhi_context::swap_color_buffers()
{
	_swap_chain->Present(0, 0);
}

void DX11_rhi_context::update_depth_stencil_buffer(const uint2& viewport_size)
{
	// if depth stencil view was not created then there is no point in updating it.
	if (!_dsv_depth_stencil) return;

	D3D11_TEXTURE2D_DESC tex_desc;
	_tex_depth_stencil->GetDesc(&tex_desc);
	tex_desc.Width = viewport_size.width;
	tex_desc.Height = viewport_size.height;


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
	_rtv_window.dispose();

	Com_ptr<ID3D11Texture2D> tex_back_buffer;
	HRESULT hr = _swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&tex_back_buffer));
	assert(hr == S_OK);

	hr = _device->CreateRenderTargetView(tex_back_buffer.ptr, nullptr, &_rtv_window.ptr);
	assert(hr == S_OK);
}

void DX11_rhi_context::update_viewport(const uint2& viewport_size)
{
	_viewport.TopLeftX = 0;
	_viewport.TopLeftY = 0;
	_viewport.Width = float(viewport_size.width);
	_viewport.Height = float(viewport_size.height);
	_viewport.MinDepth = 0.0;
	_viewport.MaxDepth = 1.0f;
}

// ----- Hlsl_shader -----

Hlsl_shader::Hlsl_shader(ID3D11Device* device, const Hlsl_shader_desc& hlsl_desc)
{
	assert(hlsl_desc.has_vertex_shader());
	assert(hlsl_desc.has_pixel_shader());

	if (hlsl_desc.has_hull_shader()) {
		assert(hlsl_desc.has_domain_shader());
	}

	init_vertex_shader(device, hlsl_desc);
	if (hlsl_desc.has_hull_shader()) init_hull_shader(device, hlsl_desc);
	if (hlsl_desc.has_domain_shader()) init_domain_shader(device, hlsl_desc);
	init_pixel_shader(device, hlsl_desc);
}

Hlsl_shader::Hlsl_shader(Hlsl_shader&& set) noexcept
	: _vertex_shader(std::move(set._vertex_shader)),
	_vertex_shader_bytecode(std::move(set._vertex_shader_bytecode)),
	_hull_shader(std::move(set._hull_shader)),
	_hull_shader_bytecode(std::move(set._hull_shader_bytecode)),
	_domain_shader(std::move(set._domain_shader)),
	_domain_shader_bytecode(std::move(set._domain_shader_bytecode)),
	_pixel_shader(std::move(set._pixel_shader)),
	_pixel_shader_bytecode(std::move(set._pixel_shader_bytecode))
{}

Hlsl_shader& Hlsl_shader::operator=(Hlsl_shader&& set) noexcept
{
	if (this == &set) return *this;

	_vertex_shader = std::move(set._vertex_shader);
	_vertex_shader_bytecode = std::move(set._vertex_shader_bytecode);
	_hull_shader = std::move(set._hull_shader);
	_hull_shader_bytecode = std::move(set._hull_shader_bytecode);
	_domain_shader = std::move(set._domain_shader);
	_domain_shader_bytecode = std::move(set._domain_shader_bytecode);
	_pixel_shader = std::move(set._pixel_shader);
	_pixel_shader_bytecode = std::move(set._pixel_shader_bytecode);

	return *this;
}

void Hlsl_shader::init_vertex_shader(ID3D11Device* device, const Hlsl_shader_desc& hlsl_data)
{
	try {
		_vertex_shader_bytecode = compile_shader(hlsl_data.source_code, hlsl_data.source_filename,
			hlsl_data.vertex_shader_entry_point, "vs_5_0", hlsl_data.compile_flags);

		HRESULT hr = device->CreateVertexShader(
			_vertex_shader_bytecode->GetBufferPointer(),
			_vertex_shader_bytecode->GetBufferSize(),
			nullptr, &_vertex_shader.ptr);

		ENFORCE(hr == S_OK, std::to_string(hr));
	}
	catch (...) {
		std::string exc_msg = EXCEPTION_MSG("Vertex shader creation error.");
		std::throw_with_nested(std::runtime_error(exc_msg));
	}
}

void Hlsl_shader::init_hull_shader(ID3D11Device* device, const Hlsl_shader_desc& hlsl_data)
{
	try {
		_hull_shader_bytecode = compile_shader(hlsl_data.source_code, hlsl_data.source_filename,
			hlsl_data.hull_shader_entry_point, "hs_5_0", hlsl_data.compile_flags);

		HRESULT hr = device->CreateHullShader(
			_hull_shader_bytecode->GetBufferPointer(),
			_hull_shader_bytecode->GetBufferSize(),
			nullptr, &_hull_shader.ptr);

		ENFORCE(hr == S_OK, std::to_string(hr));
	}
	catch (...) {
		std::string exc_msg = EXCEPTION_MSG("Hull shader creation error.");
		std::throw_with_nested(std::runtime_error(exc_msg));
	}
}

void Hlsl_shader::init_domain_shader(ID3D11Device* device, const Hlsl_shader_desc& hlsl_data)
{
	try {
		_domain_shader_bytecode = compile_shader(hlsl_data.source_code, hlsl_data.source_filename,
			hlsl_data.domain_shader_entry_point, "ds_5_0", hlsl_data.compile_flags);

		HRESULT hr = device->CreateDomainShader(
			_domain_shader_bytecode->GetBufferPointer(),
			_domain_shader_bytecode->GetBufferSize(),
			nullptr, &_domain_shader.ptr);

		ENFORCE(hr == S_OK, std::to_string(hr));
	}
	catch (...) {
		std::string exc_msg = EXCEPTION_MSG("Domain shader creation error.");
		std::throw_with_nested(std::runtime_error(exc_msg));
	}
}

void Hlsl_shader::init_pixel_shader(ID3D11Device* device, const Hlsl_shader_desc& hlsl_data)
{
	try {
		_pixel_shader_bytecode = compile_shader(hlsl_data.source_code, hlsl_data.source_filename,
			hlsl_data.pixel_shader_entry_point, "ps_5_0", hlsl_data.compile_flags);

		HRESULT hr = device->CreatePixelShader(
			_pixel_shader_bytecode->GetBufferPointer(),
			_pixel_shader_bytecode->GetBufferSize(),
			nullptr, &_pixel_shader.ptr);

		ENFORCE(hr == S_OK, std::to_string(hr));
	}
	catch (...) {
		std::string exc_msg = EXCEPTION_MSG("Pixel shader creation error.");
		std::throw_with_nested(std::runtime_error(exc_msg));
	}
}

// ----- funcs ------

Com_ptr<ID3D11Buffer> constant_buffer(ID3D11Device* device, size_t byte_count)
{
	assert(byte_count > 0);

	D3D11_BUFFER_DESC desc = {};
	desc.ByteWidth = byte_count;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	Com_ptr<ID3D11Buffer> buffer;
	HRESULT hr = device->CreateBuffer(&desc, nullptr, &buffer.ptr);
	assert(hr == S_OK);

	return buffer;
}

} // namespace dx11
} // namespace rnd
} // namesapce cg
