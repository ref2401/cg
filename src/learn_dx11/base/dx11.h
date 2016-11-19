#ifndef LEARN_DX11_BASE_DX11_H_
#define LEARN_DX11_BASE_DX11_H_

#include <cassert>
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

using cg::rnd::utility::Com_ptr;
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

	void init_pixel_shader(ID3D11Device* device, const cg::data::Hlsl_shader_set_data& hlsl_data);

	Com_ptr<ID3D11VertexShader> _vertex_shader;
	Com_ptr<ID3DBlob> _vertex_shader_bytecode;
	Com_ptr<ID3D11PixelShader> _pixel_shader;
	Com_ptr<ID3DBlob> _pixel_shader_bytecode;
};

class Pipeline_state final {
public:

	Pipeline_state() noexcept = default;

	Pipeline_state(ID3D11Device* device, IDXGISwapChain* swap_chain, 
		const cg::uint2& viewport_size);

	Pipeline_state(const Pipeline_state&) = delete;

	Pipeline_state(Pipeline_state&& state) noexcept;

	~Pipeline_state() noexcept = default;


	Pipeline_state& operator=(const Pipeline_state&) = delete;

	Pipeline_state& operator=(Pipeline_state&& state) noexcept;


	void clear_render_target_view();

	ID3D11DepthStencilState* depth_stencil_state() noexcept
	{
		return _depth_stencil_state.ptr;
	}

	ID3D11RasterizerState* rasterizer_state() noexcept
	{
		return _rasterizer_state.ptr;
	}

	ID3D11RenderTargetView* render_target_view() noexcept
	{
		return _render_target_view.ptr;
	}

	void update_depth_stencil_state(ID3D11Device* device);

	void update_render_target_view(ID3D11Device* device, IDXGISwapChain* swap_chain);

	void update_viewport(const cg::uint2& viewport_size);

	const D3D11_VIEWPORT& viewport_desc() const noexcept
	{
		return _viewport_desc;
	}

	const cg::uint2& viewport_size() const noexcept
	{
		return _viewport_size;
	}

private:

	void init_rasterizer_state(ID3D11Device* device);

	Com_ptr<ID3D11DepthStencilState> _depth_stencil_state;
	Com_ptr<ID3D11RasterizerState> _rasterizer_state;
	Com_ptr<ID3D11RenderTargetView> _render_target_view;
	cg::uint2 _viewport_size;
	D3D11_VIEWPORT _viewport_desc;
};

// The Render_context class provides access to all the DirectX's essential objects
// which are required by every example from this project.
class Render_context final {
public:

	Render_context::Render_context(HWND hwnd, const cg::uint2& window_size);

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

	Pipeline_state& pipeline_state() noexcept
	{
		return _pipeline_state;
	}

	void resize_viewport(const cg::uint2& viewport_size);

	void swap_color_buffers();

private:

	void init_device(HWND hwnd, const cg::uint2& window_size);

	void setup_pipeline_state();

	Com_ptr<ID3D11Device> _device;
	Com_ptr<ID3D11Debug> _debug;
	Com_ptr<ID3D11DeviceContext> _device_ctx;
	Com_ptr<IDXGISwapChain> _swap_chain;
	Pipeline_state _pipeline_state;
};


// Creates an unitialized constant buffer object.
Com_ptr<ID3D11Buffer> make_cbuffer(ID3D11Device* device, size_t byte_count) noexcept;

// Creates an unitialized constant buffer object.
template<typename T_cbuffer_data>
inline Com_ptr<ID3D11Buffer> make_cbuffer(ID3D11Device* device) noexcept
{
	return make_cbuffer(device, sizeof(T_cbuffer_data));
}


} // namespace learn_dx11

#endif // LEARN_DX11_BASE_DX11_H_
