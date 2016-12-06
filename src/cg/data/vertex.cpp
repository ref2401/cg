#include "cg/data/vertex.h"


namespace cg {
namespace data {

// ----- funcs -----

std::ostream& operator<<(std::ostream& out, const Vertex_attribs& attribs)
{
	out << "Vertex_attribs(";

	switch (attribs) {
		case Vertex_attribs::p:
			out << 'p';
			break;

		case Vertex_attribs::p_n:
			out << "p_n";
			break;

		case Vertex_attribs::p_n_tc:
			out << "p_n_tc";
			break;

		case Vertex_attribs::p_tc:
			out << "p_tc";
			break;

		case Vertex_attribs::p_n_tc_ts:
			out << "p_n_tc_ts";
			break;
	}

	out << ")";
	return out;
}

std::wostream& operator<<(std::wostream& out, const Vertex_attribs& attribs)
{
	out << "Vertex_attribs(";

	switch (attribs) {
		case Vertex_attribs::p:
			out << 'p';
			break;

		case Vertex_attribs::p_n:
			out << "p_n";
			break;

		case Vertex_attribs::p_n_tc:
			out << "p_n_tc";
			break;

		case Vertex_attribs::p_tc:
			out << "p_tc";
			break;

		case Vertex_attribs::p_n_tc_ts:
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
	
} // namespace data
} // namespace cg
