#ifndef CG_OPENGL_UTILITY_H_
#define CG_OPENGL_UTILITY_H_

#include "cg/opengl/opengl.h"


namespace cg {
namespace opengl {

GLuint make_shader(GLenum shader_type, const char* source_code);

} // namespace opengl
} // namespace cg

#endif // CG_OPENGL_UTILITY_H_