#ifndef TECHNIQUE_PARALLAX_OCCLUSION_MAPPING_PARALLAX_OCCLUSION_MAPPING_H_
#define TECHNIQUE_PARALLAX_OCCLUSION_MAPPING_PARALLAX_OCCLUSION_MAPPING_H_

#include "cg/rnd/dx11/dx11.h"
#include "cg/sys/app.h"


namespace parallax_occlusion_mapping {

struct Material final {

	Material() noexcept = default;

	Material(ID3D11Device* device, float tex_coord_step_scale, float min_sample_count,
		float max_sample_count, float self_shadowing_factor, const char* diffuse_rgb_filename,
		const char* height_map_filename, const char* normal_map_height);

	Material(const Material&) = delete;
	Material(Material&& m) noexcept = default;

	Material& operator=(const Material&) = delete;
	Material& operator=(Material&& m) noexcept = default;


	float tex_coord_step_scale = 1.0f;
	float min_sample_count = 0.0f;
	float max_sample_count = 0.0f;
	float self_shadowing_factor = 1.0f;
	cg::rnd::dx11::Com_ptr<ID3D11Texture2D> tex_diffuse_rgb;
	cg::rnd::dx11::Com_ptr<ID3D11Texture2D> tex_height_map;
	cg::rnd::dx11::Com_ptr<ID3D11Texture2D> tex_normal_map;
	cg::rnd::dx11::Com_ptr<ID3D11ShaderResourceView> tex_srv_diffuse_rgb;
	cg::rnd::dx11::Com_ptr<ID3D11ShaderResourceView> tex_srv_height_map;
	cg::rnd::dx11::Com_ptr<ID3D11ShaderResourceView> tex_srv_normal_map;
};

class Parallax_occlusion_mapping final : public virtual cg::sys::Example {
public:

	static constexpr cg::sys::Example_desc example_desc = {
		cg::rnd::Render_api::dx_11,
		cg::rnd::Depth_stencil_format::depth_24_stencil_8
	};


	Parallax_occlusion_mapping(const cg::sys::App_context& app_ctx, cg::rnd::Rhi_context_i& rhi_ctx);

	Parallax_occlusion_mapping(const Parallax_occlusion_mapping&) = delete;

	Parallax_occlusion_mapping(Parallax_occlusion_mapping&&) = delete;


	Parallax_occlusion_mapping& operator=(const Parallax_occlusion_mapping&) = delete;

	Parallax_occlusion_mapping& operator=(Parallax_occlusion_mapping&&) = delete;


	void on_mouse_move() override;

	void on_window_resize() override;

	void render(float interpolation_factor) override;

	void update(float dt_msec) override;

private:

	static constexpr size_t cb_vertex_shader_component_count = 3 * 16 + 3 * 4;
	static constexpr size_t cb_pixel_shader_component_count = 4;


	void int_cbuffers();

	void init_geometry();

	void init_pipeline_state();

	void init_shaders();

	void init_materials();

	void update_cb_vertex_shader(const cg::Viewpoint& viewpoint);

	void update_cb_pixel_shader();

	void setup_pipeline_state();

	void update_projection_matrix();


	// dx stuff
	cg::rnd::dx11::DX11_rhi_context& _rhi_ctx;
	ID3D11Device* _device = nullptr;
	ID3D11Debug* _debug = nullptr;
	ID3D11DeviceContext* _device_ctx = nullptr;
	// rendering
	float2 _view_roll_angles;
	float2 _prev_mouse_position;
	cg::Viewpoint _curr_viewpoint;
	cg::Viewpoint _prev_viewpoint;
	float4x4 _projection_matrix;
	cg::rnd::dx11::Com_ptr<ID3D11DepthStencilState> _depth_stencil_state;
	cg::rnd::dx11::Com_ptr<ID3D11RasterizerState> _rasterizer_state;
	cg::rnd::dx11::Com_ptr<ID3D11Buffer> _cb_vertex_shader;
	cg::rnd::dx11::Com_ptr<ID3D11Buffer> _cb_pixel_shader;
	cg::rnd::dx11::Com_ptr<ID3D11SamplerState> _sampler_state;
	cg::rnd::dx11::Hlsl_shader _pom_shader;
	// light
	float3 _dlight_position_ws;
	float3 _dlight_velocity_ws;
	// model
	float3 _model_scale;
	float3 _model_position;
	cg::rnd::dx11::Com_ptr<ID3D11InputLayout> _pom_input_layout;
	cg::rnd::dx11::Com_ptr<ID3D11Buffer> _vertex_buffer;
	cg::rnd::dx11::Com_ptr<ID3D11Buffer> _index_buffer;
	Material* _curr_material = nullptr;
	Material _rock_wall_material;
	Material _four_shapes_material;
	UINT _vertex_stride;
	UINT _index_count;
};

} // parallax_occlusion_mapping

#endif // TECHNIQUE_PARALLAX_OCCLUSION_MAPPING_PARALLAX_OCCLUSION_MAPPING_H_
