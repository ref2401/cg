#include "cg/opengl/utility.h"

#include <cassert>


namespace cg {
namespace opengl {

Persistent_buffer::Persistent_buffer(size_t byte_count) noexcept
	: _byte_limit(byte_count)
{
	assert(byte_count > 0);
	
	GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
	glCreateBuffers(1, &_id);
	glNamedBufferStorage(_id, _byte_limit, nullptr, flags);
	_ptr = static_cast<unsigned char*>(glMapNamedBufferRange(_id, 0, _byte_limit, flags));
}

Persistent_buffer::~Persistent_buffer() noexcept
{
	if (_id == Invalid::buffer_id) return;

	glUnmapNamedBuffer(_id);
	glDeleteBuffers(1, &_id);

	_id = Invalid::buffer_id;
	_ptr = nullptr;
}

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


} // namespace opengl
} // namespace cg