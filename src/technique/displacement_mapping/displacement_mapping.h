#ifndef TECHNIQUE_DISPLACEMENT_MAPPING_DISPLACEMENT_MAPPING_H_
#define TECHNIQUE_DISPLACEMENT_MAPPING_DISPLACEMENT_MAPPING_H_

#include "cg/rnd/dx11/dx11.h"
#include "cg/sys/app.h"


namespace displacement_mapping {

class Displacement_mapping final : public virtual cg::sys::Example {
public:

	static constexpr cg::sys::Example_desc example_desc = {
		cg::rnd::Render_api::dx_11,
		cg::rnd::Depth_stencil_format::depth_24_stencil_8
	};


	Displacement_mapping(const cg::sys::App_context& app_ctx, cg::rnd::Rhi_context_i& rhi_ctx);

	Displacement_mapping(const Displacement_mapping&) = delete;

	Displacement_mapping(Displacement_mapping&&) = delete;


	Displacement_mapping& operator=(const Displacement_mapping&) = delete;

	Displacement_mapping& operator=(Displacement_mapping&&) = delete;


	void on_mouse_move() override;

	void on_window_resize() override;

	void render(float interpolation_factor) override;

	void update(float dt_msec) override;

private:

	void init_cbuffers();

	void init_geometry();

	void init_pipeline_state();

	void init_shaders();

	void init_textures();

	void setup_pipeline_state();

	void update_projection_matrix();

	// dx stuff
	cg::rnd::dx11::DX11_rhi_context& _rhi_ctx;
	ID3D11Device* _device = nullptr;
	ID3D11Debug* _debug = nullptr;
	ID3D11DeviceContext* _device_ctx = nullptr;
	// rendering
	cg::float2 _view_roll_angles;
	cg::float2 _prev_mouse_position;
	cg::Viewpoint _curr_viewpoint;
	cg::Viewpoint _prev_viewpoint;
	cg::mat4 _projection_matrix;
	cg::rnd::dx11::Com_ptr<ID3D11DepthStencilState> _depth_stencil_state;
	cg::rnd::dx11::Com_ptr<ID3D11RasterizerState> _rasterizer_state;
	cg::rnd::dx11::Com_ptr<ID3D11Buffer> _cb_matrix;
	cg::rnd::dx11::Hlsl_shader _pom_shader;
	// model
	cg::mat4 _model_matrix;
	cg::rnd::dx11::Com_ptr<ID3D11InputLayout> _pom_input_layout;
	cg::rnd::dx11::Com_ptr<ID3D11Buffer> _vertex_buffer;
	cg::rnd::dx11::Com_ptr<ID3D11Buffer> _index_buffer;
	cg::rnd::dx11::Com_ptr<ID3D11Texture2D> _tex_diffuse_rgb;
	cg::rnd::dx11::Com_ptr<ID3D11ShaderResourceView> _tex_srv_diffuse_rgb;
	cg::rnd::dx11::Com_ptr<ID3D11Texture2D> _tex_displacement_map;
	cg::rnd::dx11::Com_ptr<ID3D11Texture2D> _tex_normal_map;
	UINT _vertex_stride;
	UINT _index_count;
};

} // displacement_mapping

#endif // TECHNIQUE_DISPLACEMENT_MAPPING_DISPLACEMENT_MAPPING_H_
