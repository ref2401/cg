#ifndef CG_RND_OPENGL_TEXTURE_H_
#define CG_RND_OPENGL_TEXTURE_H_

#include "cg/rnd/opengl/opengl_def.h"
#include "cg/rnd/opengl/opengl_utility.h"


namespace cg {
namespace rnd {
namespace opengl {

// Validates glTexImage/glTexStorage/glTextureStorage 'internalformat' argument value.
bool is_valid_texture_internal_format(const GLenum& value) noexcept;

} // namespace opengl
} // namespace rnd
} // namespace cg

#endif // CG_RND_OPENGL_TEXTURE_H_
