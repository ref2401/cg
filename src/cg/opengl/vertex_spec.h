#ifndef CG_OPENGL_VERTEX_SPEC_H_
#define CG_OPENGL_VERTEX_SPEC_H_

#include <cassert>
#include <iostream>
#include <vector>
#include "cg/data/mesh.h"
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


	friend bool operator==(const DE_cmd& lhs, const DE_cmd& rhs) noexcept;

	friend std::ostream& operator<<(std::ostream& out, const DE_cmd& cmd);

	friend std::wostream& operator<<(std::wostream& out, const DE_cmd& cmd);

private:
	GLuint _vao_id;
	size_t _index_count;
	size_t _offset_index;
	size_t _base_vertex;
	size_t _instance_count = 1;
	size_t _base_instance = 0;
};

class Static_vertex_spec_builder final {
public:

	// Reserves memory for vertex buffer and index buffer data.
	// The allocated memory will be reused by evry vertex specification building process.
	explicit Static_vertex_spec_builder(size_t vertex_buffer_capacity = 64, size_t index_buffer_capacity = 256);

	// Start building process.
	// Params:
	// -	format: Describes required vertex attributes.
	// -	vertex_bytes_limit: The memory threshold for the vertex buffer.
	void begin(cg::data::Interleaved_vertex_format format, size_t vertex_limit_bytes);

	DE_cmd push_back(const cg::data::Interleaved_mesh_data& mesh_data);

private:
	std::vector<float> _vertex_data;
	std::vector<uint32_t> _index_data;
	// vertex specification building process related.
	// the following fields are reset every begin() call
	GLuint _vao_id = invalid_vao_id;
	cg::data::Interleaved_vertex_format _format;
	size_t _vertex_limit_bytes;
	size_t _index_count;
	size_t _base_vertex;
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

inline bool operator==(const DE_cmd& lhs, const DE_cmd& rhs) noexcept
{
	return (lhs._vao_id == rhs._vao_id)
		&& (lhs._index_count == rhs._index_count)
		&& (lhs._offset_index == rhs._offset_index)
		&& (lhs._base_vertex == rhs._base_vertex)
		&& (lhs._instance_count == rhs._instance_count)
		&& (lhs._base_instance == rhs._base_instance);
}

inline bool operator!=(const DE_cmd& lhs, const DE_cmd& rhs) noexcept
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

inline std::ostream& operator<<(std::ostream& out, const DE_cmd& cmd)
{
	out << "DE_cmd(" << cmd._vao_id << ", " << cmd._index_count << ", "
		<< cmd._offset_index << ", " << cmd._base_vertex << ", "
		<< cmd._instance_count << ", " << cmd._base_instance << ')';
	return out;
}

inline std::wostream& operator<<(std::wostream& out, const DE_cmd& cmd)
{
	out << "DE_cmd(" << cmd._vao_id << ", " << cmd._index_count << ", "
		<< cmd._offset_index << ", " << cmd._base_vertex << ", "
		<< cmd._instance_count << ", " << cmd._base_instance << ')';
	return out;
}

} // namespace opengl
} // namespace cg

#endif // CG_OPENGL_VERTEX_SPEC_H_
