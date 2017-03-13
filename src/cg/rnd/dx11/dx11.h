#ifndef CG_RND_DX11_DX11_H_
#define CG_RND_DX11_DX11_H_

#include <cstddef>
#include <windows.h>
#include <d3d11.h>
#include <d3dcommon.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <dxgi.h>
#include "cg/math/math.h"
#include "cg/rnd/rnd.h"


namespace cg {
namespace rnd {
namespace dx11 {

// Unique_com_ptr is a smart pointer that owns and manages a COM object through a pointer 
// and disposes of that object when the Unique_com_ptr goes out of scope.
template<typename T>
struct Com_ptr final {

	// Commented because unittests use fake COM interface.
	//static_assert(std::is_base_of<IUnknown, T>::value, "T must be derived from IUnknown.");


	Com_ptr() noexcept {}

	explicit Com_ptr(T* ptr) noexcept 
		: ptr(ptr)
	{}

	Com_ptr(nullptr_t) noexcept {}

	Com_ptr(const Com_ptr&) = delete;

	Com_ptr(Com_ptr&& com_ptr) noexcept 
		: ptr(com_ptr.ptr)
	{
		com_ptr.ptr = nullptr;
	}

	~Com_ptr() noexcept
	{
		dispose();
	}


	Com_ptr& operator=(const Com_ptr&) = delete;

	Com_ptr& operator=(Com_ptr&& com_ptr) noexcept
	{
		if (this == &com_ptr) return *this;

		dispose();
		ptr = com_ptr.ptr;
		com_ptr.ptr = nullptr;
		return *this;
	}

	Com_ptr& operator=(T* ptr) noexcept
	{
		dispose();
		this->ptr = ptr;
		return *this;
	}

	Com_ptr& operator=(nullptr_t) noexcept
	{
		dispose();
		return *this;
	}

	operator bool() const noexcept
	{
		return (ptr != nullptr);
	}

	T& operator*() const noexcept
	{
		return *ptr;
	}

	T* operator->() const noexcept
	{
		return ptr;
	}


	// Releases the managed COM object if such is present.
	void dispose() noexcept;

	// Releases the ownership of the managed COM object and returns a pointer to it.
	// Does not call ptr->Release(). ptr == nullptr after that. 
	T* release_ownership() noexcept
	{
		T* tmp = ptr;
		ptr = nullptr;
		return tmp;
	}

	// Pointer to the managed COM object.
	T* ptr = nullptr;
};

template<typename T>
void Com_ptr<T>::dispose() noexcept
{
	T* temp = ptr;
	if (temp == nullptr) return;

	ptr = nullptr;
	auto c = temp->Release();
}

template<typename T>
inline bool operator==(const Com_ptr<T>& l, const Com_ptr<T>& r) noexcept
{
	return l.ptr == r.ptr;
}

template<typename T>
inline bool operator==(const Com_ptr<T>& com_ptr, nullptr_t) noexcept
{
	return com_ptr.ptr == nullptr;
}

template<typename T>
inline bool operator==(nullptr_t, const Com_ptr<T>& com_ptr) noexcept
{
	return com_ptr.ptr == nullptr;
}

template<typename T>
inline bool operator!=(const Com_ptr<T>& l, const Com_ptr<T>& r) noexcept
{
	return l.ptr != r.ptr;
}

template<typename T>
inline bool operator!=(const Com_ptr<T>& com_ptr, nullptr_t) noexcept
{
	return com_ptr.ptr != nullptr;
}

template<typename T>
inline bool operator!=(nullptr_t, const Com_ptr<T>& com_ptr) noexcept
{
	return com_ptr.ptr != nullptr;
}

class DX11_rhi_context final : public virtual cg::rnd::Rhi_context_i {
public:

	DX11_rhi_context(HWND hwnd, const cg::uint2& viewport_size, cg::rnd::Depth_stencil_format depth_stencil_format);

	DX11_rhi_context(const DX11_rhi_context&) = delete;

	DX11_rhi_context(DX11_rhi_context&&) = delete;

	~DX11_rhi_context() noexcept {};


	DX11_rhi_context& operator=(const DX11_rhi_context&) = delete;

	DX11_rhi_context& operator=(DX11_rhi_context&&) = delete;


	Render_api render_api() const noexcept override 
	{
		return cg::rnd::Render_api::dx_11;
	}

	void resize_viewport(const cg::uint2& viewport_size) override;

	void swap_color_buffers() noexcept override;

private:

	void init_device(HWND hwnd, const cg::uint2& viewport_size);

	void init_depth_stencil_buffer(const cg::uint2& viewport_size,
		cg::rnd::Depth_stencil_format depth_stencil_format);

	void update_depth_stencil_buffer();

	void update_render_target_buffer();


	cg::uint2 _viewport_size;
	Com_ptr<ID3D11Device> _device;
	Com_ptr<ID3D11Debug> _debug;
	Com_ptr<ID3D11DeviceContext> _device_ctx;
	Com_ptr<IDXGISwapChain> _swap_chain;
	Com_ptr<ID3D11RenderTargetView> _rtv_window;
	Com_ptr<ID3D11Texture2D> _tex_depth_stencil;
	Com_ptr<ID3D11DepthStencilView> _dsv_depth_stencil;
};

} // namespace dx11
} // namespace rnd
} // namespace cg

#endif // CG_RND_DX11_DX11_H_

