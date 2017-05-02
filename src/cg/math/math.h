#ifndef CG_MATH_MATH_H_
#define CG_MATH_MATH_H_

#include "cg/math/matrix.h"
#include "cg/math/transform.h"
#include "cg/math/utility.h"
#include "cg/math/vector.h"

#include "math/math.h"


namespace cg {

// Represents camera position, and orientation in the word.
struct Viewpoint final {
	Viewpoint() noexcept = default;

	Viewpoint(float3 position, float3 target, float3 up = float3::unit_y) noexcept
		: position(position), target(target), up(up)
	{}


	float3 position;
	float3 target;
	float3 up;
};


inline bool operator==(const Viewpoint& l, const Viewpoint& r) noexcept
{
	return (l.position == r.position)
		&& (l.target == r.target)
		&& (l.up == r.up);
}

inline bool operator!=(const Viewpoint& l, const Viewpoint& r) noexcept
{
	return !(l == r);
}

std::ostream& operator<<(std::ostream& out, const Viewpoint& vp);

std::wostream& operator<<(std::wostream& out, const Viewpoint& vp);

// Returns distance bitween position and target.
inline float distance(const Viewpoint& vp) noexcept
{
	return cg::len(vp.target - vp.position);
}

// Direction in which this viewpoint(camera) points to.
inline float3 forward(const Viewpoint& vp) noexcept
{
	return normalize(vp.target - vp.position);
}

// Linearly interpolates between two viewpoints.
Viewpoint lerp(const Viewpoint& l, const Viewpoint& r, float factor) noexcept;

// Calculates a matrix that cam be used to transform from world space to this view space.
// Make sure that up is normalized before calling this method.
inline mat4 view_matrix(const Viewpoint& vp) noexcept
{
	return cg::view_matrix(vp.position, vp.target, vp.up);
}

// Linearly interpolates between two viewpoints and calculates a view matrix using the interpolation result.
inline mat4 view_matrix(const Viewpoint& l, const Viewpoint& r, float lerp_factor) noexcept
{
	assert(0.0f <= lerp_factor && lerp_factor <= 1.0f);

	Viewpoint vp = lerp(l, r, lerp_factor);
	vp.up = normalize(vp.up);
	return view_matrix(vp);
}

} // namespace cg

#endif // CG_MATH_MATH_H_
