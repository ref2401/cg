#ifndef LEARN_DX11_BASE_DX11_H_
#define LEARN_DX11_BASE_DX11_H_

#include <cassert>
#include <type_traits>
#include "cg/data/shader.h"
#include "cg/math/math.h"
#include "cg/rnd/dx11/dx11.h"
#include <windows.h>
#include <d3d11.h>
#include <d3dcommon.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <dxgi.h>

using cg::rnd::dx11::Com_ptr;
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

	void init_vertex_shader(ID3D11Device* device, const cg::data::Hlsl_shader_set_data& hlsl_data);

	void init_hull_shader(ID3D11Device* device, const cg::data::Hlsl_shader_set_data& hlsl_data);

	void init_domain_shader(ID3D11Device* device, const cg::data::Hlsl_shader_set_data& hlsl_data);

	void init_pixel_shader(ID3D11Device* device, const cg::data::Hlsl_shader_set_data& hlsl_data);

	Com_ptr<ID3D11VertexShader> _vertex_shader;
	Com_ptr<ID3DBlob> _vertex_shader_bytecode;
	Com_ptr<ID3D11HullShader> _hull_shader;
	Com_ptr<ID3DBlob> _hull_shader_bytecode;
	Com_ptr<ID3D11DomainShader> _domain_shader;
	Com_ptr<ID3DBlob> _domain_shader_bytecode;
	Com_ptr<ID3D11PixelShader> _pixel_shader;
	Com_ptr<ID3DBlob> _pixel_shader_bytecode;
};

class Render_context final {
public:

	Render_context(HWND hwnd, const cg::uint2& window_size, bool init_depth_stencil_view);

	Render_context(const Render_context&) = delete;

	Render_context(Render_context&&) noexcept = delete;


	Render_context& operator=(const Render_context&) = delete;

	Render_context& operator=(Render_context&&) noexcept = delete;


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

	ID3D11DepthStencilView* depth_stencil_view() noexcept
	{
		return _depth_stencil_view.ptr;
	}

	ID3D11RenderTargetView* render_target_view() noexcept
	{
		return _render_target_view.ptr;
	}

	// Binds default render target ((owned by the window)), depth/stencil targets (if any)
	// and sets up the default viewport.
	void bind_default_render_targets();

	void resize_viewport(const cg::uint2& viewport_size);

	cg::uint2 viewport_size() const noexcept
	{
		return _viewport_size;
	}

	void swap_color_buffers();

private:

	void init_device(HWND hwnd, const cg::uint2& window_size);

	void update_depth_stencil_view(bool force_refresh = false);

	void update_render_target_view();

	cg::uint2 _viewport_size;
	Com_ptr<ID3D11Device> _device;
	Com_ptr<ID3D11Debug> _debug;
	Com_ptr<ID3D11DeviceContext> _device_ctx;
	Com_ptr<IDXGISwapChain> _swap_chain;
	Com_ptr<ID3D11RenderTargetView> _render_target_view;
	Com_ptr<ID3D11Texture2D> _tex_depth_stencil;
	Com_ptr<ID3D11DepthStencilView> _depth_stencil_view;
};


// Creates an unitialized constant buffer object.
Com_ptr<ID3D11Buffer> make_cbuffer(ID3D11Device* device, size_t byte_count);

// Creates an unitialized constant buffer object.
template<typename T_cbuffer_data>
inline Com_ptr<ID3D11Buffer> make_cbuffer(ID3D11Device* device)
{
	return make_cbuffer(device, sizeof(T_cbuffer_data));
}

// Creates a new 2D texture which has the same configuration as tex_origin but differs in size.
// The contents of the texture is uninitialized.
Com_ptr<ID3D11Texture2D> make_texture_2d(ID3D11Device* device,
	ID3D11Texture2D* tex_origin, const cg::uint2& size);


} // namespace learn_dx11

#endif // LEARN_DX11_BASE_DX11_H_
