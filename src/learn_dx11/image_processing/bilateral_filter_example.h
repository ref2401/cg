#ifndef LEARN_DX11_IMAGE_PROCESSING_BILATERAL_FILTER_H_
#define LEARN_DX11_IMAGE_PROCESSING_BILATERAL_FILTER_H_

#include "cg/base/math.h"
#include "learn_dx11/base/app.h"


namespace learn_dx11 {
namespace image_processing {

class Bilateral_filter_example final : public Example {
public:

	constexpr static bool init_depth_stencil_view = false;


	Bilateral_filter_example(Render_context& rnd_ctx);

	Bilateral_filter_example(const Bilateral_filter_example&) = delete;

	Bilateral_filter_example(Bilateral_filter_example&&) = delete;


	Bilateral_filter_example& operator=(const Bilateral_filter_example&) = delete;

	Bilateral_filter_example& operator=(Bilateral_filter_example&&) = delete;


	void on_keypress() override {}

	void on_viewport_resize(const uint2& viewport_size) override;

	void render() override;

	void update(float dt_milliseconds) override {}

private:

	void init_pipeline_state();

	void init_shaders();

	void init_textures();

	void perform_filtering();

	void setup_pipeline_state();

	void update_matrix_cbuffer(const float4x4& matrix);

	void update_pvm_matrix(float aspect_ratio);


	// pipeline state
	com_ptr<ID3D11DepthStencilState> _depth_stencil_state;
	com_ptr<ID3D11RasterizerState> _rasterizer_state;
	// example stuff
	com_ptr<ID3D11Buffer> _matrix_cbuffer;
	com_ptr<ID3D11SamplerState> _sampler_state;
	com_ptr<ID3D11Texture2D> _tex_source;
	com_ptr<ID3D11ShaderResourceView> _tex_srv_source;
	com_ptr<ID3D11Texture2D> _tex_intermediate;
	com_ptr<ID3D11ShaderResourceView> _tex_srv_intermediate;
	com_ptr<ID3D11UnorderedAccessView> _tex_uav_intermediate;
	com_ptr<ID3D11Texture2D> _tex_final;
	com_ptr<ID3D11ShaderResourceView> _tex_srv_final;
	com_ptr<ID3D11UnorderedAccessView> _tex_uav_final;
	Hlsl_compute _gaussian_filter_compute;
	Hlsl_shader_set _render_image_shader;

	float4x4 _pvm_matrix_0;
	float4x4 _pvm_matrix_1;
	float4x4 _model_matrix_0;
	float4x4 _model_matrix_1;
};

} // namespace image_processing
} // namespace learn_dx11

#endif // LEARN_DX11_IMAGE_PROCESSING_BILATERAL_FILTER_H_
