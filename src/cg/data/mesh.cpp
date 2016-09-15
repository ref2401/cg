#include "cg/data/mesh.h"

namespace cg {
namespace data {

// ----- funcs -----

float4 compute_tangent_h(const Vertex& v0, const Vertex& v1, const Vertex& v2)
{
	assert(v0.normal == v1.normal && v1.normal == v2.normal);

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

} // namesapce data
} // namespace cg