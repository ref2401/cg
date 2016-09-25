#ifndef CG_OPENGL_BUFFER_H_
#define CG_OPENGL_BUFFER_H_

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

	template<typename T>
	void write(size_t offset, T* ptr) noexcept;

private:
	size_t _byte_count = 0;
	GLuint _id = Invalid::buffer_id;
	void* _ptr = nullptr;
};

template<typename T>
void write(size_t offset, T* ptr) noexcept
{
	// default constructible, see std::copy
	// offset + ptr + size < _byte_count
}

} // namespace opengl 
} // namespace cg

#endif // CG_OPENGL_BUFFER_H_
