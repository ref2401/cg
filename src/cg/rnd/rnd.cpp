#include "cg/rnd/rnd.h"

#include "cg/rnd/dx11/dx11.h"
#include "cg/rnd/opengl/opengl.h"


namespace cg {
namespace rnd {

std::unique_ptr<rhi_context_i> rhi_context(render_api render_api, HWND hwnd,
	const uint2& viewport_size, depth_stencil_format depth_stencil_format)
{
	switch (render_api) {
		case render_api::dx_11:
			return std::make_unique<cg::rnd::dx11::dx11_rhi_context>(hwnd, 
				viewport_size, depth_stencil_format);

		case render_api::opengl_45:
			return std::make_unique<cg::rnd::opengl::Opengl_rhi_context>(hwnd,
				depth_stencil_format);
	}

	assert(false);
	return nullptr;
}

} // namespace rnd
} // namespace cg