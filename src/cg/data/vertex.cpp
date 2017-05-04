#include "cg/data/vertex.h"


namespace cg {
namespace data {

// ----- Vertex_interleaved_format_desc -----

Vertex_interleaved_format_desc::Vertex_interleaved_format_desc(vertex_attribs attribs) noexcept
	: attribs(attribs)
{
	using Format_p = Vertex_interleaved_format<vertex_attribs::p>;
	using Format_p_n = Vertex_interleaved_format<vertex_attribs::p_n>;
	using Format_p_n_tc = Vertex_interleaved_format<vertex_attribs::p_n_tc>;
	using Format_p_tc = Vertex_interleaved_format<vertex_attribs::p_tc>;
	using Format_p_n_tc_ts = Vertex_interleaved_format<vertex_attribs::p_n_tc_ts>;

	if (attribs == vertex_attribs::p) {
		position_component_count =		Format_p::position_component_count;
		position_byte_count =			Format_p::position_byte_count;
		position_byte_offset =			Format_p::position_byte_offset;
		vertex_component_count =		Format_p::vertex_component_count;
		vertex_byte_count =				Format_p::vertex_byte_count;
	}
	else if (attribs == vertex_attribs::p_n) {
		position_component_count =		Format_p_n::position_component_count;
		position_byte_count =			Format_p_n::position_byte_count;
		position_byte_offset =			Format_p_n::position_byte_offset;
		normal_component_count =		Format_p_n::normal_component_count;
		normal_byte_count =				Format_p_n::normal_byte_count;
		normal_byte_offset =			Format_p_n::normal_byte_offset;
		vertex_component_count =		Format_p_n::vertex_component_count;
		vertex_byte_count =				Format_p_n::vertex_byte_count;
	}
	else if (attribs == vertex_attribs::p_n_tc) {
		position_component_count =		Format_p_n_tc::position_component_count;
		position_byte_count =			Format_p_n_tc::position_byte_count;
		position_byte_offset =			Format_p_n_tc::position_byte_offset;
		normal_component_count =		Format_p_n_tc::normal_component_count;
		normal_byte_count =				Format_p_n_tc::normal_byte_count;
		normal_byte_offset =			Format_p_n_tc::normal_byte_offset;
		tex_coord_component_count =		Format_p_n_tc::tex_coord_component_count;
		tex_coord_byte_count =			Format_p_n_tc::tex_coord_byte_count;
		tex_coord_byte_offset =			Format_p_n_tc::tex_coord_byte_offset;
		vertex_component_count =		Format_p_n_tc::vertex_component_count;
		vertex_byte_count =				Format_p_n_tc::vertex_byte_count;
	}
	else if (attribs == vertex_attribs::p_tc) {
		position_component_count =		Format_p_tc::position_component_count;
		position_byte_count =			Format_p_tc::position_byte_count;
		position_byte_offset =			Format_p_tc::position_byte_offset;
		tex_coord_component_count =		Format_p_tc::tex_coord_component_count;
		tex_coord_byte_count =			Format_p_tc::tex_coord_byte_count;
		tex_coord_byte_offset =			Format_p_tc::tex_coord_byte_offset;
		vertex_component_count =		Format_p_tc::vertex_component_count;
		vertex_byte_count =				Format_p_tc::vertex_byte_count;
	}
	else if (attribs == vertex_attribs::p_n_tc_ts) {
		position_component_count =		Format_p_n_tc_ts::position_component_count;
		position_byte_count =			Format_p_n_tc_ts::position_byte_count;
		position_byte_offset =			Format_p_n_tc_ts::position_byte_offset;
		normal_component_count =		Format_p_n_tc_ts::normal_component_count;
		normal_byte_count =				Format_p_n_tc_ts::normal_byte_count;
		normal_byte_offset =			Format_p_n_tc_ts::normal_byte_offset;
		tex_coord_component_count =		Format_p_n_tc_ts::tex_coord_component_count;
		tex_coord_byte_count =			Format_p_n_tc_ts::tex_coord_byte_count;
		tex_coord_byte_offset =			Format_p_n_tc_ts::tex_coord_byte_offset;
		tangent_space_component_count = Format_p_n_tc_ts::tangent_space_component_count;
		tangent_space_byte_count =		Format_p_n_tc_ts::tangent_space_byte_count;
		tangent_space_byte_offset =		Format_p_n_tc_ts::tangent_space_byte_offset;
		vertex_component_count =		Format_p_n_tc_ts::vertex_component_count;
		vertex_byte_count =				Format_p_n_tc_ts::vertex_byte_count;
	}
}

// ----- funcs -----

std::ostream& operator<<(std::ostream& out, const vertex_attribs& attribs)
{
	out << "Vertex_attribs(";

	switch (attribs) {
		case vertex_attribs::p:
			out << 'p';
			break;

		case vertex_attribs::p_n:
			out << "p_n";
			break;

		case vertex_attribs::p_n_tc:
			out << "p_n_tc";
			break;

		case vertex_attribs::p_tc:
			out << "p_tc";
			break;

		case vertex_attribs::p_n_tc_ts:
			out << "p_n_tc_ts";
			break;
	}

	out << ")";
	return out;
}

std::wostream& operator<<(std::wostream& out, const vertex_attribs& attribs)
{
	out << "Vertex_attribs(";

	switch (attribs) {
		case vertex_attribs::p:
			out << 'p';
			break;

		case vertex_attribs::p_n:
			out << "p_n";
			break;

		case vertex_attribs::p_n_tc:
			out << "p_n_tc";
			break;

		case vertex_attribs::p_tc:
			out << "p_tc";
			break;

		case vertex_attribs::p_n_tc_ts:
			out << "p_n_tc_ts";
			break;
	}

	out << ")";
	return out;
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
	float h = (dot(bitangent, cross(normal, t)) >= 0.0f) ? 1.0f : -1.0f;

	return float4(t, h);
}


bool is_superset_of(vertex_attribs superset, vertex_attribs subset) noexcept
{
	switch (superset) {
		default: assert(false); return false;

		case vertex_attribs::p:			return subset == vertex_attribs::p;

		case vertex_attribs::p_n:		return subset == vertex_attribs::p
											|| subset == vertex_attribs::p_n;

		case vertex_attribs::p_tc:		return subset == vertex_attribs::p
											|| subset == vertex_attribs::p_tc;

		case vertex_attribs::p_n_tc:	return subset != vertex_attribs::p_n_tc_ts;

		case vertex_attribs::p_n_tc_ts: return true;
	}
}
	
} // namespace data
} // namespace cg
