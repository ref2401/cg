#include "learn_dx11/base/dx11.h"

#include <exception>
#include <string>
#include <utility>
#include "cg/base/base.h"

using cg::uint2;
using cg::greater_than;
using cg::data::Hlsl_shader_set_data;


namespace {

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

} // namespace


namespace learn_dx11 {

// ----- Hlsl_shader_set -----

Hlsl_shader_set::Hlsl_shader_set(ID3D11Device* device, const Hlsl_shader_set_data& hlsl_data)
{
	assert(hlsl_data.has_vertex_shader());
	assert(hlsl_data.has_pixel_shader());
	
	if (hlsl_data.has_hull_shader()) {
		assert(hlsl_data.has_domain_shader());
	}

	init_vertex_shader(device, hlsl_data);
	if (hlsl_data.has_hull_shader()) init_hull_shader(device, hlsl_data);
	if (hlsl_data.has_domain_shader()) init_domain_shader(device, hlsl_data);
	init_pixel_shader(device, hlsl_data);
}

Hlsl_shader_set::Hlsl_shader_set(Hlsl_shader_set&& set) noexcept :
	_vertex_shader(std::move(set._vertex_shader)),
	_vertex_shader_bytecode(std::move(set._vertex_shader_bytecode)),
	_hull_shader(std::move(set._hull_shader)),
	_hull_shader_bytecode(std::move(set._hull_shader_bytecode)),
	_domain_shader(std::move(set._domain_shader)),
	_domain_shader_bytecode(std::move(set._domain_shader_bytecode)),
	_pixel_shader(std::move(set._pixel_shader)),
	_pixel_shader_bytecode(std::move(set._pixel_shader_bytecode))
{}

Hlsl_shader_set& Hlsl_shader_set::operator=(Hlsl_shader_set&& set) noexcept
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

void Hlsl_shader_set::init_vertex_shader(ID3D11Device* device, const Hlsl_shader_set_data& hlsl_data)
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

void Hlsl_shader_set::init_hull_shader(ID3D11Device* device, const Hlsl_shader_set_data& hlsl_data)
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

void Hlsl_shader_set::init_domain_shader(ID3D11Device* device, const Hlsl_shader_set_data& hlsl_data)
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

void Hlsl_shader_set::init_pixel_shader(ID3D11Device* device, const Hlsl_shader_set_data& hlsl_data)
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

// ----- Render_context -----

Render_context::Render_context(HWND hwnd, const cg::uint2& window_size, bool init_depth_stencil_view)
	: _viewport_size(window_size)
{
	assert(greater_than(window_size, 0));
	assert(hwnd);

	init_device(hwnd, window_size);
	update_render_target_view();
	update_depth_stencil_view(init_depth_stencil_view);
	bind_default_render_targets();
}

void Render_context::bind_default_render_targets()
{
	ID3D11RenderTargetView* rtv = _render_target_view.ptr;
	_device_ctx->OMSetRenderTargets(1, &rtv, _depth_stencil_view.ptr);
	
	D3D11_VIEWPORT viewport_desc;
	viewport_desc.TopLeftX = 0;
	viewport_desc.TopLeftY = 0;
	viewport_desc.Width = float(_viewport_size.width);
	viewport_desc.Height = float(_viewport_size.height);
	viewport_desc.MinDepth = 0.0;
	viewport_desc.MaxDepth = 1.0f;
	_device_ctx->RSSetViewports(1, &viewport_desc);
}

void Render_context::init_device(HWND hwnd, const uint2& window_size)
{
	HRESULT hr;

	DXGI_SWAP_CHAIN_DESC swap_chain_desc = {};
	swap_chain_desc.BufferCount = 2;
	swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_chain_desc.BufferDesc.Width = window_size.width;
	swap_chain_desc.BufferDesc.Height = window_size.height;
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

	D3D_FEATURE_LEVEL expected_feature_level = D3D_FEATURE_LEVEL_11_0;
	D3D_FEATURE_LEVEL actual_feature_level;

	hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
		D3D11_CREATE_DEVICE_DEBUG, &expected_feature_level, 1, D3D11_SDK_VERSION, &swap_chain_desc,
		&_swap_chain.ptr, &_device.ptr, &actual_feature_level, &_device_ctx.ptr);
	assert(hr == S_OK);
	assert(actual_feature_level == expected_feature_level);

	hr = _device->QueryInterface<ID3D11Debug>(&_debug.ptr);
}

void Render_context::update_depth_stencil_view(bool force_refresh)
{
	// force_refresh may be true only when Render_context constructor is being called.
	//
	// if not force_refresh and _depth_stencil_view was not initialized last time
	// then we must not initialize _depth_stencil_view at all.
	if (!(force_refresh || _depth_stencil_view)) return;

	_tex_depth_stencil.dispose();
	_depth_stencil_view.dispose();

	D3D11_TEXTURE2D_DESC tex_desc = {};
	tex_desc.Width = _viewport_size.width;
	tex_desc.Height = _viewport_size.height;
	tex_desc.MipLevels = 1;
	tex_desc.ArraySize = 1;
	tex_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
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

	hr = _device->CreateDepthStencilView(_tex_depth_stencil.ptr, &view_desc, &_depth_stencil_view.ptr);
	assert(hr == S_OK);
}

void Render_context::update_render_target_view()
{
	_render_target_view.dispose();

	Com_ptr<ID3D11Texture2D> tex_back_buffer;
	HRESULT hr = _swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&tex_back_buffer));
	assert(hr == S_OK);

	hr = _device->CreateRenderTargetView(tex_back_buffer.ptr, nullptr, &_render_target_view.ptr);
	assert(hr == S_OK);
}

void Render_context::resize_viewport(const cg::uint2& viewport_size)
{
	assert(greater_than(viewport_size, 0));

	const bool update_depth_stencil = !!_depth_stencil_view;
	
	_device_ctx->OMSetRenderTargets(0, nullptr, nullptr);
	_render_target_view.dispose();
	_tex_depth_stencil.dispose();
	_depth_stencil_view.dispose();

	_viewport_size = viewport_size;

	DXGI_SWAP_CHAIN_DESC swap_chain_desc;
	_swap_chain->GetDesc(&swap_chain_desc);
	_swap_chain->ResizeBuffers(swap_chain_desc.BufferCount,
		viewport_size.width, viewport_size.height,
		swap_chain_desc.BufferDesc.Format, swap_chain_desc.Flags);

	update_render_target_view();
	update_depth_stencil_view(update_depth_stencil); // if there was no depth_stencil_view then it would not be created
	bind_default_render_targets();
}

void Render_context::swap_color_buffers()
{
	_swap_chain->Present(0, 0);
}

// ----- funcs -----

Com_ptr<ID3D11Buffer> make_cbuffer(ID3D11Device* device, size_t byte_count)
{
	assert(byte_count > 0);

	D3D11_BUFFER_DESC desc = {};
	desc.ByteWidth = byte_count;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	Com_ptr<ID3D11Buffer> cbuffer;
	device->CreateBuffer(&desc, nullptr, &cbuffer.ptr);

	return cbuffer;
}

Com_ptr<ID3D11Texture2D> make_texture_2d(ID3D11Device* device,
	ID3D11Texture2D* tex_origin, const cg::uint2& size)
{
	assert(tex_origin);
	assert(greater_than(size, 0));

	D3D11_TEXTURE2D_DESC desc;
	tex_origin->GetDesc(&desc);
	desc.Width = size.width;
	desc.Height = size.height;
	
	Com_ptr<ID3D11Texture2D> tex;
	HRESULT hr = device->CreateTexture2D(&desc, nullptr, &tex.ptr);
	assert(hr == S_OK);

	return tex;
}

} // namespace learn_dx11
