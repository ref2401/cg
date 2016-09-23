#include "cg/opengl/vertex_spec.h"

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
namespace opengl {

Static_vertex_spec_builder::Static_vertex_spec_builder(
	size_t vertex_buffer_capacity, size_t index_buffer_capacity)
	: _format(Interleaved_vertex_format(Vertex_attribs::position))
{
	_vertex_data.reserve(vertex_buffer_capacity);
	_index_data.reserve(index_buffer_capacity);
}

void Static_vertex_spec_builder::begin(Vertex_attribs attribs, size_t vertex_limit_bytes)
{
	_format = Interleaved_vertex_format(attribs);
	_vertex_limit_bytes = vertex_limit_bytes;
	_vertex_data.reserve(_vertex_limit_bytes / _format.byte_count());
	_offset_indices = 0;
	_base_vertex = 0;

	glCreateVertexArrays(1, &_vao_id);
}

void Static_vertex_spec_builder::end()
{
	assert(building_process());
	glBindVertexArray(_vao_id);

	GLuint ids[2]; // vertex_buffer_id, index_buffer_id;
	glCreateBuffers(2, ids);
	glNamedBufferStorage(ids[0], _vertex_data.size() * sizeof(float), _vertex_data.data(), 0);
	glNamedBufferStorage(ids[0], _index_data.size() * sizeof(uint32_t), _index_data.data(), 0);


	// Vertex_attrib_layout
	/*if (has_position(_format.attribs)) {
	}*/
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
} // namespace cg