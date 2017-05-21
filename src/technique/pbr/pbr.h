#ifndef TECHNIQUE_PBR_PBR_H_
#define TECHNIQUE_PBR_PBR_H_

#include "cg/rnd/dx11/dx11.h"
#include "cg/sys/app.h"

using namespace cg::rnd::dx11;


namespace pbr {

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

	static constexpr size_t cb_vertex_shader_component_count = 3 * 16 + 2 * 4;

	void init_cube_envmap_data();

	void init_geometry();

	void init_irradiance_map();

	void init_pipeline_state();

	void init_shader();

	void setup_pipeline_state();

	void update_cb_vertex_shader(const cg::Viewpoint& viewpoint);

	void update_projection_matrix();


	dx11_rhi_context& rhi_ctx;
	ID3D11Device* device = nullptr;
	ID3D11Debug* debug = nullptr;
	ID3D11DeviceContext* device_ctx = nullptr;
	// rendering
	float2 view_roll_angles;
	float2 prev_mouse_position;
	cg::Viewpoint curr_viewpoint;
	cg::Viewpoint prev_viewpoint;
	float4x4 projection_matrix;
	com_ptr<ID3D11DepthStencilState> depth_stencil_state;
	com_ptr<ID3D11RasterizerState> rasterizer_state;
	com_ptr<ID3D11Buffer> cb_vertex_shader;
	com_ptr<ID3D11Buffer> cb_pixel_shader;
	hlsl_shader shader;
	com_ptr<ID3D11SamplerState> sampler_state;
	// model
	float3 model_position;
	quat model_rotation;
	float3 model_scale;
	com_ptr<ID3D11InputLayout> input_layout;
	com_ptr<ID3D11Buffer> vertex_buffer;
	com_ptr<ID3D11Buffer> index_buffer;
	UINT vertex_stride;
	UINT index_count;
	// cube model
	com_ptr<ID3D11Buffer> cb_buffer_cube_envmap;
	hlsl_shader cube_envmap_shader;
	com_ptr<ID3D11RasterizerState> cube_envmap_rasterizer_state;
	com_ptr<ID3D11InputLayout> cube_input_layout;
	com_ptr<ID3D11Buffer> cube_vertex_buffer;
	com_ptr<ID3D11Buffer> cube_index_buffer;
	UINT cube_index_count;
	UINT cube_vertex_stride;

	// environment map -> irradiance map
	com_ptr<ID3D11Texture2D> tex_cube_envmap;
	com_ptr<ID3D11ShaderResourceView> tex_cube_envmap_srv;
	// debug
	com_ptr<ID3D11Texture2D> tex_debug;
	com_ptr<ID3D11RenderTargetView> tex_debug_rtv;
};

} // namespace pbr

#endif // TECHNIQUE_PBR_PBR_H_
