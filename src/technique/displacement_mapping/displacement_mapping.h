#ifndef TECHNIQUE_DISPLACEMENT_MAPPING_DISPLACEMENT_MAPPING_H_
#define TECHNIQUE_DISPLACEMENT_MAPPING_DISPLACEMENT_MAPPING_H_

#include "cg/rnd/dx11/dx11.h"
#include "cg/sys/app.h"


namespace displacement_mapping {

class Displacement_mapping final : public virtual cg::sys::Example {
public:

	Displacement_mapping(const cg::sys::App_context& app_ctx, cg::rnd::Rhi_context_i& rhi_ctx);

	Displacement_mapping(const Displacement_mapping&) = delete;

	Displacement_mapping(Displacement_mapping&&) = delete;


	Displacement_mapping& operator=(const Displacement_mapping&) = delete;

	Displacement_mapping& operator=(Displacement_mapping&&) = delete;


	void on_window_resize() override;

	void render(float interpolation_factor) override;

	void update(float dt_msec) override;

private:

	cg::rnd::dx11::DX11_rhi_context& _rhi_ctx;
	ID3D11Device* _device = nullptr;
	ID3D11Debug* _debug = nullptr;
	ID3D11DeviceContext* _device_ctx = nullptr;
};

} // displacement_mapping

#endif // TECHNIQUE_DISPLACEMENT_MAPPING_DISPLACEMENT_MAPPING_H_
