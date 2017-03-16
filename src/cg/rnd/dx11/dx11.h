#ifndef CG_RND_DX11_DX11_H_
#define CG_RND_DX11_DX11_H_

#include <cstddef>
#include <windows.h>
#include <d3d11.h>
#include <d3dcommon.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <dxgi.h>
#include "cg/data/shader.h"
#include "cg/math/math.h"
#include "cg/rnd/rnd.h"

using namespace DirectX;


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

	~DX11_rhi_context() noexcept = default;


	DX11_rhi_context& operator=(const DX11_rhi_context&) = delete;

	DX11_rhi_context& operator=(DX11_rhi_context&&) = delete;


	ID3D11Debug* debug() noexcept
	{
		return _debug.ptr;
	}

	ID3D11Device* device() noexcept
	{
		return _device.ptr;
	}

	ID3D11DeviceContext* device_ctx() noexcept
	{
		return _device_ctx.ptr;
	}

	ID3D11DepthStencilView* dsv_depth_stencil() noexcept
	{
		return _dsv_depth_stencil.ptr;
	}

	ID3D11RenderTargetView* rtv_window() noexcept
	{
		return _rtv_window.ptr;
	}

	Render_api render_api() const noexcept override 
	{
		return cg::rnd::Render_api::dx_11;
	}

	const D3D11_VIEWPORT& viewport() const noexcept
	{
		return _viewport;
	}

	float viewport_aspect_ratio() const noexcept
	{
		assert(!cg::approx_equal(_viewport.Height, 0.0f));
		return _viewport.Width / _viewport.Height;
	}


	void bind_window_render_targets();

	void resize_viewport(const cg::uint2& viewport_size) override;

	void swap_color_buffers() override;

private:

	void init_device(HWND hwnd, const cg::uint2& viewport_size);

	void init_depth_stencil_buffer(const cg::uint2& viewport_size,
		cg::rnd::Depth_stencil_format depth_stencil_format);

	void update_depth_stencil_buffer(const uint2& viewport_size);

	void update_render_target_buffer();

	void update_viewport(const cg::uint2& viewport_size);


	Com_ptr<ID3D11Device> _device;
	Com_ptr<ID3D11Debug> _debug;
	Com_ptr<ID3D11DeviceContext> _device_ctx;
	Com_ptr<IDXGISwapChain> _swap_chain;
	Com_ptr<ID3D11RenderTargetView> _rtv_window;
	Com_ptr<ID3D11Texture2D> _tex_depth_stencil;
	Com_ptr<ID3D11DepthStencilView> _dsv_depth_stencil;
	D3D11_VIEWPORT _viewport;
};

class Hlsl_shader final {
public:

	Hlsl_shader() noexcept = default;

	Hlsl_shader(ID3D11Device* device, const cg::data::Hlsl_shader_desc& hlsl_desc);

	Hlsl_shader(const Hlsl_shader&) = delete;

	Hlsl_shader(Hlsl_shader&& set) noexcept;

	~Hlsl_shader() noexcept = default;


	Hlsl_shader& operator=(const Hlsl_shader&) = delete;

	Hlsl_shader& operator=(Hlsl_shader&& set) noexcept;


	ID3D11VertexShader* vertex_shader() noexcept
	{
		return _vertex_shader.ptr;
	}

	ID3DBlob* vertex_shader_bytecode() noexcept
	{
		return _vertex_shader_bytecode.ptr;
	}

	ID3D11HullShader* hull_shader() noexcept
	{
		return _hull_shader.ptr;
	}

	ID3DBlob* hull_shader_bytecode() noexcept
	{
		return _hull_shader_bytecode.ptr;
	}

	ID3D11DomainShader* domain_shader() noexcept
	{
		return _domain_shader.ptr;
	}

	ID3DBlob* domain_shader_bytecode() noexcept
	{
		return _domain_shader_bytecode.ptr;
	}

	ID3D11PixelShader* pixel_shader() noexcept
	{
		return _pixel_shader.ptr;
	}

	ID3DBlob* pixel_shader_bytecode() noexcept
	{
		return _pixel_shader_bytecode.ptr;
	}

private:

	void init_vertex_shader(ID3D11Device* device, const cg::data::Hlsl_shader_desc& hlsl_data);

	void init_hull_shader(ID3D11Device* device, const cg::data::Hlsl_shader_desc& hlsl_data);

	void init_domain_shader(ID3D11Device* device, const cg::data::Hlsl_shader_desc& hlsl_data);

	void init_pixel_shader(ID3D11Device* device, const cg::data::Hlsl_shader_desc& hlsl_data);

	Com_ptr<ID3D11VertexShader> _vertex_shader;
	Com_ptr<ID3DBlob> _vertex_shader_bytecode;
	Com_ptr<ID3D11HullShader> _hull_shader;
	Com_ptr<ID3DBlob> _hull_shader_bytecode;
	Com_ptr<ID3D11DomainShader> _domain_shader;
	Com_ptr<ID3DBlob> _domain_shader_bytecode;
	Com_ptr<ID3D11PixelShader> _pixel_shader;
	Com_ptr<ID3DBlob> _pixel_shader_bytecode;
};


// Creates an unitialized constant buffer object.
Com_ptr<ID3D11Buffer> constant_buffer(ID3D11Device* device, size_t byte_count);

// Creates an unitialized constant buffer object.
template<typename T_cbuffer_data>
inline Com_ptr<ID3D11Buffer> constant_buffer(ID3D11Device* device)
{
	return constant_buffer(device, sizeof(T_cbuffer_data));
}

} // namespace dx11
} // namespace rnd
} // namespace cg

#endif // CG_RND_DX11_DX11_H_

