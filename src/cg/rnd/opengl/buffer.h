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
	virtual GLsizeiptr byte_count() const noexcept = 0;

	// Buffer's unique id.
	virtual GLuint id() const noexcept = 0;
};

// Buffer is a buffer object which contents and size may be changed after creation. 
class Buffer final : public virtual Buffer_i {
public:

	Buffer() noexcept = default;

	template<typename T>
	Buffer(GLenum usage, const std::vector<T>& vector) noexcept
		: Buffer(usage, cg::byte_count(vector), vector.data())
	{}

	Buffer(GLenum usage, GLsizeiptr byte_count, const void* data_ptr = nullptr) noexcept;

	Buffer(const Buffer&) = delete;

	Buffer(Buffer&& buffer) noexcept;

	~Buffer() noexcept;


	Buffer& operator=(const Buffer&) = delete;

	Buffer& operator=(Buffer&& buffer) noexcept;


	// Size in bytes of the buffer's data store. 
	GLsizeiptr byte_count() const noexcept override
	{
		return _byte_count;
	}

	// Number of bytes already written into the buffer's data store.
	size_t curr_byte_count() const noexcept
	{
		return _curr_byte_count;
	}

	void dispose() noexcept;

	// Buffer's unique id.
	GLuint id() const noexcept override
	{
		return _id;
	}

private:

	GLuint _id = Blank::buffer_id;
	GLsizeiptr _byte_count = 0;
	size_t _curr_byte_count = 0;
};

class Buffer_immut final : public virtual Buffer_i {
public:

	Buffer_immut() noexcept = default;

	template<typename T>
	Buffer_immut(GLbitfield flags, const std::vector<T>& vector) noexcept
		: Buffer_immut(flags, cg::byte_count(vector), vector.data())
	{}

	Buffer_immut(GLbitfield flags, size_t byte_count, const void* data_ptr) noexcept;

	Buffer_immut(const Buffer_immut&) = delete;

	Buffer_immut(Buffer_immut&& buffer) noexcept;

	~Buffer_immut() noexcept;


	Buffer_immut& operator=(const Buffer_immut&) = delete;

	Buffer_immut& operator=(Buffer_immut&& buffer) noexcept;


	// Size in bytes of the buffer's data store. 
	GLsizeiptr byte_count() const noexcept override
	{
		return _byte_count;
	}

	void dispose() noexcept;

	// Buffer's unique id.
	GLuint id() const noexcept override
	{
		return _id;
	}

private:

	GLuint _id = Blank::buffer_id;
	GLsizeiptr _byte_count = 0;
};

// Persistent_buffer represents opengl buffer object that is mapped once during the constructuon
// and is used in vertex streaming process.
// Implementation notes: Persistent_buffer allow write data on the CPU side only.
class Buffer_persistent_map final : public virtual Buffer_i {
public:

	Buffer_persistent_map() noexcept = default;

	explicit Buffer_persistent_map(GLsizeiptr byte_count) noexcept;

	Buffer_persistent_map(const Buffer_persistent_map& buff) = delete;

	Buffer_persistent_map(Buffer_persistent_map&& buff) noexcept;

	~Buffer_persistent_map() noexcept;


	Buffer_persistent_map& operator=(const Buffer_persistent_map&) = delete;

	Buffer_persistent_map& operator=(Buffer_persistent_map&& buffer) noexcept;


	// How many bytes are allocated for this buffer object on the GPU.
	GLsizeiptr byte_count() const noexcept override
	{
		return _byte_count;
	}

	void* data() noexcept
	{
		return _ptr;
	}

	const void* data() const noexcept
	{
		return _ptr;
	}

	void dispose() noexcept;

	GLuint id() const noexcept override
	{
		return _id;
	}

private:

	GLuint _id = Blank::buffer_id;
	GLsizeiptr _byte_count = 0;
	void* _ptr = nullptr;
};


// Copies contents of the src buffer to the dest buffer.
// Params:
// -	src: Source buffer object.
// -	src_offset:	The offset, in bytes, within the source buffer object at which data will be read.
// -	dest: Destination buffer object.
// -	dest_offset: The offset, in btes, within destination buffer object at which data will be written.
// -	byte_count:	The size, in bytes, of the data to be copied 
//					from the source buffer object to the destination buffer object.
void copy(const Buffer_i& src, GLintptr src_offset,
	const Buffer_i& dest, GLintptr dest_offset, GLsizeiptr byte_count) noexcept;

// Copies contents of the src buffer to the dest buffer.
inline void copy(const Buffer_i& src, const Buffer_i& dest, GLsizeiptr byte_count) noexcept
{
	copy(src, 0, dest, 0, byte_count);
}

//// Copies byte_count sequential bytes pointed to by data_ptr into buffer's data store.
//// Reallocates buffer's data store (preserving already written data) if there is not enough memory.
//void write(const Buffer_i& buffer, size_t byte_count, const void* data_ptr);

// If T represents one of the OpenGL buffers, 
// provides the member constant value equal to true. Otherwise value is false.
template<typename T, bool = std::is_same<T, Buffer>::value
	|| std::is_same<T, Buffer_immut>::value
	|| std::is_same<T, Buffer_persistent_map>::value>
struct is_opengl_buffer;

template<typename T> 
struct is_opengl_buffer<T, true> final : std::true_type {};

template<typename T>
struct is_opengl_buffer<T, false> final : std::false_type {};

// ditto
template<typename T>
constexpr bool is_opengl_buffer_v = is_opengl_buffer<T>::value;

// Validates glBindBuffer 'target' argument value.
bool is_valid_buffer_target(GLenum target) noexcept;

// Validates glBufferData/glNamedBufferData 'usage' argument value.
bool is_valid_buffer_usage(GLenum usage) noexcept;

// Writes a sequence of objects of type T into the buffer.
// Params:
// -	offset: Byte offset from the beginig of the buffer.
// -	ptr: Pointer to the first object of type T in the sequence.
// -	count: How many objects are in the sequence pointed by ptr.
//		If count is 0 the method returns immediately.
// Returns:	offset + sizeof(T) * count.
//			Byte offset into the buffer that can be used for subsequent writes.	
template<typename T>
size_t write(Buffer_persistent_map& buffer, size_t offset, const T* ptr, size_t count = 1) noexcept
{
	static_assert(std::is_trivially_copy_assignable<T>::value, "T must satisfy is_trivially_copy_assignable.");
	if (count == 0) return offset;

	const size_t data_byte_count = sizeof(T) * count;
	const size_t ret_offset = offset + data_byte_count;
	assert(GLsizeiptr(ret_offset) <= buffer.byte_count());

	std::memcpy(reinterpret_cast<unsigned char*>(buffer.data()) + offset, ptr, data_byte_count);
	return ret_offset;
}

} // namespace opengl
} // namespace rnd
} // namespace cg

#endif // CG_RND_OPENGL_BUFFER_H_


