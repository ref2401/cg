#include "cg/rnd/opengl/texture.h"

namespace cg {
namespace rnd {
namespace opengl {

bool is_valid_texture_internal_format(const GLenum& value) noexcept
{
	return (value == GL_R8)
		|| (value == GL_R32F)
		|| (value == GL_RG8)
		|| (value == GL_RG32F)
		|| (value == GL_RGB8)
		|| (value == GL_RGB32F)
		|| (value == GL_RGBA8)
		|| (value == GL_RGBA32F)
		|| (value == GL_DEPTH_COMPONENT24)
		|| (value == GL_DEPTH24_STENCIL8)
		|| (value == GL_DEPTH32F_STENCIL8)
		|| (value == GL_DEPTH_COMPONENT32)
		|| (value == GL_DEPTH_COMPONENT32F);
}

} // namespace opengl
} // namespace rnd
} // namespace cg
