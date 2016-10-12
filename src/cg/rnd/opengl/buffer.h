#ifndef CG_RND_OPENGL_BUFFER_H_
#define CG_RND_OPENGL_BUFFER_H_

#include <cassert>
#include <cstring>
#include <type_traits>
#include "cg/rnd/opengl/opengl_def.h"
#include "cg/rnd/opengl/utility.h"


namespace cg {
namespace rnd {
namespace opengl {

// Persistent_buffer represents opengl buffer object that is mapped once during the constructuon
// and is used in vertex streaming process.
// Implementation notes: Persistent_buffer allow write data on the CPU side only.
class Persistent_buffer final {
public:

	Persistent_buffer(size_t byte_count) noexcept;

	Persistent_buffer(const Persistent_buffer& buff) = delete;

	Persistent_buffer(Persistent_buffer&& buff) = delete;

	~Persistent_buffer() noexcept;

	// How many bytes are allocated for this buffer object on the GPU.
	size_t byte_count() const noexcept
	{
		return _byte_count;
	}

	GLuint id() const noexcept
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
	size_t _byte_count = 0;
	GLuint _id = Invalid::buffer_id;
	unsigned char* _ptr = nullptr;
};

// Partitioned_buffer is a buffer decorator that implements double/triple/whatever_factor buffering technique.
// Buffering technique is supposed to solve CPU-GPU synchronization problem.
// Partitioned_buffer creates an internal object of type Buffer_type 
// which is partitioned into subbufer_count equal chunks. 
// At every time Partitioned_buffer writes only into the specific(active/current) chunk of the internal buffer.
template<typename Buffer_type>
class Partitioned_buffer final {

	static_assert(std::is_same<Buffer_type, Persistent_buffer>::value,
		"Only Persistent_buffer as Buffer_type is supported at the moment.");

public:

	// Creates object of Buffer_type which size is byte_count * subbuffer_count bytes.
	Partitioned_buffer(size_t partition_count, size_t partition_byte_count) noexcept
		: _partition_count(partition_count), _partition_byte_count(partition_byte_count),
		_buffer(partition_count * partition_byte_count)
	{}

	Partitioned_buffer(const Partitioned_buffer& buff) = delete;

	Partitioned_buffer(Partitioned_buffer&& buff) = delete;

	~Partitioned_buffer() noexcept = default;


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
	size_t _partition_count;
	size_t _partition_byte_count;
	size_t _partition_offset = 0;
	Buffer_type _buffer;
};

// Static_buffer represents a buffer object which data will not be changed once it is uploaded.
class Static_buffer final {
public:

	Static_buffer() noexcept = default;

	// Constructs Static_buffer object that will manage the lifetime of the specified buffer object.
	Static_buffer(GLuint buffer_id) noexcept
		: _id(buffer_id)
	{
		assert(buffer_id != Invalid::buffer_id);
	}

	// Constructs a new buffer object and fill it using the specified sequence of objects of type T.
	// The memory allocated for such buffer object is calculated as sizeof(T) * count bytes.
	template<typename T>
	Static_buffer(const T* ptr, size_t count) noexcept;

	Static_buffer(const Static_buffer& buff) = delete;

	Static_buffer(Static_buffer&& buff) noexcept;

	~Static_buffer() noexcept;


	Static_buffer& operator=(Static_buffer&& buff) noexcept;


	GLuint id() const noexcept
	{
		return _id;
	}

private:
	void dispose() noexcept;

	GLuint _id = Invalid::buffer_id;
};

template<typename T>
Static_buffer::Static_buffer(const T* ptr, size_t count) noexcept
{
	static_assert(std::is_trivially_copy_assignable<T>::value, "T must satisfy is_trivially_copy_assignable.");
	assert(count > 0);

	glCreateBuffers(1, &_id);
	glNamedBufferStorage(_id, sizeof(T) * count, ptr, 0);
}

template<typename T>
size_t Persistent_buffer::write(size_t offset, const T* ptr, size_t count) noexcept
{
	static_assert(std::is_trivially_copy_assignable<T>::value, "T must satisfy is_trivially_copy_assignable.");
	if (count == 0) return offset;

	size_t ret_offset = offset + sizeof(T) * count;
	assert(ret_offset <= _byte_count);

	std::memcpy(_ptr + offset, ptr, sizeof(T) * count);
	return ret_offset;
}

template<typename Buffer_type>
template<typename T>
size_t Partitioned_buffer<Buffer_type>::write(size_t rel_offset, const T* ptr, size_t count) noexcept
{
	static_assert(std::is_trivially_copy_assignable<T>::value, "T must satisfy is_trivially_copy_assignable.");
	if (count == 0) return rel_offset;

	size_t ret_offset = rel_offset + sizeof(T) * count;
	assert(ret_offset <= _partition_byte_count);

	_buffer.write<T>(_partition_offset + rel_offset, ptr, count);
	return ret_offset;
}

} // namespace opengl 
} // namespace rnd
} // namespace cg

#endif // CG_RND_OPENGL_BUFFER_H_
