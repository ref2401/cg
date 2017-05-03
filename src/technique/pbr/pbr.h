#ifndef TECHNIQUE_PBR_PBR_H_
#define TECHNIQUE_PBR_PBR_H_

#include "cg/rnd/dx11/dx11.h"
#include "cg/sys/app.h"


namespace pbr {

class pbr final : public virtual cg::sys::example{
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

	cg::rnd::dx11::dx11_rhi_context& rhi_ctx_;
	ID3D11Device* device_ = nullptr;
	ID3D11Debug* debug_ = nullptr;
	ID3D11DeviceContext* device_ctx_ = nullptr;
};

} // namespace pbr

#endif // TECHNIQUE_PBR_PBR_H_
