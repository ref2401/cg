#include "technique/deferred_lighting/vertex_spec.h"

#include <utility>
#include "cg/base/base.h"

using cg::data::Model_geometry_data;
using cg::data::Model_mesh_info;
using cg::data::vertex_attribs;
using namespace cg::rnd::opengl;


namespace deferred_lighting {

// ----- Static_vertex_spec -----

Static_vertex_spec::Static_vertex_spec(GLuint vao_id, GLuint vertex_buffer_binding_index,
	cg::rnd::opengl::Buffer_immut vertex_buffer,
	cg::rnd::opengl::Buffer_immut index_buffer) noexcept
	: _vao_id(vao_id), 
	_vertex_buffer_binding_index(vertex_buffer_binding_index),
	_vertex_buffer(std::move(vertex_buffer)),
	_index_buffer(std::move(index_buffer))
{
	assert(vao_id != Blank::vao_id);
}

Static_vertex_spec::Static_vertex_spec(Static_vertex_spec&& spec) noexcept 
	: _vao_id(spec._vao_id),
	_vertex_buffer_binding_index(spec._vertex_buffer_binding_index),
	_vertex_buffer(std::move(spec._vertex_buffer)),
	_index_buffer(std::move(spec._index_buffer))
{
	spec._vao_id = Blank::vao_id;
	spec._vertex_buffer_binding_index = 0;
}

Static_vertex_spec::~Static_vertex_spec() noexcept
{
	dispose();

	_vao_id = Blank::vao_id;
	_vertex_buffer_binding_index = 0;
}

Static_vertex_spec& Static_vertex_spec::operator=(Static_vertex_spec&& spec) noexcept
{
	if (this == &spec) return *this;

	dispose();

	_vao_id = spec._vao_id;
	_vertex_buffer_binding_index = spec._vertex_buffer_binding_index;
	_vertex_buffer = std::move(spec._vertex_buffer);
	_index_buffer = std::move(spec._index_buffer);

	spec._vao_id = Blank::vao_id;
	spec._vertex_buffer_binding_index = 0;

	return *this;
}

void Static_vertex_spec::dispose() noexcept
{
	if (_vao_id != Blank::vao_id) {
		glDeleteVertexArrays(1, &_vao_id);
		_vao_id = Blank::vao_id;
	}
}

// ----- Static_vertex_spec_builder -----

Static_vertex_spec_builder::Static_vertex_spec_builder(
	size_t vertex_buffer_capacity, size_t index_buffer_capacity)
{
	_vertex_data.reserve(vertex_buffer_capacity);
	_index_data.reserve(index_buffer_capacity);
}

Static_vertex_spec Static_vertex_spec_builder::end(const Vertex_attrib_layout& attrib_layout, bool unbind_vao)
{
	using cg::data::has_normal;
	using cg::data::has_tex_coord;
	using cg::data::has_tangent_space;

	assert(building_process());

	cg::rnd::opengl::Buffer_immut vertex_buffer(0, _vertex_data);
	cg::rnd::opengl::Buffer_immut index_buffer(0, _index_data);

	//GLuint ids[2]; // vertex_buffer_id, index_buffer_id;
	//glCreateBuffers(2, ids);
	//glNamedBufferStorage(ids[0], _vertex_data.size() * sizeof(uint8_t), _vertex_data.data(), 0);
	//glNamedBufferStorage(ids[1], _index_data.size() * sizeof(uint32_t), _index_data.data(), 0);

	GLuint vb_binding_index = 0;
	glBindVertexArray(_vao_id);
	glVertexArrayVertexBuffer(_vao_id, 0, vertex_buffer.id(), vb_binding_index, GLsizei(_format_desc.vertex_byte_count));

	// position
	assert(attrib_layout.position_location != Blank::vertex_attrib_location);
	glEnableVertexArrayAttrib(_vao_id, attrib_layout.position_location);
	glVertexArrayAttribBinding(_vao_id, attrib_layout.position_location, vb_binding_index);
	glVertexArrayAttribFormat(_vao_id, attrib_layout.position_location,
		3, GL_FLOAT, false, GLsizei(_format_desc.position_byte_offset));

	// normal
	if (has_normal(_format_desc.attribs)) {
		assert(attrib_layout.normal_location != Blank::vertex_attrib_location);
		glEnableVertexArrayAttrib(_vao_id, attrib_layout.normal_location);
		glVertexArrayAttribBinding(_vao_id, attrib_layout.normal_location, vb_binding_index);
		glVertexArrayAttribFormat(_vao_id, attrib_layout.normal_location,
			3, GL_FLOAT, false, GLsizei(_format_desc.normal_byte_offset));
	}

	// tex_coord
	if (has_tex_coord(_format_desc.attribs)) {
		assert(attrib_layout.tex_coord_location != Blank::vertex_attrib_location);
		glEnableVertexArrayAttrib(_vao_id, attrib_layout.tex_coord_location);
		glVertexArrayAttribBinding(_vao_id, attrib_layout.tex_coord_location, vb_binding_index);
		glVertexArrayAttribFormat(_vao_id, attrib_layout.tex_coord_location,
			2, GL_FLOAT, false, GLsizei(_format_desc.tex_coord_byte_offset));
	}

	// tangent_h
	if (has_tangent_space(_format_desc.attribs)) {
		assert(attrib_layout.tangent_h_location != Blank::vertex_attrib_location);
		glEnableVertexArrayAttrib(_vao_id, attrib_layout.tangent_h_location);
		glVertexArrayAttribBinding(_vao_id, attrib_layout.tangent_h_location, vb_binding_index);
		glVertexArrayAttribFormat(_vao_id, attrib_layout.tangent_h_location,
			4, GL_FLOAT, false, GLsizei(_format_desc.tangent_space_byte_offset));
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer.id());

	if (unbind_vao)
		glBindVertexArray(Blank::vao_id);

	GLuint vao_id_temp = _vao_id;
	_vao_id = Blank::vao_id; // this ends building process
	return Static_vertex_spec(vao_id_temp, vb_binding_index,
		std::move(vertex_buffer), std::move(index_buffer));
}

} // namespace deferred_lighting
