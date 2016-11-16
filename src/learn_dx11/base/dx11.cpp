#include "learn_dx11/base/dx11.h"

#include <exception>
#include <string>
#include <utility>
#include "cg/base/base.h"

using cg::uint2;
using cg::greater_than;
using cg::data::Hlsl_shader_set_data;


namespace {

Unique_com_ptr<ID3DBlob> compile_shader(const std::string& source_code, 
	const std::string& source_filename, const std::string& entry_point_name,
	const char* shader_model, uint32_t compile_flags)
{
	Unique_com_ptr<ID3DBlob> bytecode;
	Unique_com_ptr<ID3DBlob> error_blob;

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

Unique_com_ptr<ID3D11VertexShader> make_vertex_shader(ID3D11Device* device, const Hlsl_shader_set_data& hlsl_data)
{
	Unique_com_ptr<ID3D11VertexShader> vertex_shader;

	try {
		auto bytecode = compile_shader(hlsl_data.source_code, hlsl_data.source_filename,
			hlsl_data.vertex_shader_entry_point, "vs_5_0", hlsl_data.compile_flags);

		HRESULT hr = device->CreateVertexShader(
			bytecode->GetBufferPointer(), bytecode->GetBufferSize(),
			nullptr, &vertex_shader.ptr);

		ENFORCE(hr == S_OK, std::to_string(hr));
	}
	catch(...) {
		std::string exc_msg = EXCEPTION_MSG("Vertex shader creation error.");
		std::throw_with_nested(std::runtime_error(exc_msg));
	}

	return vertex_shader;
}

Unique_com_ptr<ID3D11PixelShader> make_pixel_shader(ID3D11Device* device, const Hlsl_shader_set_data& hlsl_data)
{
	Unique_com_ptr<ID3D11PixelShader> pixel_shader;

	try {
		auto bytecode = compile_shader(hlsl_data.source_code, hlsl_data.source_filename,
			hlsl_data.pixel_shader_entry_point, "ps_5_0", hlsl_data.compile_flags);

		HRESULT hr = device->CreatePixelShader(
			bytecode->GetBufferPointer(), bytecode->GetBufferSize(),
			nullptr, &pixel_shader.ptr);

		ENFORCE(hr == S_OK, std::to_string(hr));
	}
	catch (...) {
		std::string exc_msg = EXCEPTION_MSG("Pixel shader creation error.");
		std::throw_with_nested(std::runtime_error(exc_msg));
	}

	return pixel_shader;
}

} // namespace


namespace learn_dx11 {

// ----- Hlsl_shader_set -----

Hlsl_shader_set::Hlsl_shader_set(ID3D11Device* device, const Hlsl_shader_set_data& hlsl_data)
{
	assert(hlsl_data.has_vertex_shader());
	assert(hlsl_data.has_pixel_shader());

	_vertex_shader = make_vertex_shader(device, hlsl_data);
	_pixel_shader = make_pixel_shader(device, hlsl_data);
}

Hlsl_shader_set::Hlsl_shader_set(Hlsl_shader_set&& set) noexcept :
	_vertex_shader(std::move(set._vertex_shader)),
	_pixel_shader(std::move(set._pixel_shader))
{}

Hlsl_shader_set& Hlsl_shader_set::operator=(Hlsl_shader_set&& set) noexcept
{
	if (this == &set) return *this;

	_vertex_shader = std::move(set._vertex_shader);
	_pixel_shader = std::move(set._pixel_shader);

	return *this;
}

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
	
	hr = _device->QueryInterface<ID3D11Debug>(&_device_debug.ptr);

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