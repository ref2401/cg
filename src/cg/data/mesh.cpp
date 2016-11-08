#include "cg/data/mesh.h"

#include <iterator>
#include <type_traits>


namespace cg {
namespace data {

// ----- Interleaved_mesh_data_old -----

Interleaved_mesh_data_old::Interleaved_mesh_data_old(Vertex_attribs attribs, size_t vertex_count, size_t index_count)
	: _format(attribs)
{
	assert(attribs != Vertex_attribs::none);
	_data.reserve(_format.component_count() * vertex_count);
	_indices.reserve(index_count);
}

void Interleaved_mesh_data_old::push_back_index(uint32_t i)
{
	_indices.push_back(i);
}

void Interleaved_mesh_data_old::push_back_indices(uint32_t i0, uint32_t i1, uint32_t i2)
{
	uint32_t index_buffer[3];
	index_buffer[0] = i0;
	index_buffer[1] = i1;
	index_buffer[2] = i2;
	_indices.insert(_indices.end(), index_buffer, index_buffer + std::extent<decltype(index_buffer)>::value);
}

void Interleaved_mesh_data_old::push_back_vertex(const Vertex_old& v)
{
	float data_buffer[12];
	size_t offset = 0;

	if (has_position(_format.attribs)) {
		data_buffer[0] = v.position.x;
		data_buffer[1] = v.position.y;
		data_buffer[2] = v.position.z;
		offset += 3;
	}

	if (has_normal(_format.attribs)) {
		data_buffer[offset]		= v.normal.x;
		data_buffer[offset + 1] = v.normal.y;
		data_buffer[offset + 2] = v.normal.z;
		offset += 3;
	}

	if (has_tex_coord(_format.attribs)) {
		data_buffer[offset]		= v.tex_coord.u;
		data_buffer[offset + 1] = v.tex_coord.v;
		offset += 2;
	}

	if (has_tangent_space(_format.attribs)) {
		data_buffer[offset]		= v.tangent_space.x;
		data_buffer[offset + 1] = v.tangent_space.y;
		data_buffer[offset + 2] = v.tangent_space.z;
		data_buffer[offset + 3] = v.tangent_space.w;
		offset += 4;
	}

	_data.insert(_data.end(), data_buffer, data_buffer + offset);
}

void Interleaved_mesh_data_old::push_back_vertices(const Vertex_old& v0, const Vertex_old& v1, const Vertex_old& v2)
{
	push_back_vertex(v0);
	push_back_vertex(v1);
	push_back_vertex(v2);
}

// ----- Mesh_builder -----

Mesh_builder::Mesh_builder(size_t vertex_count, size_t index_count)
{
	assert(vertex_count > 0);
	assert(index_count > 0);

	_shared_vertices.reserve(vertex_count);
	_vertices.reserve(vertex_count);
	_indices.reserve(index_count);
}

void Mesh_builder::clear() noexcept
{
	_shared_vertices.clear();
	_vertices.clear();
	_indices.clear();
}

void Mesh_builder::push_back_vertex(const Vertex_ts& vertex)
{
	Vertex v_key(vertex.position, vertex.normal, vertex.tex_coord);
	auto it = _shared_vertices.find(v_key);
	
	uint32_t index;
	if (it == _shared_vertices.end()) {
		index = static_cast<uint32_t>(_vertices.size());
		_shared_vertices[v_key] = index;
		_vertices.push_back(vertex);
	}
	else {
		index = it->second;
		auto& v = _vertices[index];

		v.tangent += vertex.tangent;
		v.bitangent += vertex.bitangent;
	}

	_indices.push_back(index);
}

void Mesh_builder::push_back_triangle(const Vertex_ts& v0, const Vertex_ts& v1, const Vertex_ts& v2)
{
	push_back_vertex(v0);
	push_back_vertex(v1);
	push_back_vertex(v2);
}

// ----- funcs -----

std::ostream& operator<<(std::ostream& out, const Interleaved_vertex_format_old& fmt)
{
	out << fmt.attribs;
	return out;
}

std::wostream& operator<<(std::wostream& out, const Interleaved_vertex_format_old& fmt)
{
	out << fmt.attribs;
	return out;
}

} // namesapce data
} // namespace cg