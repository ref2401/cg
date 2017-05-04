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
#include "cg/base/math.h"
#include "cg/rnd/rnd.h"
#include <wrl/client.h>
using namespace DirectX;


namespace cg {
namespace rnd {
namespace dx11 {

// Unique_com_ptr is a smart pointer that owns and manages a COM object through a pointer 
// and disposes of that object when the Unique_com_ptr goes out of scope.
template<typename T>
struct com_ptr final {

	// Commented because unittests use fake COM interface.
	//static_assert(std::is_base_of<IUnknown, T>::value, "T must be derived from IUnknown.");


	com_ptr() noexcept {}

	explicit com_ptr(T* ptr) noexcept 
		: ptr(ptr)
	{}

	com_ptr(nullptr_t) noexcept {}

	com_ptr(const com_ptr&) = delete;

	com_ptr(com_ptr&& com_ptr) noexcept 
		: ptr(com_ptr.ptr)
	{
		com_ptr.ptr = nullptr;
	}

	~com_ptr() noexcept
	{
		dispose();
	}


	com_ptr& operator=(const com_ptr&) = delete;

	com_ptr& operator=(com_ptr&& com_ptr) noexcept
	{
		if (this == &com_ptr) return *this;

		dispose();
		ptr = com_ptr.ptr;
		com_ptr.ptr = nullptr;
		return *this;
	}

	com_ptr& operator=(T* ptr) noexcept
	{
		dispose();
		this->ptr = ptr;
		return *this;
	}

	com_ptr& operator=(nullptr_t) noexcept
	{
		dispose();
		return *this;
	}

	T& operator*() const noexcept
	{
		return *ptr;
	}

	T* operator->() const noexcept
	{
		return ptr;
	}

	operator bool() const noexcept
	{
		return (ptr != nullptr);
	}

	operator T*() const noexcept
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
void com_ptr<T>::dispose() noexcept
{
	T* temp = ptr;
	if (temp == nullptr) return;

	ptr = nullptr;
	auto c = temp->Release();
}

template<typename T>
inline bool operator==(const com_ptr<T>& l, const com_ptr<T>& r) noexcept
{
	return l.ptr == r.ptr;
}

template<typename T>
inline bool operator==(const com_ptr<T>& com_ptr, nullptr_t) noexcept
{
	return com_ptr.ptr == nullptr;
}

template<typename T>
inline bool operator==(nullptr_t, const com_ptr<T>& com_ptr) noexcept
{
	return com_ptr.ptr == nullptr;
}

template<typename T>
inline bool operator!=(const com_ptr<T>& l, const com_ptr<T>& r) noexcept
{
	return l.ptr != r.ptr;
}

template<typename T>
inline bool operator!=(const com_ptr<T>& com_ptr, nullptr_t) noexcept
{
	return com_ptr.ptr != nullptr;
}

template<typename T>
inline bool operator!=(nullptr_t, const com_ptr<T>& com_ptr) noexcept
{
	return com_ptr.ptr != nullptr;
}

class dx11_rhi_context final : public virtual cg::rnd::rhi_context_i {
public:

	dx11_rhi_context(HWND hwnd, const uint2& viewport_size, cg::rnd::depth_stencil_format depth_stencil_format);

	dx11_rhi_context(const dx11_rhi_context&) = delete;

	dx11_rhi_context(dx11_rhi_context&&) = delete;

	~dx11_rhi_context() noexcept = default;


	dx11_rhi_context& operator=(const dx11_rhi_context&) = delete;

	dx11_rhi_context& operator=(dx11_rhi_context&&) = delete;


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

	cg::rnd::render_api render_api() const noexcept override
	{
		return cg::rnd::render_api::dx_11;
	}

	const D3D11_VIEWPORT& viewport() const noexcept
	{
		return _viewport;
	}

	float viewport_aspect_ratio() const noexcept
	{
		assert(!approx_equal(_viewport.Height, 0.0f));
		return _viewport.Width / _viewport.Height;
	}


	void bind_window_render_targets();

	void resize_viewport(const uint2& viewport_size) override;

	void swap_color_buffers() override;

private:

	void init_device(HWND hwnd, const uint2& viewport_size);

	void init_depth_stencil_buffer(const uint2& viewport_size,
		cg::rnd::depth_stencil_format depth_stencil_format);

	void update_depth_stencil_buffer(const uint2& viewport_size);

	void update_render_target_buffer();

	void update_viewport(const uint2& viewport_size);


	com_ptr<ID3D11Device> _device;
	com_ptr<ID3D11Debug> _debug;
	com_ptr<ID3D11DeviceContext> _device_ctx;
	com_ptr<IDXGISwapChain> _swap_chain;
	com_ptr<ID3D11RenderTargetView> _rtv_window;
	com_ptr<ID3D11Texture2D> _tex_depth_stencil;
	com_ptr<ID3D11DepthStencilView> _dsv_depth_stencil;
	D3D11_VIEWPORT _viewport;
};

struct hlsl_compute_desc final {
	// Hlsl source code.
	std::string source_code;

	// Hlsl source code filename. Use in error messages.
	std::string source_filename;

	// The name of a function where vertex shader execution begins.
	std::string compute_shader_entry_point = "cs_main";

	// The D3DCOMPILE constants specify how the compiler compiles the HLSL code.
	uint32_t compile_flags = 0;
};

// hlsl_shader_desc struct stores all required and optional params
// which are used in hlsl shader creation process.
struct hlsl_shader_desc final {
	// Hlsl source code.
	std::string source_code;

	// Hlsl source code filename. Use in error messages.
	std::string source_filename;

	// The name of a function where vertex shader execution begins.
	std::string vertex_shader_entry_point;

	// The name of a function where hull shader execution begins.
	std::string hull_shader_entry_point;

	// The name of a function where domain shader execution begins.
	std::string domain_shader_entry_point;

	// The name of a function where pixel shader execution begins.
	std::string pixel_shader_entry_point;

	// The D3DCOMPILE constants specify how the compiler compiles the HLSL code.
	uint32_t compile_flags = 0;
};

struct hlsl_shader final {

	hlsl_shader() noexcept = default;

	hlsl_shader(ID3D11Device* device, const hlsl_shader_desc& hlsl_desc);

	hlsl_shader(const hlsl_shader&) = delete;

	hlsl_shader(hlsl_shader&& set) noexcept;

	~hlsl_shader() noexcept = default;


	hlsl_shader& operator=(const hlsl_shader&) = delete;

	hlsl_shader& operator=(hlsl_shader&& set) noexcept;


	com_ptr<ID3D11VertexShader> vertex_shader;
	com_ptr<ID3DBlob> vertex_shader_bytecode;
	com_ptr<ID3D11HullShader> hull_shader;
	com_ptr<ID3DBlob> hull_shader_bytecode;
	com_ptr<ID3D11DomainShader> domain_shader;
	com_ptr<ID3DBlob> domain_shader_bytecode;
	com_ptr<ID3D11PixelShader> pixel_shader;
	com_ptr<ID3DBlob> pixel_shader_bytecode;

private:

	void init_vertex_shader(ID3D11Device* device, const hlsl_shader_desc& hlsl_data);

	void init_hull_shader(ID3D11Device* device, const hlsl_shader_desc& hlsl_data);

	void init_domain_shader(ID3D11Device* device, const hlsl_shader_desc& hlsl_data);

	void init_pixel_shader(ID3D11Device* device, const hlsl_shader_desc& hlsl_data);
};


// Creates an unitialized constant buffer object.
com_ptr<ID3D11Buffer> constant_buffer(ID3D11Device* device, size_t byte_count);

// Creates an unitialized constant buffer object.
template<typename T_cbuffer_data>
inline com_ptr<ID3D11Buffer> constant_buffer(ID3D11Device* device)
{
	return constant_buffer(device, sizeof(T_cbuffer_data));
}

// Returns true if vertex shader's entry point is specified.
inline bool has_vertex_shader(const hlsl_shader_desc& hlsl_desc) noexcept
{
	return !hlsl_desc.vertex_shader_entry_point.empty();
}

// Returns true if hull shader's entry point is specified.
inline bool has_hull_shader(const hlsl_shader_desc& hlsl_desc) noexcept
{
	return !hlsl_desc.hull_shader_entry_point.empty();
}

// Returns true if domain shader's entry point is specified.
inline bool has_domain_shader(const hlsl_shader_desc& hlsl_desc) noexcept
{
	return !hlsl_desc.domain_shader_entry_point.empty();
}

// Returns true if pixel shader's entry point is specified.
inline bool has_pixel_shader(const hlsl_shader_desc& hlsl_desc) noexcept
{
	return !hlsl_desc.pixel_shader_entry_point.empty();
}

hlsl_compute_desc load_hlsl_compute_desc(const char* filename);

inline hlsl_compute_desc load_hlsl_compute_desc(const std::string& filename)
{
	return load_hlsl_compute_desc(filename.c_str());
}

// Loads the specified hlsl shader source code file.
// If the specified file does not exist the function will throw. 
hlsl_shader_desc load_hlsl_shader_set_desc(const char* filename);

// Loads the specified hlsl shader source code file.
// If the specified file does not exist the function will throw. 
inline hlsl_shader_desc load_hlsl_shader_set_desc(const std::string& filename)
{
	return load_hlsl_shader_set_desc(filename.c_str());
}


} // namespace dx11
} // namespace rnd
} // namespace cg

#endif // CG_RND_DX11_DX11_H_

