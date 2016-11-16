#ifndef LEARN_DX11_BASE_DX11_H_
#define LEARN_DX11_BASE_DX11_H_

#include <cassert>
#include <memory>
#include <type_traits>
#include "cg/math/math.h"
#include <windows.h>
#include <d3d11.h>
#include <d3dcommon.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <dxgi.h>

using namespace DirectX;


namespace learn_dx11 {

// Represent a destruction policy for COM objects.
struct Com_dispose_func final {
	void operator()(IUnknown* com_obj) noexcept
	{
		if (com_obj == nullptr) return;

		auto c = com_obj->Release();
		com_obj = nullptr;
	}
};

template<typename T>
using unique_com = std::unique_ptr<T, Com_dispose_func>;

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
		return _device.get();
	}

	ID3D11DeviceContext* device_ctx() noexcept
	{
		return _device_ctx.get();
	}

	ID3D11RenderTargetView* rtv_back_buffer() noexcept
	{
		return _rtv_back_buffer.get();
	}

	IDXGISwapChain* swap_chain() noexcept
	{
		return _swap_chain.get();
	}

private:

	void init_depth_stencil_state() noexcept;

	void init_render_target_view() noexcept;

	cg::uint2 _viewport_size;
	unique_com<ID3D11Device> _device = nullptr;
	unique_com<ID3D11DeviceContext> _device_ctx = nullptr;
	unique_com<IDXGISwapChain> _swap_chain = nullptr;
	unique_com<ID3D11RenderTargetView> _rtv_back_buffer = nullptr;
};

// Manually releases the given com object.
inline void dispose_com(IUnknown* com_obj) noexcept
{
	if (com_obj == nullptr) return;

	auto c = com_obj->Release();
	com_obj = nullptr;
}

} // namespace learn_dx11

#endif // LEARN_DX11_BASE_DX11_H_
