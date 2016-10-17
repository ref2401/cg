#include "cg/rnd/opengl/vertex_spec.h"

#include "cg/base/base.h"

using cg::data::Interleaved_mesh_data;
using cg::data::Interleaved_vertex_format;
using cg::data::Vertex_attribs;
using cg::data::has_normal;
using cg::data::has_position;
using cg::data::has_tangent_h;
using cg::data::has_tex_coord;
using cg::data::is_superset_of;


namespace cg {
namespace rnd {
namespace opengl {

// ----- Static_vertex_spec

Static_vertex_spec::Static_vertex_spec(Static_vertex_spec&& spec) noexcept :
	_vao_id(spec._vao_id),
	_format(spec._format),
	_vertex_buffer(std::move(spec._vertex_buffer)),
	_vertex_buffer_binding_index(spec._vertex_buffer_binding_index),
	_index_buffer(std::move(spec._index_buffer))
{
	spec._vao_id = Invalid::vao_id;
	spec._format.attribs = Vertex_attribs::none;
	spec._vertex_buffer_binding_index = 0;
}

Static_vertex_spec::Static_vertex_spec(cg::data::Interleaved_vertex_format format, GLuint vao_id,
	GLuint vertex_buffer_id, GLuint vertex_buffer_binding_index, GLuint index_buffer_id) noexcept
:	_format(format), 
	_vao_id(vao_id), 
	_vertex_buffer(vertex_buffer_id), 
	_vertex_buffer_binding_index(vertex_buffer_binding_index), 
	_index_buffer(index_buffer_id)
{
	assert(vao_id != Invalid::vao_id);
	assert(_vertex_buffer.id() != _index_buffer.id());
}

Static_vertex_spec::~Static_vertex_spec() noexcept
{
	dispose();

	_vao_id = Invalid::vao_id;
	_format.attribs = Vertex_attribs::none;
	_vertex_buffer_binding_index = 0;
}

Static_vertex_spec& Static_vertex_spec::operator=(Static_vertex_spec&& spec) noexcept
{
	if (this == &spec) return *this;

	dispose();

	_vao_id = spec._vao_id;
	_format = spec._format;
	_vertex_buffer = std::move(spec._vertex_buffer);
	_vertex_buffer_binding_index = spec._vertex_buffer_binding_index;
	_index_buffer = std::move(spec._index_buffer);

	spec._vao_id = Invalid::vao_id;
	spec._format.attribs = Vertex_attribs::none;
	spec._vertex_buffer_binding_index = 0;

	return *this;
}

void Static_vertex_spec::dispose() noexcept
{
	if (_vao_id != Invalid::vao_id) {
		glDeleteVertexArrays(1, &_vao_id);
		_vao_id = Invalid::vao_id;
	}
}

// ----- Static_vertex_spec_builder

Static_vertex_spec_builder::Static_vertex_spec_builder(
	size_t vertex_buffer_capacity, size_t index_buffer_capacity)
{
	_vertex_data.reserve(vertex_buffer_capacity);
	_index_data.reserve(index_buffer_capacity);
}

void Static_vertex_spec_builder::begin(Vertex_attribs attribs, size_t vertex_limit_bytes)
{
	assert(attribs != Vertex_attribs::none);

	_format = Interleaved_vertex_format(attribs);
	_vertex_limit_bytes = vertex_limit_bytes;
	_vertex_data.reserve(_vertex_limit_bytes / _format.byte_count());
	_offset_indices = 0;
	_base_vertex = 0;

	glCreateVertexArrays(1, &_vao_id);
}

Static_vertex_spec Static_vertex_spec_builder::end(const Vertex_attrib_layout& attrib_layout, bool unbind_vao)
{
	assert(building_process());
	assert(is_superset_of(attrib_layout.attribs(), _format.attribs));
	
	GLuint ids[2]; // vertex_buffer_id, index_buffer_id;
	glCreateBuffers(2, ids);
	glNamedBufferStorage(ids[0], _vertex_data.size() * sizeof(float), _vertex_data.data(), 0);
	glNamedBufferStorage(ids[1], _index_data.size() * sizeof(uint32_t), _index_data.data(), 0);

	GLuint vb_binding_index = 0;
	glBindVertexArray(_vao_id);
	glVertexArrayVertexBuffer(_vao_id, 0, ids[0], vb_binding_index, _format.byte_count());
	// position
	if (has_position(_format.attribs)) {
		assert(attrib_layout.position_location != Invalid::vertex_attrib_location);
		glEnableVertexArrayAttrib(_vao_id, attrib_layout.position_location);
		glVertexArrayAttribBinding(_vao_id, attrib_layout.position_location, vb_binding_index);
		glVertexArrayAttribFormat(_vao_id, attrib_layout.position_location,
			3, GL_FLOAT, false, _format.byte_offset_position());
	}

	// normal
	if (has_normal(_format.attribs)) {
		assert(attrib_layout.normal_location != Invalid::vertex_attrib_location);
		glEnableVertexArrayAttrib(_vao_id, attrib_layout.normal_location);
		glVertexArrayAttribBinding(_vao_id, attrib_layout.normal_location, vb_binding_index);
		glVertexArrayAttribFormat(_vao_id, attrib_layout.normal_location,
			3, GL_FLOAT, false, _format.byte_offset_normal());
	}

	// tex_coord
	if (has_tex_coord(_format.attribs)) {
		assert(attrib_layout.tex_coord_location != Invalid::vertex_attrib_location);
		glEnableVertexArrayAttrib(_vao_id, attrib_layout.tex_coord_location);
		glVertexArrayAttribBinding(_vao_id, attrib_layout.tex_coord_location, vb_binding_index);
		glVertexArrayAttribFormat(_vao_id, attrib_layout.tex_coord_location,
			2, GL_FLOAT, false, _format.byte_offset_tex_coord());
	}

	// tangent_h
	if (has_tangent_h(_format.attribs)) {
		assert(attrib_layout.tangent_h_location != Invalid::vertex_attrib_location);
		glEnableVertexArrayAttrib(_vao_id, attrib_layout.tangent_h_location);
		glVertexArrayAttribBinding(_vao_id, attrib_layout.tangent_h_location, vb_binding_index);
		glVertexArrayAttribFormat(_vao_id, attrib_layout.tangent_h_location,
			4, GL_FLOAT, false, _format.byte_offset_tangent_h());
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ids[1]);

	if (unbind_vao)
		glBindVertexArray(Invalid::vao_id);

	GLuint vao_id_temp = _vao_id;
	_vao_id = Invalid::vao_id; // this ends building process
	return Static_vertex_spec(_format, vao_id_temp, ids[0], vb_binding_index, ids[1]);
}

DE_cmd Static_vertex_spec_builder::push_back(const cg::data::Interleaved_mesh_data& mesh_data)
{
	assert(building_process());
	assert(mesh_data.format() == _format);

	_vertex_data.insert(_vertex_data.cend(), mesh_data.data().cbegin(), mesh_data.data().cend());
	_index_data.insert(_index_data.cend(), mesh_data.indices().cbegin(), mesh_data.indices().cend());
	
	DE_cmd cmd(_vao_id, mesh_data.index_count(), _offset_indices, _base_vertex);
	_offset_indices += mesh_data.index_count();
	_base_vertex += mesh_data.vertex_count();
	
	return cmd;
}

} // namepsace opengl
} // namespace rnd
} // namespace cg