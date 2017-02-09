#include "cg/rnd/opengl/buffer.h"


namespace cg {
namespace rnd {
namespace opengl {

// ----- Buffer -----

Buffer::Buffer(GLenum usage, GLsizeiptr byte_count, const void* data_ptr) noexcept
	: _byte_count(byte_count)
{
	assert(byte_count > 0);
	assert(is_valid_buffer_usage(usage));

	glCreateBuffers(1, &_id);
	glNamedBufferData(_id, byte_count, data_ptr, usage);
}

Buffer::Buffer(Buffer&& buffer) noexcept :
	_id(buffer._id),
	_byte_count(buffer._byte_count)
{
	buffer._id = Blank::buffer_id;
	buffer._byte_count = 0;
}

Buffer::~Buffer() noexcept
{
	dispose();
}

Buffer& Buffer::operator=(Buffer&& buffer) noexcept
{
	if (this == &buffer) return *this;

	dispose();

	_id = buffer._id;
	_byte_count = buffer._byte_count;

	buffer._id = Blank::buffer_id;
	buffer._byte_count = 0;

	return *this;
}

void Buffer::dispose() noexcept
{
	if (_id == Blank::buffer_id) return;

	glDeleteBuffers(1, &_id);
	_id = Blank::buffer_id;
	_byte_count = 0;
}

// ----- Buffer_immut -----

Buffer_immut::Buffer_immut(GLbitfield flags, size_t byte_count, const void* data_ptr) noexcept 
	: _byte_count(byte_count)
{
	assert(byte_count > 0);

	glCreateBuffers(1, &_id);
	glNamedBufferStorage(_id, _byte_count, data_ptr, flags);
}

Buffer_immut::Buffer_immut(Buffer_immut&& buffer) noexcept
	: _id(buffer._id),
	_byte_count(buffer._byte_count)
{
	buffer._id = Blank::buffer_id;
	buffer._byte_count = 0;
}

Buffer_immut::~Buffer_immut() noexcept
{
	dispose();
}

Buffer_immut& Buffer_immut::operator=(Buffer_immut&& buffer) noexcept
{
	if (this == &buffer) return *this;

	dispose();

	_id = buffer._id;
	_byte_count = buffer._byte_count;

	buffer._id = Blank::buffer_id;
	buffer._byte_count = 0;

	return *this;
}

void Buffer_immut::dispose() noexcept
{
	if (_id == Blank::buffer_id) return;

	glDeleteBuffers(1, &_id);
	_id = Blank::buffer_id;
}

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
	buffer._id = Blank::buffer_id;
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

	buffer._id = Blank::buffer_id;
	buffer._byte_count = 0;
	buffer._curr_byte_count = 0;

	return *this;
}

void Buffer_dynamic::dispose() noexcept
{
	if (_id == Blank::buffer_id) return;

	glDeleteBuffers(1, &_id);
	_id = Blank::buffer_id;
}

void Buffer_dynamic::read(void* dest_ptr) noexcept
{
	assert(_id != Blank::buffer_id);
	assert(dest_ptr);

	if (_curr_byte_count == 0) return;
	if (_byte_count == 0) return;

	glGetNamedBufferSubData(_id, 0, _curr_byte_count, dest_ptr);
}

void Buffer_dynamic::write(size_t byte_count, const void* data_ptr)
{
	assert(_id != Blank::buffer_id);

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
	buffer._id = Blank::buffer_id;
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

	buffer._id = Blank::buffer_id;
	buffer._byte_count = 0;

	return *this;
}

void Buffer_gpu::dispose() noexcept
{
	if (_id == Blank::buffer_id) return;

	glDeleteBuffers(1, &_id);
	_id = Blank::buffer_id;
}

// ----- Buffer_persistent-----

Buffer_persistent_map::Buffer_persistent_map(GLsizeiptr byte_count) noexcept
	: _byte_count(byte_count)
{
	assert(byte_count > 0);

	constexpr GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;

	glCreateBuffers(1, &_id);
	glNamedBufferStorage(_id, _byte_count, nullptr, flags);
	_ptr = glMapNamedBufferRange(_id, 0, _byte_count, flags);
}

Buffer_persistent_map::Buffer_persistent_map(Buffer_persistent_map&& buffer) noexcept
	: _id(buffer._id),
	_byte_count(buffer._byte_count),
	_ptr(buffer._ptr)
{
	buffer._id = Blank::buffer_id;
	buffer._byte_count = 0;
	buffer._ptr = nullptr;
}

Buffer_persistent_map::~Buffer_persistent_map() noexcept
{
	dispose();
}

Buffer_persistent_map& Buffer_persistent_map::operator=(Buffer_persistent_map&& buffer) noexcept
{
	if (this == &buffer) return *this;

	dispose();
	
	_id = buffer._id;
	_byte_count = buffer._byte_count;
	_ptr = buffer._ptr;

	buffer._id = Blank::buffer_id;
	buffer._byte_count = 0;
	buffer._ptr = nullptr;

	return *this;
}

void Buffer_persistent_map::dispose() noexcept
{
	if (_id == Blank::buffer_id) return;

	glUnmapNamedBuffer(_id);
	glDeleteBuffers(1, &_id);

	_byte_count = 0;
	_id = Blank::buffer_id;
	_ptr = nullptr;
}

// ---- funcs -----

void copy(const Buffer_i& src, GLintptr  src_offset,
	const Buffer_i& dest, GLintptr  dest_offset, GLsizeiptr byte_count) noexcept
{
	assert(src.id() != Blank::buffer_id);
	assert(dest.id() != Blank::buffer_id);
	assert(src.byte_count() >= src_offset + byte_count);
	assert(dest.byte_count() >= dest_offset + byte_count);

	if (byte_count == 0) return;

	glCopyNamedBufferSubData(src.id(), dest.id(), src_offset, dest_offset, byte_count);
}

bool is_valid_buffer_target(GLenum target) noexcept
{
	return target == GL_ARRAY_BUFFER
		|| target == GL_ATOMIC_COUNTER_BUFFER
		|| target == GL_DISPATCH_INDIRECT_BUFFER
		|| target == GL_DRAW_INDIRECT_BUFFER
		|| target == GL_COPY_READ_BUFFER
		|| target == GL_COPY_WRITE_BUFFER
		|| target == GL_ELEMENT_ARRAY_BUFFER
		|| target == GL_PIXEL_PACK_BUFFER
		|| target == GL_PIXEL_UNPACK_BUFFER
		|| target == GL_QUERY_BUFFER
		|| target == GL_SHADER_STORAGE_BUFFER
		|| target == GL_TEXTURE_BUFFER
		|| target == GL_TRANSFORM_FEEDBACK_BUFFER
		|| target == GL_UNIFORM_BUFFER;
}

bool is_valid_buffer_usage(GLenum usage) noexcept
{
	return usage == GL_STATIC_COPY
		|| usage == GL_STATIC_READ
		|| usage == GL_STATIC_DRAW
		|| usage == GL_DYNAMIC_COPY
		|| usage == GL_DYNAMIC_READ
		|| usage == GL_DYNAMIC_DRAW
		|| usage == GL_STREAM_COPY
		|| usage == GL_STREAM_COPY
		|| usage == GL_STREAM_DRAW;
}

} // namespace opengl
} // namespace rnd
} // namespace cg
