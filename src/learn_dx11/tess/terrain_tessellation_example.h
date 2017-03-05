#ifndef LEARN_DX11_TESS_TERRAIN_TESSELLATION_EXAMPLE_H_
#define LEARN_DX11_TESS_TERRAIN_TESSELLATION_EXAMPLE_H_

#include "cg/math/math.h"
#include "learn_dx11/base/app.h"


namespace learn_dx11 {
namespace tess {

class Terrain_tessellation_example final : public Example {
public:

	constexpr static bool init_depth_stencil_view = true;


	Terrain_tessellation_example(Render_context& rnd_ctx);

	Terrain_tessellation_example(const Terrain_tessellation_example&) = delete;

	Terrain_tessellation_example(Terrain_tessellation_example&&) noexcept = delete;


	Terrain_tessellation_example& operator=(const Terrain_tessellation_example&) = delete;

	Terrain_tessellation_example& operator=(Terrain_tessellation_example&&) noexcept = delete;


	void on_keypress() override {}

	void on_viewport_resize(const cg::uint2& viewport_size) override;

	void render() override;

	void update(float dt_milliseconds) override {}

private:

	void init_cbuffers();

	void init_geometry();

	void init_pipeline_state();

	void init_shaders();

	void setup_pipeline_state();

	void setup_pvm_matrix();

	void update_projection_matrix(float aspect_ratio);


	cg::float3 _viewpoint_position;
	cg::mat4 _projection_matrix;
	cg::mat4 _view_matrix;
	cg::mat4 _model_matrix;

	// pipeline state
	Hlsl_shader_set _shader_set;
	Com_ptr<ID3D11DepthStencilState> _depth_stencil_state;
	Com_ptr<ID3D11RasterizerState> _default_rasterizer_state;
	Com_ptr<ID3D11RasterizerState> _wireframe_rasterizer_state;
	// resources
	Com_ptr<ID3D11Buffer> _vertex_buffer;
	Com_ptr<ID3D11Buffer> _index_buffer;
	Com_ptr<ID3D11InputLayout> _input_layout;
	Com_ptr<ID3D11Buffer> _pvm_matrix_cbuffer;
	Com_ptr<ID3D11Buffer> _tess_control_cbuffer;
	UINT _index_count;
};

} // namespace tess
} // namespace learn_dx11

#endif // LEARN_DX11_TESS_TERRAIN_TESSELLATION_EXAMPLE_H_
