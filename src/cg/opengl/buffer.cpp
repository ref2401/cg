#include "cg/opengl/buffer.h"

#include <cassert>


namespace cg {
namespace opengl {

Persistent_buffer::Persistent_buffer(size_t byte_count) noexcept
	: _byte_count(byte_count)
{
	assert(byte_count > 0);
	
	GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
	glCreateBuffers(1, &_id);
	glNamedBufferStorage(_id, byte_count, nullptr, flags);
	_ptr = glMapNamedBufferRange(_id, 0, byte_count, flags);
}

Persistent_buffer::~Persistent_buffer() noexcept
{
	if (_id == Invalid::buffer_id) return;

	glUnmapNamedBuffer(_id);
	glDeleteBuffers(1, &_id);

	_id = Invalid::buffer_id;
	_ptr = nullptr;
}

} // namespace opengl
} // namespace cg