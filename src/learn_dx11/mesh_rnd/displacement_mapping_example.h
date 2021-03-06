#ifndef LEARN_DX11_DISPLACEMENT_MAPPING_EXAMPLE_H_
#define LEARN_DX11_DISPLACEMENT_MAPPING_EXAMPLE_H_

#include "cg/base/math.h"
#include "learn_dx11/base/app.h"


namespace learn_dx11 {
namespace mesh_rnd {

class Displacement_mapping_example final : public Example {
public:

	static constexpr bool init_depth_stencil_view = true;


	Displacement_mapping_example(Render_context& rnd_ctx);

	Displacement_mapping_example(const Displacement_mapping_example&) = delete;

	Displacement_mapping_example(Displacement_mapping_example&&) noexcept = delete;


	Displacement_mapping_example& operator=(const Displacement_mapping_example&) = delete;

	Displacement_mapping_example& operator=(Displacement_mapping_example&&) noexcept = delete;


	void on_viewport_resize(const uint2& viewport_size) override;

	void render() override;

	void update(float dt_milliseconds) override {}

private:

	void init_cbuffers();

	void init_render_states();

	void init_shaders();

	void init_terrain_textures();

	void setup_pipeline_state();

	void setup_pvm_matrix();

	void update_projection_matrix(float wh_aspect_ratio);

	// renderer settings
	com_ptr<ID3D11DepthStencilState> _depth_stencil_state;
	com_ptr<ID3D11RasterizerState> _default_rasterizer_state;
	com_ptr<ID3D11RasterizerState> _wireframe_rasterizer_state;

	Hlsl_shader_set _shader_set;
	com_ptr<ID3D11Buffer> _model_cbuffer;
	com_ptr<ID3D11Buffer> _projection_view_cbuffer;
	com_ptr<ID3D11Texture2D> _tex_terrain_displacement_map;
	com_ptr<ID3D11ShaderResourceView> _tex_terrain_displacement_map_srv;
	com_ptr<ID3D11Texture2D> _tex_terrain_normal_map;
	com_ptr<ID3D11ShaderResourceView> _tex_terrain_normal_map_srv;
	com_ptr<ID3D11SamplerState> _linear_sampler;

	float4x4 _projection_matrix;
	float4x4 _view_matrix;
	float4x4 _model_matrix;
};

} // namespace mesh_rnd
} // namespace learn_dx11

#endif // LEARN_DX11_DISPLACEMENT_MAPPING_EXAMPLE_H_

