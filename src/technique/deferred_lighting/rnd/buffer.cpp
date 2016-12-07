#include "technique/deferred_lighting/rnd/buffer.h"

#include <cassert>


namespace deferred_lighting {
namespace rnd {

// ----- Persistent_buffer -----

Persistent_buffer::Persistent_buffer(size_t byte_count) noexcept
	: _byte_count(byte_count)
{
	assert(byte_count > 0);

	GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
	glCreateBuffers(1, &_id);
	glNamedBufferStorage(_id, _byte_count, nullptr, flags);
	_ptr = static_cast<unsigned char*>(glMapNamedBufferRange(_id, 0, _byte_count, flags));
}

Persistent_buffer::~Persistent_buffer() noexcept
{
	if (_id == Invalid::buffer_id) return;

	glUnmapNamedBuffer(_id);
	glDeleteBuffers(1, &_id);

	_id = Invalid::buffer_id;
	_ptr = nullptr;
}

// ----- Static_buffer -----

Static_buffer::Static_buffer(Static_buffer&& buff) noexcept
:	 _id(buff._id)
{
	buff._id = Invalid::buffer_id;
}

Static_buffer::~Static_buffer() noexcept
{
	dispose();
}

Static_buffer& Static_buffer::operator=(Static_buffer&& buff) noexcept
{
	dispose();

	_id = buff._id;
	buff._id = Invalid::buffer_id;
	return *this;
}

void Static_buffer::dispose() noexcept
{
	if (_id == Invalid::buffer_id) return;

	glDeleteBuffers(1, &_id);
	_id = Invalid::buffer_id;
}

} // namespace deferred_lighting
} // namespace cg
