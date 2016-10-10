#ifndef CG_MATH_TRANSFORM_H_
#define CG_MATH_TRANSFORM_H_

#include <cassert>
#include <ostream>
#include <type_traits>
#include "cg/math/float3.h"
#include "cg/math/float4.h"
#include "cg/math/mat3.h"
#include "cg/math/mat4.h"
#include "cg/math/quat.h"
#include "cg/math/utility.h"


namespace cg {

namespace internal {

	// Checks whether the given type T is float3 or float4 vector.
	template<typename T>
	struct is_3d_float_vector : std::false_type {};
	template<> struct is_3d_float_vector<float3> : std::true_type {};
	template<> struct is_3d_float_vector<float4> : std::true_type {};

	// Checks whether the given type T is mat3 or mat4 vector.
	template<typename T>
	struct is_matrix : std::false_type {};
	template<> struct is_matrix<mat3> : std::true_type {};
	template<> struct is_matrix<mat4> : std::true_type {};

} // namespace internal


constexpr float pi = 3.1415926535897932384626433832f;
constexpr float pi_2 = pi / 2.f;
constexpr float pi_3 = pi / 3.f;
constexpr float pi_4 = pi / 4.f;
constexpr float pi_8 = pi / 8.f;
constexpr float pi_16 = pi / 16.f;
constexpr float pi_32 = pi / 32.f;
constexpr float pi_64 = pi / 64.f;
constexpr float pi_128 = pi / 128.f;

// Represents camera position, and orientation in the word.
struct Viewpoint final {
	Viewpoint() noexcept = default;

	Viewpoint(float3 position, float3 target, float3 up = float3::unit_y) noexcept;


	// Direction in which this viewpoint(camera) points to.
	float3 forward() const noexcept
	{
		return normalize(target - position);
	}

	// Returns distance bitween position and target.
	float distance() const noexcept
	{
		return cg::len(target - position);
	}

	// Calculates a matrix that cam be used to transform from world space to this view space.
	// Make sure that up is normalized before calling this method.
	mat4 view_matrix() const noexcept;


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

// Create a quaternion from the axis-angle respresentation.
//	Params:
//		axis = a unit vector indicates direction of a rotation axis.
//		angle = (In radians) describes the magnitude of the rotation about the axis.
quat from_axis_angle_rotation(const float3& axis, float angle) noexcept;

// Construct a unit quaternion from the specified rotation matrix.
// In the case of M is mat4 translation and perspective components are ignored.
template<typename TMat>
quat from_rotation_matrix(const TMat& m) noexcept
{
	static_assert(cg::internal::is_matrix<TMat>::value, "TMat must be a matrix (mat3 or mat4).");

	if (m == TMat::zero) return quat::zero;
	assert(is_orthogonal(m));

	// NOTE(ref2401): see Ken Shoemake: Quaternions.
	quat res;
	float u = m.m00 + m.m11 + m.m22;

	if (u >= 0.f) {
		float s = std::sqrt(u + 1.f);
		res.a = 0.5f * s;

		s = 0.5f / s;
		res.x = (m.m21 - m.m12) * s;
		res.y = (m.m02 - m.m20) * s;
		res.z = (m.m10 - m.m01) * s;
	}
	else if (m.m00 > m.m11 && m.m00 > m.m22) {
		float s = sqrt(m.m00 - m.m11 - m.m22 + 1.f);
		res.x = 0.5f * s;

		s = 0.5f / s;
		res.y = (m.m10 + m.m01) * s;
		res.z = (m.m02 + m.m20) * s;
		res.a = (m.m21 - m.m12) * s;
	}
	else if (m.m11 > m.m22) {
		float s = sqrt(m.m11 - m.m00 - m.m22 + 1.f);
		res.y = 0.5f * s;

		s = 0.5f / s;
		res.x = (m.m10 + m.m01) * s;
		res.z = (m.m21 + m.m12) * s;
		res.a = (m.m02 - m.m20) * s;
	}
	else {
		float s = std::sqrt(m.m22 - m.m00 - m.m11 + 1.f);
		res.z = 0.5f * s;

		s = 0.5f / s;
		res.a = (m.m10 - m.m01) * s;
		res.x = (m.m02 + m.m20) * s;
		res.y = (m.m21 + m.m12) * s;
	}

	return res;
}

// Linearly interpolates between two viewpoints.
Viewpoint lerp(const Viewpoint& l, const Viewpoint& r, float factor) noexcept;

// Creates an OpenGL compatible orthographic projection matrix. 
// right = -left, top = -bottom, near < far.
mat4 orthographic_matrix(float width, float height, float near_z, float far_z) noexcept;

// Creates an OpenGL compatible orthographic projection matrix.
// left < right, bottom < top, near < far.
mat4 orthographic_matrix(float left, float right, float bottom, float top, float near_z, float far_z) noexcept;

// Computes an OpenGL compatible projection matrix for general frustum.
// left < right, bottom < top, 0 < near < far.
mat4 perspective_matrix(float left, float right, float bottom, float top, float near_z, float far_z) noexcept;

// Computes an OpenGL compatible symmetric perspective projection matrix based on a field of view.
// 0 < vert_Fov < cg::pi, 0 < near < far.
//	Params:
//		vert_fov = Vertical field of view in radians.
//		wh_ratio = The ratio of the width to the height of the near clipping plane.
//		near = the distance between a viewer and the near clipping plane.
//		far = the distance between a viewer and the far clipping plane.
mat4 perspective_matrix(float vert_fov, float wh_ratio, float near_z, float far_z) noexcept;

// Returns the position component of the specified matrix.
inline float3 position(const mat4& m) noexcept
{
	return float3(m.m03, m.m13, m.m23);
}

// Sets the position component of the specifiend matrix.
inline void set_position(mat4& m, const float3& p) noexcept
{
	m.m03 = p.x;
	m.m13 = p.y;
	m.m23 = p.z;
}

// Rotate position p by a quaternion q.
// Usually rotation q is specified as a rotation around an arbitrary axis by some angle.
inline float3 rotate(const quat& q, const float3& p) noexcept
{
	assert(is_normalized(q));

	quat pq = quat(p.x, p.y, p.z, 1.0f);
	quat res = q * pq * conjugate(q);
	return float3(res.x, res.y, res.z);
}

// Constructs rotation matrix from (possibly non-unit) quaternion.
template<typename TRetMat>
TRetMat rotation_matrix(const quat& q) noexcept
{
	static_assert(cg::internal::is_matrix<TRetMat>::value, "TRetMat must be a matrix (mat3, mat4).");

	float l = len(q);
	if (approx_equal(l, 0.f)) return TRetMat::zero;

	float s = 2.f / l;
	float xx = q.x * q.x;
	float yy = q.y * q.y;
	float zz = q.z * q.z;
	float ax = q.a * q.x;
	float ay = q.a * q.y;
	float az = q.a * q.z;
	float xy = q.x * q.y;
	float xz = q.x * q.z;
	float yz = q.y * q.z;

	TRetMat rot = TRetMat::identity;
	rot.m00 = 1.f - s * (yy + zz);
	rot.m01 = s * (xy - az);
	rot.m02 = s * (xz + ay);

	rot.m10 = s * (xy + az);
	rot.m11 = 1.f - s * (xx + zz);
	rot.m12 = s * (yz - ax);

	rot.m20 = s * (xz - ay);
	rot.m21 = s * (yz + ax);
	rot.m22 = 1.f - s * (xx + yy);

	return rot;
}

// Composes a rotatiom matrix that rotates a vector by angle about an arbitrary axis.
// The rotation is conter-clockwise.
//	Params:
//		axis = a unit vector indicates the direction of a rotation axis.
//		angle = describes the magnitude in radians of the rotation about the axis.
template<typename TRetMat>
TRetMat rotation_matrix(const float3& axis, float angle) noexcept
{
	static_assert(cg::internal::is_matrix<TRetMat>::value, "TRetMat must be a matrix (mat3, mat4).");
	assert(is_normalized(axis));

	float cos_a = std::cos(angle);
	float sin_a = std::sin(angle);
	float one_minus_cos_a = (1.f - cos_a);
	float xx = axis.x * axis.x;
	float xy = axis.x * axis.y;
	float xz = axis.x * axis.z;
	float yy = axis.y * axis.y;
	float yz = axis.y * axis.z;
	float zz = axis.z * axis.z;

	TRetMat rot = TRetMat::identity;
	rot.m00 = cos_a + one_minus_cos_a * xx;
	rot.m01 = one_minus_cos_a * xy - axis.z * sin_a;
	rot.m02 = one_minus_cos_a * xz + axis.y * sin_a;

	rot.m10 = one_minus_cos_a * xy + axis.z * sin_a;
	rot.m11 = cos_a + one_minus_cos_a * yy;
	rot.m12 = one_minus_cos_a * yz - axis.x * sin_a;

	rot.m20 = one_minus_cos_a * xz - axis.y * sin_a;
	rot.m21 = one_minus_cos_a * yz + axis.x * sin_a;
	rot.m22 = cos_a + one_minus_cos_a * zz;

	return rot;
}

// Composes a look at rotation matrix. Translation component is not set.
// Use tr_matrix to consturct a look at rotation and translation to position chain.
//	Params:
//		position = is eye/object position.
//		target = point of interest.v The point we want to look at.
//		up = the direction that is considered to be upward.
template<typename TRetMat>
TRetMat rotation_matrix(const float3& position, const float3& target, const float3& up = float3::unit_y) noexcept
{
	assert(position != target);
	assert(is_normalized(up));

	float3 forward = normalize(target - position);
	float3 right = normalize(cross(up, forward));
	float3 new_up = normalize(cross(forward, right));

	TRetMat r = TRetMat::identity;
	r.set_ox(right);
	r.set_oy(new_up);
	r.set_oz(forward);

	return r;
}

// Composes a rotatiom matrix about ox axis.
//	Params:
//		angle = describes the magnitude in radians of the rotation about ox.
template<typename TRetMat>
TRetMat rotation_matrix_ox(float angle) noexcept
{
	static_assert(cg::internal::is_matrix<TRetMat>::value, "TRetMat must be a matrix (mat3, mat4).");

	if (approx_equal(angle, 0.f)) return TRetMat::identity;

	float cos_a = std::cos(angle);
	float sin_a = std::sin(angle);

	TRetMat r = TRetMat::identity;
	r.m11 = cos_a;
	r.m21 = sin_a;
	r.m12 = -sin_a;
	r.m22 = cos_a;

	return r;
}

// Composes a rotatiom matrix about oy axis.
//	Params:
//		angle = describes the magnitude in radians of the rotation about oy.
template<typename TRetMat>
TRetMat rotation_matrix_oy(float angle) noexcept
{
	static_assert(cg::internal::is_matrix<TRetMat>::value, "TRetMat must be a matrix (mat3, mat4).");

	if (approx_equal(angle, 0.f)) return TRetMat::identity;

	float cos_a = std::cos(angle);
	float sin_a = std::sin(angle);

	TRetMat r = TRetMat::identity;
	r.m00 = cos_a;
	r.m20 = -sin_a;
	r.m02 = sin_a;
	r.m22 = cos_a;

	return r;
}

// Composes a rotatiom matrix about oz axis.
//	Params:
//		angle = describes the magnitude in radians of the rotation about oz.
template<typename TRetMat>
TRetMat rotation_matrix_oz(float angle) noexcept
{
	static_assert(cg::internal::is_matrix<TRetMat>::value, "TRetMat must be a matrix (mat3, mat4).");

	if (approx_equal(angle, 0.f)) return TRetMat::identity;

	float cos_a = std::cos(angle);
	float sin_a = std::sin(angle);

	TRetMat r = TRetMat::identity;
	r.m00 = cos_a;
	r.m10 = sin_a;
	r.m01 = -sin_a;
	r.m11 = cos_a;

	return r;
}

// Returns a matrix which can be used to scale vectors s.
template<typename TRetMat>
TRetMat scale_matrix(const float3& s) noexcept
{
	assert(greater_than(s, 0));

	TRetMat m = TRetMat::identity;
	m.m00 = s.x;
	m.m11 = s.y;
	m.m22 = s.z;
	return m;
}

// Returns a matrix that is a concatenation of translation by p and rotation by q.
// The result is equal to translation_matrix(p) * rotation_matrix(q).
inline mat4 tr_matrix(const float3& p, const quat& q) noexcept
{
	mat4 m = rotation_matrix<mat4>(q);
	set_position(m, p);
	return m;
}

// Returns a matrix that is a concatentation of traslation by p and a look at rotation.
inline mat4 tr_matrix(const float3& position, const float3& target, const float3& up = float3::unit_y) noexcept
{
	mat4 m = rotation_matrix<mat4>(position, target, up);
	set_position(m, position);
	return m;
}

// Returns a matrix which can be used to translate vectors to the position p.
inline mat4 translation_matrix(const float3& p) noexcept
{
	mat4 m = mat4::identity;
	set_position(m, p);
	return m;
}

// Return a matrix that is a concatenation of translation by p, rotation by q and scale by s.
// The result is equal to translation_matrix(p) * rotation_matrix(q) * scale_matrix(s).
inline mat4 trs_matrix(const float3& p, const quat& q, const float3& s) noexcept
{
	return tr_matrix(p, q) * scale_matrix<mat4>(s);
}

// Returns a matrix that is a concatentation of traslation by p and scale by s.
inline mat4 ts_matrix(const float3& p, const float3& s) noexcept
{
	mat4 m = scale_matrix<mat4>(s);
	set_position(m, p);
	return m;
}

// Composes a matrix that cam be used to transform from world space to view space.
//	Params:
//		position = an origin, where eye(camera) is situated.
//		target = a point of interest.
//		up = the direction that is considered to be upward.
mat4 view_matrix(const float3& position, const float3& target, const float3& up = float3::unit_y) noexcept;

// Linearly interpolates between two viewpoints and calculates a view matrix using the interpolation result.
inline mat4 view_matrix(const Viewpoint& l, const Viewpoint& r, float lerp_factor) noexcept
{
	assert(0.0f <= lerp_factor && lerp_factor <= 1.0f);

	auto vp = lerp(l, r, lerp_factor);
	vp.up = normalize(vp.up);
	return vp.view_matrix();
}

} // namespace cg

#endif // CG_MATH_TRANSFORM_H_
