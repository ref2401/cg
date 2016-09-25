#ifndef CG_OPENGL_UTILITY_H_
#define CG_OPENGL_UTILITY_H_

#include <cstring>
#include <type_traits>
#include "cg/opengl/opengl_def.h"


namespace cg {
namespace opengl {

// Persistent_buffer represents opengl buffer object that is mapped once during the constructuon
// and is used in vertex streaming process.
// Implementation notes: Persistent_buffer allow write data on the CPU side only.
class Persistent_buffer final {
public:

	Persistent_buffer(size_t byte_count) noexcept;

	~Persistent_buffer() noexcept;


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
	size_t write(size_t offset, T* ptr, size_t count = 1) noexcept;

private:
	size_t _byte_limit = 0;
	GLuint _id = Invalid::buffer_id;
	unsigned char* _ptr = nullptr;
};

template<typename T>
size_t Persistent_buffer::write(size_t offset, T* ptr, size_t count) noexcept
{
	static_assert(std::is_trivially_copy_assignable<T>::value, "T must satisfy is_trivially_copy_assignable.");

	size_t ret_offset = offset + sizeof(T) * count;
	assert(ret_offset < _byte_limit);

	std::memcpy(_ptr + offset, ptr, sizeof(T) * count);
	return ret_offset;
}

// ----- funcs -----

// Puts the current thread into a spinlock, while sync_obj is noc completed.
// if sync_obj is nullptr then the func returns immediately.
void wait_for(GLsync sync_obj) noexcept;

} // namespace opengl
} // namespace cg

#endif // CG_OPENGL_UTILITY_H_

