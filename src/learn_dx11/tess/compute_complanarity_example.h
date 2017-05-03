#ifndef LEARN_DX11_COMPUTE_PATCH_COMPLANARITY_H_
#define LEARN_DX11_COMPUTE_PATCH_COMPLANARITY_H_

#include "learn_dx11/base/app.h"


namespace learn_dx11 {
namespace tess {

class Compute_complanarity_example final : public Example {
public:

	static constexpr bool init_depth_stencil_view = true;


	Compute_complanarity_example(Render_context& rnd_ctx);

	Compute_complanarity_example(const Compute_complanarity_example&) = delete;

	Compute_complanarity_example(Compute_complanarity_example&&) = delete;


	Compute_complanarity_example& operator=(const Compute_complanarity_example&) = delete;

	Compute_complanarity_example& operator=(Compute_complanarity_example&&) = delete;


	void on_keypress() override {}

	void on_viewport_resize(const uint2& viewport_size) override;

	void render() override;

	void update(float dt_milliseconds) override {}

private:

	void init_cbuffers();

	void init_geometry();

	void init_pipeline_state();

	void init_shaders();

	void init_textures();

	void preprocess_displacement_map();

	void setup_pipeline_state();

	void setup_pvm_matrix();

	void update_projection_matrix(float aspect_ratio);


	const size_t _terrain_z_cell_count;
	const size_t _terrain_x_cell_count;
	float3 _viewpoint_position;
	float4x4 _projection_matrix;
	float4x4 _view_matrix;
	float4x4 _model_matrix;
	com_ptr<ID3D11Buffer> _pvm_matrix_cbuffer;
	com_ptr<ID3D11Buffer> _tess_control_cbuffer;
	Hlsl_shader_set _shader_set;
	com_ptr<ID3D11DepthStencilState> _depth_stencil_state;
	com_ptr<ID3D11RasterizerState> _default_rasterizer_state;
	com_ptr<ID3D11RasterizerState> _wireframe_rasterizer_state;
	com_ptr<ID3D11Buffer> _vertex_buffer;
	com_ptr<ID3D11Buffer> _index_buffer;
	com_ptr<ID3D11InputLayout> _input_layout;
	com_ptr<ID3D11Texture2D> _tex_displacement_map;
	com_ptr<ID3D11ShaderResourceView> _tex_srv_displacement_map;
	com_ptr<ID3D11Texture2D> _tex_lookup;
	com_ptr<ID3D11UnorderedAccessView> _tex_uav_lookup;
	com_ptr<ID3D11ShaderResourceView> _tex_srv_lookup;
	com_ptr<ID3D11SamplerState> _linear_sampler;
	UINT _index_count;
};

} // namsespace tess
} // namespace learn_dx11

#endif // LEARN_DX11_COMPUTE_PATCH_COMPLANARITY_H_
