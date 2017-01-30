#ifndef CG_RND_OPENGL_BUFFER_H_
#define CG_RND_OPENGL_BUFFER_H_

#include <cassert>
#include <array>
#include <type_traits>
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

	GLuint _id = Invalid::buffer_id;
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

	GLuint _id = Invalid::buffer_id;
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

	GLuint _id = Invalid::buffer_id;
	size_t _byte_count = 0;
	void* _ptr = nullptr;
};

template<typename T>
size_t Buffer_persistent_map::write(size_t offset, const T* ptr, size_t count) noexcept
{
	static_assert(std::is_trivially_copy_assignable<T>::value, "T must satisfy is_trivially_copy_assignable.");
	if (count == 0) return offset;

	size_t ret_offset = offset + sizeof(T) * count;
	assert(ret_offset <= _byte_count);

	std::memcpy(_ptr + offset, ptr, sizeof(T) * count);
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


