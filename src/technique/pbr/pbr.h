#ifndef TECHNIQUE_PBR_PBR_H_
#define TECHNIQUE_PBR_PBR_H_

#include "cg/rnd/dx11/dx11.h"
#include "cg/sys/app.h"

using namespace cg::rnd::dx11;


namespace pbr {

class cube_envmap_pass final {
public:

	static constexpr size_t reflection_mip_level_count = 5;


	cube_envmap_pass(ID3D11Device* device, ID3D11DeviceContext* device_ctx, ID3D11Debug* debug_,
		const char* envmap_filename, size_t cube_side_size, size_t irradiance_side_size,
		size_t reflection_size_size);

	cube_envmap_pass(cube_envmap_pass&&) = delete;
	cube_envmap_pass& operator=(cube_envmap_pass&&) = delete;


	ID3D11ShaderResourceView* tex_brdf_map_srv() const noexcept
	{
		return tex_brdf_map_srv_;
	}

	ID3D11ShaderResourceView* tex_cube_envmap_srv() const noexcept
	{
		return tex_cube_envmap_srv_;
	}

	ID3D11ShaderResourceView* tex_irradiance_map_srv() const noexcept
	{
		return tex_irradiance_map_srv_;
	}

	ID3D11ShaderResourceView* tex_reflection_map_srv() const noexcept
	{
		return tex_reflection_map_srv_;
	}

	void render_envmap(const float4x4& projection_view_matrix);

private:

	static constexpr UINT cube_index_count = 14;


	void init_brdf_maps();

	// Loads an epirectengular hdr image and projects in onto tex_cube_envmap_.
	// Convolutes tex_cube_envmap_ and stores the result in tex_irradiance_map_
	void init_cube_maps(const char* envmap_filename, size_t cube_side_size, size_t irradiance_side_size,
		size_t reflection_size_size);

	void init_pipeline_state();


	ID3D11Device* device_;
	ID3D11DeviceContext* device_ctx_;
	ID3D11Debug* debug_;
	// cube envmap data:
	//
	com_ptr<ID3D11Texture2D> tex_cube_envmap_;
	com_ptr<ID3D11ShaderResourceView> tex_cube_envmap_srv_;
	com_ptr<ID3D11Texture2D> tex_irradiance_map_;
	com_ptr<ID3D11ShaderResourceView> tex_irradiance_map_srv_;
	com_ptr<ID3D11Texture2D> tex_reflection_map_;
	com_ptr<ID3D11ShaderResourceView> tex_reflection_map_srv_;
	com_ptr<ID3D11Texture2D> tex_brdf_map_;
	com_ptr<ID3D11ShaderResourceView> tex_brdf_map_srv_;
	// rendering:
	//
	com_ptr<ID3D11Buffer> constant_buffer_;
	hlsl_shader shader_;
	com_ptr<ID3D11RasterizerState> rasterizer_state_;
	com_ptr<ID3D11DepthStencilState> depth_stencil_state_;
	com_ptr<ID3D11SamplerState> sampler_state_;
};

class pbr final : public virtual cg::sys::example {
public:

	static constexpr cg::sys::example_desc example_desc = {
		cg::rnd::render_api::dx_11,
		cg::rnd::depth_stencil_format::depth_24_stencil_8
	};


	pbr(const cg::sys::app_context& app_ctx, cg::rnd::rhi_context_i& rhi_ctx);

	pbr(pbr&&) = delete;

	pbr& operator=(pbr&&) = delete;



	void on_mouse_move() override;

	void on_window_resize() override;

	void render(float interpolation_factor) override;

	void update(float dt_msec) override;

private:

	static constexpr size_t cb_vertex_shader_component_count = 4 * 16 + 2 * 4;

	void init_geometry();

	void init_pipeline_state();

	void init_shader();

	void update_cb_vertex_shader(const cg::Viewpoint& viewpoint);

	void update_projection_matrix();


	dx11_rhi_context& rhi_ctx_;
	ID3D11Device* device_ = nullptr;
	ID3D11Debug* debug_ = nullptr;
	ID3D11DeviceContext* device_ctx_ = nullptr;
	// rendering
	float2 view_roll_angles_;
	float2 prev_mouse_position_;
	cg::Viewpoint curr_viewpoint_;
	cg::Viewpoint prev_viewpoint_;
	float4x4 projection_matrix_;
	com_ptr<ID3D11DepthStencilState> depth_stencil_state_;
	com_ptr<ID3D11RasterizerState> rasterizer_state_;
	com_ptr<ID3D11Buffer> cb_vertex_shader_;
	com_ptr<ID3D11Buffer> cb_pixel_shader_;
	hlsl_shader shader_;
	com_ptr<ID3D11SamplerState> sampler_state_;
	// model
	float3 model_position_;
	quat model_rotation_;
	float3 model_scale_;
	com_ptr<ID3D11InputLayout> input_layout_;
	com_ptr<ID3D11Buffer> vertex_buffer_;
	com_ptr<ID3D11Buffer> index_buffer_;
	UINT vertex_stride_;
	UINT index_count_;
	// cube model
	cube_envmap_pass cube_envmap_pass_;
	// debug_
	com_ptr<ID3D11Texture2D> tex_debug_;
	com_ptr<ID3D11RenderTargetView> tex_debug_rtv_;
};

} // namespace pbr

#endif // TECHNIQUE_PBR_PBR_H_
