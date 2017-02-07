#ifndef CG_RND_OPENGL_BUFFER_H_
#define CG_RND_OPENGL_BUFFER_H_

#include <cassert>
#include <array>
#include <type_traits>
#include <utility>
#include <vector>
#include "cg/base/container.h"
#include "cg/rnd/opengl/opengl_def.h"
#include "cg/rnd/opengl/opengl_utility.h"


namespace cg {
namespace rnd {
namespace opengl {

class Buffer_i {
public:

	virtual ~Buffer_i() noexcept {}


	// Size in bytes of the buffer's data store. 
	virtual size_t byte_count() const noexcept = 0;

	// Buffer's unique id.
	virtual GLuint id() const noexcept = 0;
};

// Buffer_dynamic is a buffer object which contents and size may be changed after creation.
// Buffer_dynamic can be used to accumulate data which size is not known and then copy data
// into some other buffer. 
// Buffer_dynamic is not meant to be used in streaming techniques.
class Buffer_dynamic final : public virtual Buffer_i {
public:

	Buffer_dynamic() noexcept;

	template<typename T>
	explicit Buffer_dynamic(const std::vector<T>& vector) noexcept
		: Buffer_dynamic(cg::byte_count(vector), vector.data())
	{}

	explicit Buffer_dynamic(size_t byte_count, const void* data_ptr = nullptr) noexcept;

	Buffer_dynamic(const Buffer_dynamic&) = delete;

	Buffer_dynamic(Buffer_dynamic&& buffer) noexcept;

	~Buffer_dynamic() noexcept;


	Buffer_dynamic& operator=(const Buffer_dynamic&) = delete;

	Buffer_dynamic& operator=(Buffer_dynamic&& buffer) noexcept;


	// Size in bytes of the buffer's data store. 
	size_t byte_count() const noexcept override
	{
		return _byte_count;
	}

	// Number of bytes already written into the buffer's data store.
	size_t curr_byte_count() const noexcept
	{
		return _curr_byte_count;
	}

	// Buffer's unique id.
	GLuint id() const noexcept override
	{
		return _id;
	}

	// Copies buffer's contents into dest_ptr. 
	// dest_ptr must point to sequence which size is at least curr_byte_count().
	// If buffer is empty then dest_ptr contents is left unchanged.
	void read(void* dest_ptr) noexcept;

	// Copies byte_count sequential bytes pointed to by data_ptr into buffer's data store.
	// Reallocates buffer's data store (preserving already written data) if there is not enough memory.
	void write(size_t byte_count, const void* data_ptr);

private:

	static constexpr GLenum usage_flags = GL_DYNAMIC_READ | GL_DYNAMIC_DRAW;

	void dispose() noexcept;

	GLuint _id = Blank::buffer_id;
	size_t _byte_count = 0;
	size_t _curr_byte_count = 0;
};

// Buffer_gpu represents a buffer object that is owned almost entirely by OpenGL processes.
// The contents of such buffer is written by Compute shader, Transform feedback, 
// other gpu (server) side routines. It is not possible to modify buffer's contents from the cpu-side after creation.
// Also Buffer_gpu objects can be used as static data buffer. 
// Data stored in such buffer objects will not be changed once it is uploaded.
class Buffer_gpu final : public virtual Buffer_i {
public:

	Buffer_gpu() noexcept = default;

	template<typename T>
	explicit Buffer_gpu(const std::vector<T>& vector) noexcept
		: Buffer_gpu(cg::byte_count(vector), vector.data())
	{}

	Buffer_gpu(size_t byte_count, const void* data_ptr) noexcept;

	Buffer_gpu(const Buffer_gpu&) = delete;

	Buffer_gpu(Buffer_gpu&& buffer) noexcept;

	~Buffer_gpu() noexcept;


	Buffer_gpu& operator=(const Buffer_gpu&) = delete;

	Buffer_gpu& operator=(Buffer_gpu&& buffer) noexcept;


	// Size in bytes of the buffer's data store. 
	size_t byte_count() const noexcept override
	{
		return _byte_count;
	}

	// Buffer's unique id.
	GLuint id() const noexcept override
	{
		return _id;
	}

private:

	void dispose() noexcept;

	GLuint _id = Blank::buffer_id;
	size_t _byte_count = 0;
};

// Persistent_buffer represents opengl buffer object that is mapped once during the constructuon
// and is used in vertex streaming process.
// Implementation notes: Persistent_buffer allow write data on the CPU side only.
class Buffer_persistent_map final : public virtual Buffer_i {
public:

	Buffer_persistent_map() noexcept = default;

	Buffer_persistent_map(size_t byte_count) noexcept;

	Buffer_persistent_map(const Buffer_persistent_map& buff) = delete;

	Buffer_persistent_map(Buffer_persistent_map&& buff) noexcept;

	~Buffer_persistent_map() noexcept;


	Buffer_persistent_map& operator=(const Buffer_persistent_map&) = delete;

	Buffer_persistent_map& operator=(Buffer_persistent_map&& buffer) noexcept;


	// How many bytes are allocated for this buffer object on the GPU.
	size_t byte_count() const noexcept override
	{
		return _byte_count;
	}

	GLuint id() const noexcept override
	{
		return _id;
	}

	// Writes a sequence of objects of type T into the buffer.
	// Params:
	// -	offset: Byte offset from the beginig of the buffer.
	// -	ptr: Pointer to the first object of type T in the sequence.
	// -	count: How many objects are in the sequence pointed by ptr.
	//		If count is 0 the method returns immediately.
	// Returns:	offset + sizeof(T) * count.
	//			Byte offset into the buffer that can be used for subsequent writes.	
	template<typename T>
	size_t write(size_t offset, const T* ptr, size_t count = 1) noexcept;

private:

	void dispose() noexcept;

	GLuint _id = Blank::buffer_id;
	size_t _byte_count = 0;
	void* _ptr = nullptr;
};

template<typename T>
size_t Buffer_persistent_map::write(size_t offset, const T* ptr, size_t count) noexcept
{
	static_assert(std::is_trivially_copy_assignable<T>::value, "T must satisfy is_trivially_copy_assignable.");
	if (count == 0) return offset;

	const size_t data_byte_count = sizeof(T) * count;
	const size_t ret_offset = offset + data_byte_count;
	assert(ret_offset <= _byte_count);

	std::memcpy(reinterpret_cast<unsigned char*>(_ptr) + offset, ptr, data_byte_count);
	return ret_offset;
}

// Partitioned_buffer is a buffer decorator that implements double/triple/whatever_factor buffering technique.
// Buffering technique is supposed to solve CPU-GPU synchronization problem.
// Partitioned_buffer creates an internal object of type Buffer_type 
// which is partitioned into subbufer_count equal chunks. 
// At every time Partitioned_buffer writes only into the specific(active/current) chunk of the internal buffer.
template<typename Buffer_type>
class Buffer_partitioned final {

	static_assert(std::is_same<Buffer_type, Buffer_persistent_map>::value,
		"Only Buffer_persistent_map as Buffer_type is supported at the moment.");

public:

	// Creates object of Buffer_type which size is byte_count * subbuffer_count bytes.
	Buffer_partitioned(size_t partition_count, size_t partition_byte_count) noexcept
		: _partition_count(partition_count), _partition_byte_count(partition_byte_count),
		_buffer(partition_count * partition_byte_count)
	{}

	Buffer_partitioned(const Buffer_partitioned&) = delete;

	Buffer_partitioned(Buffer_partitioned&& bp) noexcept;

	~Buffer_partitioned() noexcept = default;


	Buffer_partitioned& operator=(const Buffer_partitioned&) = delete;

	Buffer_partitioned& operator=(Buffer_partitioned&& bp) noexcept;


	// The index of partition which is currently used as target for writes.
	// The number lies in range [0, partition_count).
	size_t current_partition_index() const noexcept
	{
		return _partition_offset / _partition_byte_count;
	}

	GLuint id() const noexcept
	{
		return _buffer.id();
	}

	// Makes current the next buffer partition. If the end of the internal buffer has been reached
	// then the first partition (index: 0) will be current.
	void move_next_partition() noexcept
	{
		size_t next_index = (current_partition_index() + 1) % _partition_count;
		_partition_offset = next_index * _partition_byte_count;
	}

	// How many bytes are occupies by each partition of the internal buffer.
	size_t partition_byte_count() const noexcept
	{
		return _partition_byte_count;
	}

	// The number of partitions of the internal buffer.
	size_t partition_count() const noexcept
	{
		return _partition_count;
	}

	// Writes a sequence of objects of type T into the current partiotion of the internal buffer.
	// Params:
	// -	rel_offset: Byte offset from the beginig of the current partition.
	// -	ptr: Pointer to the first object of type T in the sequence.
	// -	count: How many objects are in the sequence pointed by ptr.
	//		If count is 0 the method returns immediately.
	// Returns:	rel_offset + sizeof(T) * count.
	//			Byte offset into the current partition that can be used for subsequent writes.	
	template<typename T>
	size_t write(size_t rel_offset, const T* ptr, size_t count = 1) noexcept;

private:
	size_t _partition_count = 0;
	size_t _partition_byte_count = 0;
	size_t _partition_offset = 0;
	Buffer_type _buffer;
};

template<typename Buffer_type>
Buffer_partitioned<Buffer_type>::Buffer_partitioned(Buffer_partitioned&& bp) noexcept
	: _partition_count(bp._partition_count),
	_partition_byte_count(bp._partition_byte_count),
	_partition_offset(bp._partition_offset)
	_buffer(std::move(bp._buffer))
{
	bp._partition_count = 0;
	bp._partition_byte_count = 0;
	bp._partition_offset = 0;
}

template<typename Buffer_type>
Buffer_partitioned<Buffer_type>& Buffer_partitioned<Buffer_type>::operator=(Buffer_partitioned&& bp) noexcept
{
	if (this == &bp) return *this;

	_partition_count = bp._partition_count;
	_partition_byte_count = bp._partition_byte_count;
	_partition_offset = bp._partition_offset;
	_buffer = std::move(bp._buffer);

	bp._partition_count = 0;
	bp._partition_byte_count = 0;
	bp._partition_offset = 0;

	return *this;
}

template<typename Buffer_type>
template<typename T>
size_t Buffer_partitioned<Buffer_type>::write(size_t rel_offset, const T* ptr, size_t count) noexcept
{
	static_assert(std::is_trivially_copy_assignable<T>::value, "T must satisfy is_trivially_copy_assignable.");
	if (count == 0) return rel_offset;

	const size_t ret_offset = rel_offset + sizeof(T) * count;
	assert(ret_offset <= _partition_byte_count);

	_buffer.write<T>(_partition_offset + rel_offset, ptr, count);
	return ret_offset;
}


// Copies contents of the src buffer to the dest buffer.
// Params:
// -	src: Source buffer object.
// -	src_offset:	The offset, in bytes, within the source buffer object at which data will be read.
// -	dest: Destination buffer object.
// -	dest_offset: The offset, in btes, within destination buffer object at which data will be written.
// -	byte_count:	The size, in bytes, of the data to be copied 
//					from the source buffer object to the destination buffer object.
void copy(const Buffer_i& src, size_t src_offset,
	Buffer_i& dest, size_t dest_offset, size_t byte_count) noexcept;

// Copies contents of the src buffer to the dest buffer.
inline void copy(const Buffer_i& src, Buffer_i& dest, size_t byte_count) noexcept
{
	copy(src, 0, dest, 0, byte_count);
}

// If T represents one of the OpenGL buffers, 
// provides the member constant value equal to true. Otherwise value is false.
template<typename T, bool 
	= std::is_same<T, Buffer_dynamic>::value
	|| std::is_same<T, Buffer_gpu>::value>
struct is_opengl_buffer;

template<typename T> 
struct is_opengl_buffer<T, true> final : std::true_type {};

template<typename T>
struct is_opengl_buffer<T, false> final : std::false_type {};

// ditto
template<typename T>
constexpr bool is_opengl_buffer_v = is_opengl_buffer<T>::value;

} // namespace opengl
} // namespace rnd
} // namespace cg

#endif // CG_RND_OPENGL_BUFFER_H_


