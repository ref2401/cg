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

void Interleaved_mesh_data::push_back_vertex(const Vertex& v)
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

	if (has_tangent_h(_format.attribs)) {
		data_buffer[offset]		= v.tangent_h.x;
		data_buffer[offset + 1] = v.tangent_h.y;
		data_buffer[offset + 2] = v.tangent_h.z;
		data_buffer[offset + 3] = v.tangent_h.w;
		offset += 4;
	}

	_data.insert(_data.end(), data_buffer, data_buffer + offset);
}

void Interleaved_mesh_data::push_back_vertices(const Vertex& v0, const Vertex& v1, const Vertex& v2)
{
	push_back_vertex(v0);
	push_back_vertex(v1);
	push_back_vertex(v2);
}

// ----- funcs -----

float4 compute_tangent_h(const Vertex& v0, const Vertex& v1, const Vertex& v2)
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
	float3 t_prj_n = normal * dot(t, normal);
	float3 tangent = normalize(t - t_prj_n);

	return float4(tangent, handedness);
}

std::ostream& operator<<(std::ostream& out, const Vertex_attribs& attribs)
{
	out << "Vertex_attribs(";

	if (attribs == Vertex_attribs::none) {
		out << "none";
	}
	else {
		if (has_position(attribs)) out << "position";
		if (has_normal(attribs)) out << "normal ";
		if (has_tex_coord(attribs)) out << "tex_coord ";
		if (has_tangent_h(attribs)) out << "tangent_h";
	}

	out << ")";
	return out;
}

std::wostream& operator<<(std::wostream& out, const Vertex_attribs& attribs)
{
	out << "Vertex_attribs(";

	if (attribs == Vertex_attribs::none) {
		out << "none";
	}
	else {
		if (has_position(attribs)) out << "position";
		if (has_normal(attribs)) out << "normal ";
		if (has_tex_coord(attribs)) out << "tex_coord ";
		if (has_tangent_h(attribs)) out << "tangent_h";
	}

	out << ")";
	return out;
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

std::ostream& operator<<(std::ostream& out, const Vertex& v)
{
	out << "Vertex(p: " << v.position
		<< "; n: " << v.normal
		<< "; tc: " << v.tex_coord
		<< "; tan_h: " << v.tangent_h << ")";
	return out;
}

std::wostream& operator<<(std::wostream& out, const Vertex& v)
{
	out << "Vertex(p: " << v.position
		<< "; n: " << v.normal
		<< "; tc: " << v.tex_coord
		<< "; tan_h: " << v.tangent_h << ")";
	return out;
}

} // namesapce data
} // namespace cg