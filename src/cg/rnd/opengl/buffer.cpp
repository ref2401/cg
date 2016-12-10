#include "cg/rnd/opengl/buffer.h"

#include <cassert>

namespace cg {
namespace rnd {
namespace opengl {

// ---- Buffer_static -----

Buffer_gpu::Buffer_gpu(size_t byte_count, const void* data_ptr) noexcept :
	_byte_count(byte_count)
{
	assert(byte_count > 0);

	glCreateBuffers(1, &_id);
	glNamedBufferStorage(_id, _byte_count, data_ptr, 0);
}

Buffer_gpu::Buffer_gpu(Buffer_gpu&& buffer) noexcept :
	_id(buffer._id),
	_byte_count(buffer._byte_count)
{
	buffer._id = Invalid::buffer_id;
	buffer._byte_count = 0;
}

Buffer_gpu::~Buffer_gpu() noexcept
{
	dispose();
}

Buffer_gpu& Buffer_gpu::operator=(Buffer_gpu&& buffer) noexcept
{
	if (this == &buffer) return *this;

	dispose();
	_id = buffer._id;
	_byte_count = buffer._byte_count;

	return *this;
}

void Buffer_gpu::dispose() noexcept
{
	if (_id == Invalid::buffer_id) return;

	glDeleteBuffers(1, &_id);
}

// ---- funcs -----

} // namespace opengl
} // namespace rnd
} // namespace cg
