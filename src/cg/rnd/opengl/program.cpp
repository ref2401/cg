#include "cg/rnd/opengl/program.h"

namespace cg {
namespace rnd {
namespace opengl {

// ----- funcs -----

bool is_valid_program_property(GLenum value) noexcept
{
	return (value == GL_DELETE_STATUS)
		|| (value == GL_INFO_LOG_LENGTH)
		|| (value == GL_LINK_STATUS)
		|| (value == GL_VALIDATE_STATUS);
}

bool is_valid_shader_property(GLenum value) noexcept
{
	return (value == GL_COMPILE_STATUS)
		|| (value == GL_DELETE_STATUS)
		|| (value == GL_INFO_LOG_LENGTH)
		|| (value == GL_SHADER_SOURCE_LENGTH)
		|| (value == GL_SHADER_TYPE);
}

bool is_valid_shader_type(GLenum value) noexcept
{
	return (value == GL_VERTEX_SHADER)
		|| (value == GL_FRAGMENT_SHADER);
}

} // namespace opengl
} // namespace rnd
} // namespace cg
