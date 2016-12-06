#ifndef CG_DATA_VERTEX_H_
#define CG_DATA_VERTEX_H_

#include <cassert>
#include <ostream>
#include "cg/base/base.h"
#include "cg/math/math.h"


namespace cg {
namespace data {

// Computes tangent and bitangent vectors for a triangle that is specified by 3 position, tex_coord attribures.
// Returned vectors are not normalized.
std::pair<cg::float3, cg::float3> compute_tangent_bitangent(
	const cg::float3& pos0, const cg::float2& tc0,
	const cg::float3& pos1, const cg::float2& tc1,
	const cg::float3& pos2, const cg::float2& tc2) noexcept;

// Computes orthogonalized tangent and bitangent's handedness.
// All vectors have to be normalized.
// Returns: vector of 4 floats, xyz stands for the tangent & w stands for the handedness value.
cg::float4 compute_tangent_handedness(const cg::float3& tangent,
	const cg::float3& bitangent, const cg::float3& normal) noexcept;


} // namespace data
} // namespace cg

#endif // CG_DATA_VERTEX_H_
