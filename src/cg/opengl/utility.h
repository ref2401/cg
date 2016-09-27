#ifndef CG_OPENGL_UTILITY_H_
#define CG_OPENGL_UTILITY_H_

#include <cstring>
#include <type_traits>
#include "cg/opengl/opengl_def.h"


namespace cg {
namespace opengl {

// Puts the current thread into a spinlock, while sync_obj is not completed.
// if sync_obj is nullptr then the func returns immediately.
void wait_for(GLsync sync_obj) noexcept;

} // namespace opengl
} // namespace cg

#endif // CG_OPENGL_UTILITY_H_

