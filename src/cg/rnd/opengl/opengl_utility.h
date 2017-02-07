#ifndef CG_RND_OPENGL_OPENGL_UTILITY_H_
#define CG_RND_OPENGL_OPENGL_UTILITY_H_

#include "cg/rnd/opengl/opengl_def.h"

namespace cg {
namespace rnd {
namespace opengl {

class Blank final {
public:

	static constexpr GLuint buffer_id = 0;
	static constexpr GLuint framebuffer_id = 0;
	static constexpr GLuint program_id = 0;
	static constexpr GLuint renderbuffer_id = 0;
	static constexpr GLuint sampler_id = 0;
	static constexpr GLuint shader_id = 0;
	static constexpr GLuint texture_id = 0;
	static constexpr GLint uniform_location = -1;
	static constexpr GLuint vao_id = 0;
	static constexpr GLint vertex_attrib_location = -1;
};


// Puts the current thread into a spinlock, while sync_obj is not completed.
// if sync_obj is nullptr then the func returns immediately.
void wait_for(GLsync sync_obj) noexcept;

} // namespace opengl
} // namespace rnd
} // namespace cg

#endif // CG_RND_OPENGL_OPENGL_UTILITY_H_

