#include "technique/displacement_mapping/displacement_mapping.h"



namespace displacement_mapping {

// ----- Displacement_mapping -----

Displacement_mapping::Displacement_mapping(const cg::sys::App_context& app_ctx, cg::rnd::Rhi_context_i& rhi_ctx)
	: Example(app_ctx, rhi_ctx),
	_rhi_ctx(dynamic_cast<cg::rnd::dx11::DX11_rhi_context&>(rhi_ctx)),
	_device(_rhi_ctx.device()),
	_debug(_rhi_ctx.debug()),
	_device_ctx(_rhi_ctx.device_ctx())
{

}

void Displacement_mapping::on_window_resize()
{
}

void Displacement_mapping::render(float interpolation_factor)
{
}

void Displacement_mapping::update(float dt_msec) 
{
}

} // displacement_mapping
