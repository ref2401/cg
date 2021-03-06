#ifndef CG_RND_RND_H_
#define CG_RND_RND_H_

#include <memory>
#include <windows.h>
#include "cg/base/math.h"


namespace cg {
namespace rnd {

enum class depth_stencil_format : unsigned char {
	none,
	depth_24_stencil_8,
	depth_32,
	depth_32f
};

enum class render_api : unsigned char {
	dx_11,
	opengl_45
};

class rhi_context_i {
public:

	virtual ~rhi_context_i() noexcept = default;


	virtual render_api render_api() const noexcept = 0;

	virtual void resize_viewport(const uint2& viewport_size) = 0;

	virtual void swap_color_buffers() = 0;
};

std::unique_ptr<rhi_context_i> rhi_context(render_api render_api, HWND hwnd, 
	const uint2& viewport_size, depth_stencil_format depth_stencil_format);

} // namepsace rnd
} // namespace cg

#endif // CG_RND_RND_H_
