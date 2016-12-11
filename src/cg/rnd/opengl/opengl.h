#ifndef CG_RND_OPENGL_OPENGL_H_
#define CG_RND_OPENGL_OPENGL_H_

#include "cg/rnd/rnd.h"
#include "cg/rnd/opengl/buffer.h"
#include "cg/rnd/opengl/glcorearb.h"
#include "cg/rnd/opengl/opengl_def.h"
#include <windows.h>


namespace cg {
namespace rnd {
namespace opengl {

class Opengl_rhi_context final : public virtual Rhi_context_i {
public:

	Opengl_rhi_context(HWND hwnd);

	Opengl_rhi_context(const Opengl_rhi_context&) = delete;

	Opengl_rhi_context(Opengl_rhi_context&&) = delete;

	~Opengl_rhi_context() noexcept override;


	Opengl_rhi_context& operator=(const Opengl_rhi_context&) = delete;

	Opengl_rhi_context& operator=(Opengl_rhi_context&&) = delete;


	Render_api render_api() const noexcept override
	{
		return Render_api::opengl;
	}

	void swap_color_buffers() noexcept override;

private:

	void inti_context();

	void load_opengl_11() const;

	void load_opengl_45() const;

	// Opengl_render_context does not own and is not responsible for _hwnd object.
	HWND _hwnd = nullptr;
	HDC _hdc = nullptr;
	HGLRC _hglrc = nullptr;
	HMODULE _opengl_dll = nullptr;
	int _version_major = 0;
	int _version_minor = 0;
};

} // namespace opengl
} // namespace rnd
} // namespace cg

#endif // CG_RND_OPENGL_OPENGL_H_

