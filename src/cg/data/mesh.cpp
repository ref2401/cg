#include "cg/data/mesh.h"

#include <iterator>
#include <type_traits>


namespace cg {
namespace data {

// ----- Interleaved_mesh_data -----

Interleaved_mesh_data::Interleaved_mesh_data(Vertex_attribs attribs, size_t vertex_count, size_t index_count)
	: _format(attribs)
{
	assert(attribs != Vertex_attribs::none);
	_data.reserve(_format.component_count() * vertex_count);
	_indices.reserve(index_count);
}

void Interleaved_mesh_data::push_back_index(uint32_t i)
{
	_indices.push_back(i);
}

void Interleaved_mesh_data::push_back_indices(uint32_t i0, uint32_t i1, uint32_t i2)
{
	uint32_t index_buffer[3];
	index_buffer[0] = i0;
	index_buffer[1] = i1;
	index_buffer[2] = i2;
	_indices.insert(_indices.end(), index_buffer, index_buffer + std::extent<decltype(index_buffer)>::value);
}

void Interleaved_mesh_data::push_back_vertex(const Vertex_old& v)
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
		data_buffer[offset]		= v.tangent_h.x;
		data_buffer[offset + 1] = v.tangent_h.y;
		data_buffer[offset + 2] = v.tangent_h.z;
		data_buffer[offset + 3] = v.tangent_h.w;
		offset += 4;
	}

	_data.insert(_data.end(), data_buffer, data_buffer + offset);
}

void Interleaved_mesh_data::push_back_vertices(const Vertex_old& v0, const Vertex_old& v1, const Vertex_old& v2)
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

	//_shared_vertices.reserve(vertex_count);
	_vertices.reserve(vertex_count);
	_indices.reserve(index_count);
}

void Mesh_builder::clear() noexcept
{
	//_shared_vertices.clear();
	_vertices.clear();
	_indices.clear();
	_curr_index_counter = 0;
}

void Mesh_builder::push_back_vertex(const Vertex_ts& v)
{
	//auto it = _shared_vertices.find(v);
	//
	//uint32_t index;
	//if (it == _shared_vertices.end()) {
	//	index = static_cast<uint32_t>(_vertices.size());

	//}
	//else 
	//{
	//	index = it->second;
	//	auto& vertex = _vertices[index];

	//	vertex.tangent += v.tangent;
	//	vertex.bitangent += v.bitangent;
	//}

	//_indices
}

void Mesh_builder::push_back_triangle(const Vertex_ts& v0, const Vertex_ts& v1, const Vertex_ts& v2)
{
	push_back_vertex(v0);
	push_back_vertex(v1);
	push_back_vertex(v2);
}

// ----- funcs -----

float4 compute_tangent_h(const Vertex_old& v0, const Vertex_old& v1, const Vertex_old& v2)
{
	float3 pos0 = v0.position;
	float3 pos1 = v1.position;
	float3 pos2 = v2.position;

	float dx1 = pos1.x - pos0.x;
	float dx2 = pos2.x - pos0.x;
	float dy1 = pos1.y - pos0.y;
	float dy2 = pos2.y - pos0.y;
	float dz1 = pos1.z - pos0.z;
	float dz2 = pos2.z - pos0.z;

	float2 tc0 = v0.tex_coord;
	float2 tc1 = v1.tex_coord;
	float2 tc2 = v2.tex_coord;
	float ds1 = tc1.x - tc0.x;
	float ds2 = tc2.x - tc0.x;
	float dt1 = tc1.y - tc0.y;
	float dt2 = tc2.y - tc0.y;

	float r = 1.0f / (ds1 * dt2 - ds2 * dt1);
	float3 t = normalize(r * float3((dt2 * dx1 - dt1 * dx2), (dt2 * dy1 - dt1 * dy2), (dt2 * dz1 - dt1 * dz2)));
	float3 b = normalize(r * float3((ds1 * dx2 - ds2 * dx1), (ds1 * dy2 - ds2 * dy1), (ds1 * dz2 - ds2 * dz1)));
	float3 normal = v0.normal;
	float handedness = (dot(b, cross(normal, t)) > 0.f) ? 1.f : -1.f;

	// Gram-Schmidt orthogonalize.
	// project tangent vector onto normal.
	/*float3 t_prj_n = normal * dot(t, normal);
	float3 tangent = normalize(t - t_prj_n);
	return float4(tangent, handedness);*/
	return float4(t, handedness);
}

std::pair<float3, float3> compute_tangent_bitangent(
	const float3& pos0, const float2& tc0,
	const float3& pos1, const float2& tc1,
	const float3& pos2, const float2& tc2) noexcept
{
	float dx1 = pos1.x - pos0.x;
	float dx2 = pos2.x - pos0.x;
	float dy1 = pos1.y - pos0.y;
	float dy2 = pos2.y - pos0.y;
	float dz1 = pos1.z - pos0.z;
	float dz2 = pos2.z - pos0.z;

	float ds1 = tc1.x - tc0.x;
	float ds2 = tc2.x - tc0.x;
	float dt1 = tc1.y - tc0.y;
	float dt2 = tc2.y - tc0.y;

	float r = 1.0f / (ds1 * dt2 - ds2 * dt1);
	float3 t = normalize(r * float3((dt2 * dx1 - dt1 * dx2), (dt2 * dy1 - dt1 * dy2), (dt2 * dz1 - dt1 * dz2)));
	float3 b = normalize(r * float3((ds1 * dx2 - ds2 * dx1), (ds1 * dy2 - ds2 * dy1), (ds1 * dz2 - ds2 * dz1)));

	return std::make_pair(t, b);
}

float4 compute_tangent_handedness(const float3& tangent, 
	const float3& bitangent, const float3& normal) noexcept
{
	assert(is_normalized(tangent));
	assert(is_normalized(bitangent));
	assert(is_normalized(normal));

	// Gram-Schmidt orthogonalize.
	// project tangent vector onto normal.
	float3 t_prj_n = normal * dot(tangent, normal);
	float3 t = normalize(tangent - t_prj_n);
	float h = (dot(bitangent, cross(normal, t)) > 0.f) ? 1.f : -1.f;

	return float4(t, h);
}


std::ostream& operator<<(std::ostream& out, const Interleaved_vertex_format& fmt)
{
	out << fmt.attribs;
	return out;
}

std::wostream& operator<<(std::wostream& out, const Interleaved_vertex_format& fmt)
{
	out << fmt.attribs;
	return out;
}

} // namesapce data
} // namespace cg