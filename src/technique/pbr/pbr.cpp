#include "technique/pbr/pbr.h"



namespace pbr {

pbr::pbr(const cg::sys::app_context& app_ctx, cg::rnd::rhi_context_i& rhi_ctx)
	: example(app_ctx, rhi_ctx),
	rhi_ctx_(dynamic_cast<cg::rnd::dx11::dx11_rhi_context&>(rhi_ctx)),
	device_(rhi_ctx_.device()),
	debug_(rhi_ctx_.debug()),
	device_ctx_(rhi_ctx_.device_ctx())
{
}

void pbr::on_mouse_move()
{}

void pbr::on_window_resize()
{}

void pbr::render(float interpolation_factor)
{}

void pbr::update(float dt_msec)
{}


} // namespace pbr
