#include "cg/rnd/opengl/buffer.h"

#include <cassert>

namespace cg {
namespace rnd {
namespace opengl {

// ----- Buffer_dynamic -----

Buffer_dynamic::Buffer_dynamic() noexcept : 
	Buffer_dynamic(0, nullptr)
{}

Buffer_dynamic::Buffer_dynamic(size_t byte_count, const void* data_ptr) noexcept :
	_byte_count(byte_count)
{
	if (data_ptr) assert(byte_count > 0);

	glCreateBuffers(1, &_id);
	if (byte_count > 0) 
		glNamedBufferData(_id, byte_count, data_ptr, Buffer_dynamic::usage_flags);
}

Buffer_dynamic::Buffer_dynamic(Buffer_dynamic&& buffer) noexcept :
	_id(buffer._id),
	_byte_count(buffer._byte_count),
	_curr_byte_count(buffer._curr_byte_count)
{
	buffer._id = Invalid::buffer_id;
	buffer._byte_count = 0;
	buffer._curr_byte_count = 0;
}

Buffer_dynamic::~Buffer_dynamic() noexcept
{
	dispose();
}

Buffer_dynamic& Buffer_dynamic::operator=(Buffer_dynamic&& buffer) noexcept
{
	if (this == &buffer) return *this;

	dispose();

	_id = buffer._id;
	_byte_count = buffer._byte_count;
	_curr_byte_count = buffer._curr_byte_count;

	buffer._id = Invalid::buffer_id;
	buffer._byte_count = 0;
	buffer._curr_byte_count = 0;

	return *this;
}

void Buffer_dynamic::dispose() noexcept
{
	if (_id == Invalid::buffer_id) return;

	glDeleteBuffers(1, &_id);
	_id = Invalid::buffer_id;
}

void Buffer_dynamic::read(void* dest_ptr) noexcept
{
	assert(_id != Invalid::buffer_id);
	assert(dest_ptr);

	if (_curr_byte_count == 0) return;
	if (_byte_count == 0) return;

	glGetNamedBufferSubData(_id, 0, _curr_byte_count, dest_ptr);
}

void Buffer_dynamic::write(size_t byte_count, const void* data_ptr)
{
	assert(_id != Invalid::buffer_id);

	size_t req_byte_count = _curr_byte_count + byte_count;
	if (req_byte_count > _byte_count) {
		unsigned char* tmp = nullptr;
		
		if (_curr_byte_count > 0) {
			tmp = new(std::nothrow) unsigned char[_curr_byte_count];
			assert(tmp);
			read(tmp);
		}

		glNamedBufferData(_id, req_byte_count, tmp, Buffer_dynamic::usage_flags);
		_byte_count = req_byte_count;
		delete[] tmp;
	}

	glNamedBufferSubData(_id, _curr_byte_count, byte_count, data_ptr);
	auto err = glGetError();
	_curr_byte_count += byte_count;
}

// ----- Buffer_gpu -----

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

	buffer._id = Invalid::buffer_id;
	buffer._byte_count = 0;

	return *this;
}

void Buffer_gpu::dispose() noexcept
{
	if (_id == Invalid::buffer_id) return;

	glDeleteBuffers(1, &_id);
	_id = Invalid::buffer_id;
}

// ---- funcs -----

void copy(const Buffer_i& src, size_t src_offset,
	Buffer_i& dest, size_t dest_offset, size_t byte_count) noexcept
{
	assert(src.id() != Invalid::buffer_id);
	assert(dest.id() != Invalid::buffer_id);
	assert(src.byte_count() >= src_offset + byte_count);
	assert(dest.byte_count() >= dest_offset + byte_count);

	if (byte_count == 0) return;

	glCopyNamedBufferSubData(src.id(), dest.id(), src_offset, dest_offset, byte_count);
}


} // namespace opengl
} // namespace rnd
} // namespace cg
