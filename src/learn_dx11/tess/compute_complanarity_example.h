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

	void on_viewport_resize(const cg::uint2& viewport_size) override;

	void render() override;

	void update(float dt_milliseconds) override {}

private:

	void init_pipeline_state();

	void setup_pipeline_state();

	void setup_pvm_matrix();

	void update_projection_matrix(float aspect_ratio);


	cg::float3 _viewpoint_position;
	cg::mat4 _projection_matrix;
	cg::mat4 _view_matrix;
	cg::mat4 _model_matrix;
	Com_ptr<ID3D11Buffer> _pvm_cbuffer;
	Com_ptr<ID3D11DepthStencilState> _depth_stencil_state;
	Com_ptr<ID3D11RasterizerState> _default_rasterizer_state;
	Com_ptr<ID3D11RasterizerState> _wireframe_rasterizer_state;

};

} // namsespace tess
} // namespace learn_dx11

#endif // LEARN_DX11_COMPUTE_PATCH_COMPLANARITY_H_
