#ifndef CG_RND_RND_H_
#define CG_RND_RND_H_

namespace cg {
namespace rnd {

enum class Render_api : unsigned char {
	dx11,
	opengl
};

class Rhi_context_i {
public:

	virtual ~Rhi_context_i() noexcept = default;


	virtual Render_api render_api() const noexcept = 0;

	virtual void swap_color_buffers() noexcept = 0;
};

} // namepsace rnd
} // namespace cg

#endif // CG_RND_RND_H_
