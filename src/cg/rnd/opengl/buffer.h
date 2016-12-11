#ifndef CG_RND_OPENGL_BUFFER_H_
#define CG_RND_OPENGL_BUFFER_H_

#include <vector>
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


// not implemented
// Buffer_map (read/write)
// Buffer_persistent_map (read/write)

} // namespace opengl
} // namespace rnd
} // namespace cg

#endif // CG_RND_OPENGL_BUFFER_H_


