#ifndef TECHNIQUE_DISPLACEMENT_MAPPING_DISPLACEMENT_MAPPING_H_
#define TECHNIQUE_DISPLACEMENT_MAPPING_DISPLACEMENT_MAPPING_H_

#include "cg/rnd/dx11.h"
#include "cg/sys/app.h"


namespace displacemnet_mapping {

public Displacement_mapping final : public virtual cg::sys::Example {
public:

	Displacement_mapping(const App_context& app_ctx, cg::rnd::Rhi_context_i& rhi_ctx);

	Displacement_mapping(const Displacement_mapping&) = delete;

	Displacement_mapping(Displacement_mapping&&) = delete;


	Displacement_mapping& operator=(const Displacement_mapping&) = delete;

	Displacement_mapping& operator=(Displacement_mapping&&) = delete;

private:
};

} // displacemnet_mapping

#endif // TECHNIQUE_DISPLACEMENT_MAPPING_DISPLACEMENT_MAPPING_H_
