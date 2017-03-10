#ifndef LEARN_DX11_IMAGE_PROCESSNG_GAUSSIAN_FILTER_EXAMPLE_H_
#define LEARN_DX11_IMAGE_PROCESSNG_GAUSSIAN_FILTER_EXAMPLE_H_


#include "cg/math/math.h"
#include "learn_dx11/base/app.h"

namespace learn_dx11 {
namespace image_processing {

class Gaussian_filter_example final : public Example {
public:

	constexpr static bool init_depth_stencil_view = true;


	Gaussian_filter_example(Render_context& rnd_ctx);

	Gaussian_filter_example(const Gaussian_filter_example&) = delete;

	Gaussian_filter_example(Gaussian_filter_example&&) = delete;


	Gaussian_filter_example& operator=(const Gaussian_filter_example&) = delete;

	Gaussian_filter_example& operator=(Gaussian_filter_example&&) = delete;


	void on_keypress() override {}

	void on_viewport_resize(const cg::uint2 &) override {}

	void render() override;

	void update(float dt_milliseconds) override {}

private:

	void init_shaders();

	void init_textures();

	Com_ptr<ID3D11Texture2D> _tex_source;
	Com_ptr<ID3D11Texture2D> _tex_intermidiate;
	Com_ptr<ID3D11Texture2D> _tex_final;

	Hlsl_compute _gaussian_filter_compute;
	Hlsl_shader_set _render_image_shader;

};

} // namespace image_processing
} // namespace learn_dx11

#endif // LEARN_DX11_IMAGE_PROCESSNG_GAUSSIAN_FILTER_EXAMPLE_H_