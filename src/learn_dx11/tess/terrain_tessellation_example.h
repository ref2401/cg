#ifndef LEARN_DX11_TESS_TERRAIN_TESSELLATION_EXAMPLE_H_
#define LEARN_DX11_TESS_TERRAIN_TESSELLATION_EXAMPLE_H_

#include "cg/math/math.h"
#include "learn_dx11/base/app.h"


namespace learn_dx11 {
namespace tess {

class Terrain_tessellation_example final : public Example {
public:

	Terrain_tessellation_example(Render_context_old& rnd_ctx);

	Terrain_tessellation_example(const Terrain_tessellation_example&) = delete;

	Terrain_tessellation_example(Terrain_tessellation_example&&) noexcept = delete;


	Terrain_tessellation_example& operator=(const Terrain_tessellation_example&) = delete;

	Terrain_tessellation_example& operator=(Terrain_tessellation_example&&) noexcept = delete;


	void on_viewport_resize(const cg::uint2& viewport_size) override;

	void render() override;

	void update(float dt_milliseconds) override {}

private:

};

} // namespace tess
} // namespace learn_dx11

#endif // LEARN_DX11_TESS_TERRAIN_TESSELLATION_EXAMPLE_H_
