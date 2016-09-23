#ifndef CG_OPENGL_VERTEX_SPEC_H_
#define CG_OPENGL_VERTEX_SPEC_H_

#include <cassert>
#include <iostream>
#include "cg/opengl/opengl_def.h"


namespace cg {
namespace opengl {

constexpr GLuint invalid_vao_id = 0;

// Specifies params to call the glDrawElementsBaseVertex func.
// glDrawElementsBaseVertex(mode, index_count, index_type, offset_bytes, base_vertex);
struct DE_base_vertex_params {
	DE_base_vertex_params(GLenum mode, GLsizei index_count, GLenum index_type,
		void* offset_bytes, GLint base_vertex) noexcept
		: mode(mode), index_count(index_count), index_type(index_type),
		offset_bytes(offset_bytes), base_vertex(base_vertex)
	{
		assert(mode == GL_TRIANGLES);
		assert(index_type == GL_UNSIGNED_INT);
	}

	// The type of primitive to render.
	GLenum mode;

	// The number of indices to render.
	GLsizei index_count;

	// The type of each index.
	GLenum index_type;

	// The byte offset into the buffer bound to GL_ELEMENT_ARRAY_BUFFER.
	void* offset_bytes;

	// The constant that should be added to each index while rendering using this command.
	GLint base_vertex;
};

// DE_cmd class contains all the required params to bind vertex array object
// and render geomertry which is written in a specific part of vertex/index buffers.
// The class is used to constracut various direct & indirect glDrawElenentsXXX commands' params.
class DE_cmd final {
public:

	DE_cmd(GLuint vao_id, size_t index_count, size_t offset_index, size_t base_vertex) noexcept
		: _vao_id(vao_id), _index_count(index_count), _offset_index(offset_index), _base_vertex(base_vertex)
	{}

	DE_cmd::DE_cmd(GLuint vao_id, size_t index_count, size_t offset_index, size_t base_vertex,
		size_t instance_count, size_t base_instance) noexcept
		: _vao_id(vao_id), _index_count(index_count), _offset_index(offset_index),
		_base_vertex(base_vertex), _instance_count(instance_count), _base_instance(base_instance)
	{
		assert(_vao_id != invalid_vao_id);
	}

	// The constant that should be added to each instance index
	// while fetching from enabled vertex attributes.
	// gl_InstanceID is not affected by base_instance value.
	size_t base_instance() const noexcept
	{
		return _base_instance;
	}

	// The constant that should be added to each index while rendering using this command.
	size_t base_vertex() const noexcept
	{
		return _base_vertex;
	}

	// Returns glDrawElementsBaseVertex params.
	DE_base_vertex_params get_base_vertex_params() const noexcept {
		return DE_base_vertex_params(GL_TRIANGLES, _index_count, GL_UNSIGNED_INT,
			reinterpret_cast<void*>(_offset_index * sizeof(GL_UNSIGNED_INT)), _base_vertex);
	}

	// The number of indices to be rendered.
	size_t index_count() const noexcept
	{
		return _index_count;
	}

	// The number of instances to be rendered.
	size_t instance_count() const noexcept
	{
		return _instance_count;
	}

	// The number of precedint indices that must be ignored.
	// The offset value is in units of indices (GL_UNSIGNED_INT).
	size_t offset_index() const
	{
		return _offset_index;
	}

	// Vertex array object that owns vertex/index buffers required to perform this command. 
	GLuint vao_id() const noexcept
	{
		return _vao_id;
	}

private:
	GLuint _vao_id;
	size_t _index_count;
	size_t _offset_index;
	size_t _base_vertex;
	size_t _instance_count = 1;
	size_t _base_instance = 0;
};


inline bool operator==(const DE_base_vertex_params& lhs, const DE_base_vertex_params& rhs) noexcept
{
	return (lhs.mode == rhs.mode)
		&& (lhs.index_count == rhs.index_count)
		&& (lhs.index_type == rhs.index_type)
		&& (lhs.offset_bytes == rhs.offset_bytes)
		&& (lhs.base_vertex == rhs.base_vertex);
}

inline bool operator!=(const DE_base_vertex_params& lhs, const DE_base_vertex_params& rhs) noexcept
{
	return !(lhs == rhs);
}

inline std::ostream& operator<<(std::ostream& out, const DE_base_vertex_params& p)
{
	out << "DE_base_vertex_params(" << p.mode << ", " << p.index_count << ", "
		<< p.index_type << ", " << p.offset_bytes << ", " << p.base_vertex << ')';
	return out;
}

inline std::wostream& operator<<(std::wostream& out, const DE_base_vertex_params& p)
{
	out << "DE_base_vertex_params(" << p.mode << ", " << p.index_count << ", "
		<< p.index_type << ", " << p.offset_bytes << ", " << p.base_vertex << ')';
	return out;
}

} // namespace opengl
} // namespace cg

#endif // CG_OPENGL_VERTEX_SPEC_H_
