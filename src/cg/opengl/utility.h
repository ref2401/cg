#ifndef CG_OPENGL_UTILITY_H_
#define CG_OPENGL_UTILITY_H_

#include <cstring>
#include <type_traits>
#include "cg/opengl/opengl_def.h"


namespace cg {
namespace opengl {

class Invalid final {
public:

	static constexpr GLuint buffer_id = 0;
	static constexpr GLuint sampler_id = 0;
	static constexpr GLuint shader_id = 0;
	static constexpr GLuint shader_program_id = 0;
	static constexpr GLuint texture_id = 0;
	static constexpr GLint uniform_location = -1;
	static constexpr GLuint vao_id = 0;
	static constexpr GLint vertex_attrib_location = -1;
};

// Validate glTextureSubImage format argument value.
constexpr bool is_valid_texture_sub_image_format(const GLenum& v) noexcept
{
	return (v == GL_RED)
		|| (v == GL_RG)
		|| (v == GL_RGB)
		|| (v == GL_BGR)
		|| (v == GL_RGBA)
		|| (v == GL_BGRA)
		|| (v == GL_DEPTH_COMPONENT)
		|| (v == GL_STENCIL_INDEX);
}

// Validate glTextureSubImage type argument value.
constexpr bool is_valid_texture_sub_image_type(const GLenum& v) noexcept
{
	return (v == GL_UNSIGNED_BYTE)
		|| (v == GL_BYTE)
		|| (v == GL_UNSIGNED_SHORT)
		|| (v == GL_SHORT)
		|| (v == GL_UNSIGNED_INT)
		|| (v == GL_INT)
		|| (v == GL_FLOAT)
		|| (v == GL_UNSIGNED_BYTE_3_3_2)
		|| (v == GL_UNSIGNED_BYTE_2_3_3_REV)
		|| (v == GL_UNSIGNED_SHORT_5_6_5)
		|| (v == GL_UNSIGNED_SHORT_5_6_5_REV)
		|| (v == GL_UNSIGNED_SHORT_4_4_4_4)
		|| (v == GL_UNSIGNED_SHORT_4_4_4_4_REV)
		|| (v == GL_UNSIGNED_SHORT_5_5_5_1)
		|| (v == GL_UNSIGNED_SHORT_1_5_5_5_REV)
		|| (v == GL_UNSIGNED_INT_8_8_8_8)
		|| (v == GL_UNSIGNED_INT_8_8_8_8_REV)
		|| (v == GL_UNSIGNED_INT_10_10_10_2)
		|| (v == GL_UNSIGNED_INT_2_10_10_10_REV);
}

// Puts the current thread into a spinlock, while sync_obj is not completed.
// if sync_obj is nullptr then the func returns immediately.
void wait_for(GLsync sync_obj) noexcept;

} // namespace opengl
} // namespace cg

#endif // CG_OPENGL_UTILITY_H_

