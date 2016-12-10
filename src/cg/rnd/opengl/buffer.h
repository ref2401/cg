#ifndef CG_RND_OPENGL_BUFFER_H_
#define CG_RND_OPENGL_BUFFER_H_

#include "cg/rnd/opengl/opengl_def.h"
#include "cg/rnd/opengl/opengl_utility.h"


namespace cg {
namespace rnd {
namespace opengl {


// The contents of the data store may be updated after creation through calls to glBufferSubData.
//class Buffer {};

// Buffer_gpu represents a buffer object that is owned almost entirely by OpenGL processes.
// The contents of such buffer is written by Compute shader, Transform feedback, 
// other gpu (server) side routines. There is no way to modify buffer's contents from the cpu-side after creation.
// Also Buffer_gpu objects can be used as static data buffer. 
// Data stored in such buffer objects will not be changed once it is uploaded.
class Buffer_gpu final {
public:

	Buffer_gpu() noexcept = default;

	Buffer_gpu(size_t byte_count, const void* data_ptr) noexcept;

	Buffer_gpu(const Buffer_gpu&) = delete;

	Buffer_gpu(Buffer_gpu&& buffer) noexcept;

	~Buffer_gpu() noexcept;


	Buffer_gpu& operator=(const Buffer_gpu&) = delete;

	Buffer_gpu& operator=(Buffer_gpu&& buffer) noexcept;


	// Size in bytes of the buffer's data store. 
	size_t byte_count() const noexcept
	{
		return _byte_count;
	}

	// Buffer's unique id.
	GLuint id() const noexcept
	{
		return _id;
	}

private:

	void dispose() noexcept;

	GLuint _id = Invalid::buffer_id;
	size_t _byte_count = 0;
};


} // namespace opengl
} // namespace rnd
} // namespace cg

#endif // CG_RND_OPENGL_BUFFER_H_


