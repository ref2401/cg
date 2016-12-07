#include "technique/deferred_lighting/rnd/utility.h"

#include <cassert>


namespace deferred_lighting {
namespace rnd {

// ----- funcs -----

void wait_for(GLsync sync_obj) noexcept
{
	// NOTE(ref2401): The implementation is inspired by the azdo talk.

	if (!sync_obj) return;

	GLbitfield wait_flags = 0; // first time just try to check without flushing to the gpu queue.
	GLuint64 wait_timeout = 0;
	while (true) {
		GLenum status = glClientWaitSync(sync_obj, wait_flags, wait_timeout);
		if (status == GL_CONDITION_SATISFIED || status == GL_ALREADY_SIGNALED) break;
		
		assert(status != GL_WAIT_FAILED);
		wait_flags = GL_SYNC_FLUSH_COMMANDS_BIT;
		wait_timeout = 1'000'000; // 1 milisecond in nanoseconds
	}
}


} // namespace rnd
} // namespace deferred_lighting