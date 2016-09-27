#ifndef CG_OPENGL_VERTEX_SPEC_H_
#define CG_OPENGL_VERTEX_SPEC_H_

#include <cassert>
#include <iostream>
#include <memory>
#include <vector>
#include "cg/data/mesh.h"
#include "cg/opengl/shader.h"
#include "cg/opengl/opengl_def.h"


namespace cg {
namespace opengl {

// Specifies params to call the glDrawElementsBaseVertex func.
// glDrawElementsBaseVertex(mode, index_count, index_type, offset_bytes, base_vertex);
struct DE_base_vertex_params {
	DE_base_vertex_params() noexcept = default;

	DE_base_vertex_params(GLenum mode, GLsizei index_count, GLenum index_type,
		void* offset_bytes, GLint base_vertex) noexcept
		: mode(mode), index_count(index_count), index_type(index_type),
		offset_bytes(offset_bytes), base_vertex(base_vertex)
	{
		assert(mode == GL_TRIANGLES);
		assert(index_type == GL_UNSIGNED_INT);
	}

	// The type of primitive to render.
	GLenum mode = GL_TRIANGLES;

	// The number of indices to render.
	GLsizei index_count = 0;

	// The type of each index.
	GLenum index_type = GL_UNSIGNED_INT;

	// The byte offset into the buffer bound to GL_ELEMENT_ARRAY_BUFFER.
	void* offset_bytes = nullptr;

	// The constant that should be added to each index while rendering using this command.
	GLint base_vertex = 0;
};

// The layout of the struct is specified by Opengl spec.
// DE_indirect_params opbject is compatible with glDrawElementsIndirect & glMultiDrawElementsIndirect.
struct DE_indirect_params {

	DE_indirect_params() noexcept = default;

	DE_indirect_params(GLuint index_count, GLuint instance_count,
		GLuint offset_indices, GLuint base_vertex, GLuint base_instance) noexcept
		: index_count(index_count), instance_count(instance_count), 
		offset_indices(offset_indices), base_vertex(base_vertex), base_instance(base_instance)
	{}


	// The number of indices to render.
	GLuint index_count = 0;

	// The number of instances to be rendered.
	GLuint instance_count = 0;

	// The number of precedint indices that must be ignored.
	// The offset value is in units of indices (GL_UNSIGNED_INT).
	GLuint offset_indices = 0;

	// The constant that should be added to each index while rendering using this command.
	GLuint base_vertex = 0;

	// The constant that should be added to each instance index while fetching from enabled vertex attributes.
	// gl_InstanceID is not affected by base_instance value.
	GLuint base_instance = 0;
};

// DE_cmd class contains all the required params to bind vertex array object
// and render geomertry which is written in a specific part of vertex/index buffers.
// The class is used to constracut various direct & indirect glDrawElenentsXXX commands' params.
class DE_cmd final {
public:

	DE_cmd() noexcept = default;

	DE_cmd(GLuint vao_id, size_t index_count, size_t offset_indices, size_t base_vertex) noexcept
		: _vao_id(vao_id), _index_count(index_count), _offset_indices(offset_indices), _base_vertex(base_vertex)
	{}

	DE_cmd::DE_cmd(GLuint vao_id, size_t index_count, size_t offset_indices, size_t base_vertex,
		size_t instance_count, size_t base_instance) noexcept
		: _vao_id(vao_id), _index_count(index_count), _offset_indices(offset_indices),
		_base_vertex(base_vertex), _instance_count(instance_count), _base_instance(base_instance)
	{
		assert(_vao_id != Invalid::vao_id);
	}


	// The constant that should be added to each instance index while fetching from enabled vertex attributes.
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

	// Returns glDrawElementsBaseVertex compatible params.
	DE_base_vertex_params get_base_vertex_params() const noexcept 
	{
		return DE_base_vertex_params(GL_TRIANGLES, _index_count, GL_UNSIGNED_INT,
			reinterpret_cast<void*>(_offset_indices * sizeof(GL_UNSIGNED_INT)), _base_vertex);
	}

	// Returns glDrawElementsIndirect compatible params.
	DE_indirect_params get_indirect_params() const noexcept
	{
		return DE_indirect_params(_index_count, _instance_count, _offset_indices, 
			_base_vertex, _base_instance);
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
	size_t offset_indices() const
	{
		return _offset_indices;
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
	GLuint _vao_id = Invalid::vao_id;
	size_t _index_count = 0;
	size_t _offset_indices = 0;
	size_t _base_vertex = 0;
	size_t _instance_count = 1;
	size_t _base_instance = 0;
};

// ...
// Static_vertex_spec determines life time of all its resources: vao, vertex/index buffers.
class Static_vertex_spec final {
public:

	Static_vertex_spec(cg::data::Interleaved_vertex_format format, GLuint vao_id, 
		GLuint vertex_buffer_id, GLuint vertex_buffer_binding_index, GLuint index_buffer_id) noexcept;

	Static_vertex_spec(const Static_vertex_spec& spec) = delete;

	Static_vertex_spec(Static_vertex_spec&& spec) = delete;

	~Static_vertex_spec() noexcept;


	cg::data::Interleaved_vertex_format format() const noexcept
	{
		return _format;
	}

	GLuint index_buffer_id() const noexcept
	{
		return _index_buffer_id;
	}

	GLuint vao_id() const noexcept
	{
		return _vao_id;
	}

	GLuint vertex_buffer_id() const noexcept
	{
		return _vertex_buffer_id;
	}

	GLuint vertex_buffer_binding_index() const noexcept
	{
		return _vertex_buffer_binding_index;
	}

private:

	GLuint _vao_id = Invalid::vao_id;
	cg::data::Interleaved_vertex_format _format;
	GLuint _vertex_buffer_id = Invalid::buffer_id;
	GLuint _vertex_buffer_binding_index;
	GLuint _index_buffer_id = Invalid::buffer_id;
};

class Static_vertex_spec_builder final {
public:

	// Reserves memory for vertex buffer and index buffer data.
	// The allocated memory will be reused by evry vertex specification building process.
	explicit Static_vertex_spec_builder(size_t vertex_buffer_capacity = 64, size_t index_buffer_capacity = 256);

	// Start building process.
	// Params:
	// -	attribs: Describes required vertex attributes.
	// -	vertex_bytes_limit: The memory threshold for the vertex buffer.
	void begin(cg::data::Vertex_attribs attribs, size_t vertex_limit_bytes);

	// Ends the building process and returns a Static_vertex_spec object 
	// that manages internal OpenGL resources.
	// Returns: a spec object which is responsible for internal vao and buffers.
	std::unique_ptr<Static_vertex_spec> end(const Vertex_attrib_layout& attrib_layout, bool unbind_vao = false);

	// Returns true if the building process has been started.
	// The process is considered started between begin() and end() calls.
	bool building_process() const noexcept
	{
		return _vao_id != Invalid::vao_id;
	}

	DE_cmd push_back(const cg::data::Interleaved_mesh_data& mesh_data);

private:
	std::vector<float> _vertex_data;
	std::vector<uint32_t> _index_data;
	// The following fields are related to the vertex specification building process.
	// The fields are reset every begin() call
	GLuint _vao_id = Invalid::vao_id;
	cg::data::Interleaved_vertex_format _format;
	size_t _vertex_limit_bytes;
	size_t _offset_indices;
	size_t _base_vertex;
};


inline bool operator==(const DE_cmd& lhs, const DE_cmd& rhs) noexcept
{
	return (lhs._vao_id == rhs._vao_id)
		&& (lhs._index_count == rhs._index_count)
		&& (lhs._offset_indices == rhs._offset_indices)
		&& (lhs._base_vertex == rhs._base_vertex)
		&& (lhs._instance_count == rhs._instance_count)
		&& (lhs._base_instance == rhs._base_instance);
}

inline bool operator!=(const DE_cmd& lhs, const DE_cmd& rhs) noexcept
{
	return !(lhs == rhs);
}

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

inline bool operator==(const DE_indirect_params& lhs, const DE_indirect_params& rhs) noexcept
{
	return (lhs.index_count == rhs.index_count)
		&& (lhs.instance_count == rhs.instance_count)
		&& (lhs.offset_indices == rhs.offset_indices)
		&& (lhs.base_vertex == rhs.base_vertex)
		&& (lhs.base_instance == rhs.base_instance);
}

inline bool operator!=(const DE_indirect_params& lhs, const DE_indirect_params& rhs) noexcept
{
	return !(lhs == rhs);
}

inline std::ostream& operator<<(std::ostream& out, const DE_cmd& cmd)
{
	out << "DE_cmd(" << cmd._vao_id << ", " << cmd._index_count << ", "
		<< cmd._offset_indices << ", " << cmd._base_vertex << ", "
		<< cmd._instance_count << ", " << cmd._base_instance << ')';
	return out;
}

inline std::wostream& operator<<(std::wostream& out, const DE_cmd& cmd)
{
	out << "DE_cmd(" << cmd._vao_id << ", " << cmd._index_count << ", "
		<< cmd._offset_indices << ", " << cmd._base_vertex << ", "
		<< cmd._instance_count << ", " << cmd._base_instance << ')';
	return out;
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

inline std::ostream& operator<<(std::ostream& out, const DE_indirect_params& p)
{
	out << "DE_indirect_params(" << p.index_count << ", " << p.instance_count << ", "
		<< p.offset_indices << ", " << p.base_vertex << ", " << p.base_instance << ')';
	return out;
}

inline std::wostream& operator<<(std::wostream& out, const DE_indirect_params& p)
{
	out << "DE_indirect_params(" << p.index_count << ", " << p.instance_count << ", "
		<< p.offset_indices << ", " << p.base_vertex << ", " << p.base_instance << ')';
	return out;
}

// Convenient function to call the glDrawElementsBaseVertex func.
inline void draw_elements_base_vertex(const DE_base_vertex_params& p) noexcept
{
	glDrawElementsBaseVertex(p.mode, p.index_count, p.index_type, p.offset_bytes, p.base_vertex);
}

// Convenient function to call the glDrawElementsBaseVertex func.
inline void draw_elements_base_vertex(const DE_cmd& cmd) noexcept
{
	draw_elements_base_vertex(cmd.get_base_vertex_params());
}

} // namespace opengl
} // namespace cg

#endif // CG_OPENGL_VERTEX_SPEC_H_
