#ifndef LEARN_DX11_BASE_DX11_H_
#define LEARN_DX11_BASE_DX11_H_

#include <cassert>
#include <memory>
#include <type_traits>
#include "cg/data/shader.h"
#include "cg/math/math.h"
#include "cg/rnd/utility/utility.h"
#include <windows.h>
#include <d3d11.h>
#include <d3dcommon.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <dxgi.h>

using cg::rnd::utility::Unique_com_ptr;
using namespace DirectX;


namespace learn_dx11 {

class Hlsl_shader_set final {
public:

	Hlsl_shader_set() noexcept = default;

	Hlsl_shader_set(ID3D11Device* device, const cg::data::Hlsl_shader_set_data& hlsl_data);

	Hlsl_shader_set(const Hlsl_shader_set&) = delete;

	Hlsl_shader_set(Hlsl_shader_set&& set) noexcept;

	~Hlsl_shader_set() noexcept = default;


	Hlsl_shader_set& operator=(const Hlsl_shader_set&) = delete;

	Hlsl_shader_set& operator=(Hlsl_shader_set&& set) noexcept;


	ID3D11VertexShader* vertex_shader() noexcept
	{
		return _vertex_shader.ptr;
	}

	ID3D11PixelShader* pixel_shader() noexcept
	{
		return _pixel_shader.ptr;
	}

private:
	Unique_com_ptr<ID3D11VertexShader> _vertex_shader;
	Unique_com_ptr<ID3D11PixelShader> _pixel_shader;
};

// The Render_context class provides access to all the DirectX's essential objects
// which are required by every example from this project.
class Render_context final {
public:
	Render_context(const cg::uint2& viewport_size, HWND hwnd) noexcept;

	Render_context(const Render_context&) = delete;

	Render_context(Render_context&&) = delete;


	Render_context& operator=(const Render_context&) = delete;

	Render_context& operator=(Render_context&&) = delete;


	ID3D11Device* device() noexcept
	{
		return _device.ptr;
	}

	ID3D11DeviceContext* device_ctx() noexcept
	{
		return _device_ctx.ptr;
	}

	ID3D11Debug* device_debug() noexcept
	{
		return _device_debug.ptr;
	}

	ID3D11RenderTargetView* rtv_back_buffer() noexcept
	{
		return _rtv_back_buffer.ptr;
	}

	IDXGISwapChain* swap_chain() noexcept
	{
		return _swap_chain.ptr;
	}

private:

	void init_depth_stencil_state() noexcept;

	void init_render_target_view() noexcept;

	cg::uint2 _viewport_size;
	Unique_com_ptr<ID3D11Device> _device;
	Unique_com_ptr<ID3D11Debug> _device_debug;
	Unique_com_ptr<ID3D11DeviceContext> _device_ctx;
	Unique_com_ptr<IDXGISwapChain> _swap_chain;
	Unique_com_ptr<ID3D11RenderTargetView> _rtv_back_buffer;
};


} // namespace learn_dx11

#endif // LEARN_DX11_BASE_DX11_H_
