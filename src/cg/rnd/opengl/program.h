#ifndef CG_RND_OPENGL_PROGRAM_H_
#define CG_RND_OPENGL_PROGRAM_H_

#include "cg/data/shader.h"
#include "cg/math/math.h"
#include "cg/rnd/opengl/opengl_def.h"
#include "cg/rnd/opengl/opengl_utility.h"


namespace cg {
namespace rnd {
namespace opengl {

// Validate glGetShader 'pname' argument value.
bool is_valid_shader_property(GLenum value) noexcept;

// Validates shader object type value. 
bool is_valid_shader_type(GLenum value) noexcept;

} // namespace opengl
} // namespace rnd
} // namespace cg

#endif // CG_RND_OPENGL_PROGRAM_H_
