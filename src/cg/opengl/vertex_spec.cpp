#include "cg/opengl/vertex_spec.h"

#include "cg/base/base.h"

using cg::data::Interleaved_mesh_data;
using cg::data::Interleaved_vertex_format;
using cg::data::Vertex_attribs;
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

void Static_vertex_spec_builder::begin(Interleaved_vertex_format format, size_t vertex_limit_bytes)
{
	_format = format;
	_vertex_limit_bytes = vertex_limit_bytes;
	_vertex_data.reserve(_vertex_limit_bytes / _format.byte_count());
	_index_count = 0;
	_base_vertex = 0;

	glCreateVertexArrays(1, &_vao_id);
}

DE_cmd Static_vertex_spec_builder::push_back(const cg::data::Interleaved_mesh_data& mesh_data)
{
	assert(is_superset_of(mesh_data.attribs(), _format.attribs));

	// vertex attribs
	// base_vertex
	// offset_index
	return DE_cmd(1, 2, 3, 4);
}

} // namepsace opengl
} // namespace cg