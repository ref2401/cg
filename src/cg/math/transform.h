#ifndef CG_MATH_TRANSFORM_H_
#define CG_MATH_TRANSFORM_H_

#include <cassert>
#include <type_traits>
#include "cg/math/float3.h"
#include "cg/math/float4.h"
#include "cg/math/mat3.h"
#include "cg/math/mat4.h"
#include "cg/math/quat.h"
#include "cg/math/utils.h"


namespace cg {

constexpr float pi = 3.1415926535897932384626433832f;
constexpr float pi_2 = pi / 2.f;
constexpr float pi_4 = pi / 4.f;
constexpr float pi_8 = pi / 8.f;

// Create a quaternion from the axis-angle respresentation.
//	Params:
//		axis = a unit vector indicates direction of a rotation axis.
//		angle = (In radians) describes the magnitude of the rotation about the axis.
quat from_axis_angle_rotation(const float3& axis, float angle);

// Construct a unit quaternion from the specified rotation matrix.
// In the case of M is mat4 translation and perspective components are ignored.
template<typename TMat>
quat from_rotation_matrix(const TMat& m);

// Creates an OpenGL compatible orthographic projection matrix. 
// right = -left, top = -bottom, near < far.
mat4 orthographic_matrix(float width, float height, float near, float far);

// Creates an OpenGL compatible orthographic projection matrix.
// left < right, bottom < top, near < far.
mat4 orthographic_matrix(float left, float right, float bottom, float top, float near, float far);

// Computes an OpenGL compatible projection matrix for general frustum.
// left < right, bottom < top, 0 < near < far.
mat4 perspective_matrix(float left, float right, float bottom, float top, float near, float far);

// Computes an OpenGL compatible symmetric perspective projection matrix based on a field of view.
// 0 < vert_Fov < cg::pi, 0 < near < far.
//	Params:
//		vert_fov = Vertical field of view in radians.
//		wh_ratio = The ratio of the width to the height of the near clipping plane.
//		near = the distance between a viewer and the near clipping plane.
//		far = the distance between a viewer and the far clipping plane.
mat4 perspective_matrix(float vert_fov, float wh_ratio, float near, float far);

// Returns the position component of the specified matrix.
float3 position(const mat4& m);

// Sets the position component of the specifiend matrix.
void set_position(mat4& m, const float3& p);

// Constructs rotation matrix from (possibly non-unit) quaternion.
template<typename TRetMat>
TRetMat rotation_matrix(const quat& q);

// Composes a rotatiom matrix that rotates a vector by angle about an arbitrary axis.
// The rotation is conter-clockwise.
//	Params:
//		axis = a unit vector indicates the direction of a rotation axis.
//		angle = describes the magnitude in radians of the rotation about the axis.
template<typename TRetMat>
TRetMat rotation_matrix(const float3& axis, float angle);

// Composes a look at rotation matrix. Translation component is not set.
// Use tr_matrix to consturct a look at rotation and translation to position chain.
//	Params:
//		position = is eye/object position.
//		target = point of interest.v The point we want to look at.
//		up = the direction that is considered to be upward.
template<typename TRetMat>
TRetMat rotation_matrix(const float3& position, const float3& target, const float3& up = float3::unit_y);

// Composes a rotatiom matrix about ox axis.
//	Params:
//		angle = describes the magnitude in radians of the rotation about ox.
template<typename TRetMat>
TRetMat rotation_matrix_ox(float angle);

// Composes a rotatiom matrix about oy axis.
//	Params:
//		angle = describes the magnitude in radians of the rotation about oy.
template<typename TRetMat>
TRetMat rotation_matrix_oy(float angle);

// Composes a rotatiom matrix about oz axis.
//	Params:
//		angle = describes the magnitude in radians of the rotation about oz.
template<typename TRetMat>
TRetMat rotation_matrix_oz(float angle);

// Returns a matrix which can be used to scale vectors s.
template<typename TRetMat>
TRetMat scale_matrix(const float3& s);

// Returns a matrix that is a concatenation of translation by p and rotation by q.
// The result is equal to translation_matrix(p) * rotation_matrix(q).
mat4 tr_matrix(const float3& p, const quat& q);

// Returns a matrix that is a concatentation of traslation by p and a look at rotation
mat4 tr_matrix(const float3& position, const float3& target, const float3& up = float3::unit_y);

// Returns a matrix which can be used to translate vectors to the position p.
mat4 translation_matrix(const float3& p);

// Return a matrix that is a concatenation of translation by p, rotation by q and scale by s.
// The result is equal to translation_matrix(p) * rotation_matrix(q) * scale_matrix(s).
mat4 trs_matrix(const float3& p, const quat& q, const float3& s);

// Composes a matrix that cam be used to transform from world space to view space.
//	Params:
//		position = an origin, where eye(camera) is situated.
//		target = a point of interest.
//		up = the direction that is considered to be upward.
mat4 view_matrix(const float3& position, const float3& target, const float3& up = float3::unit_y);

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


inline quat from_axis_angle_rotation(const float3& axis, float angle)
{
	assert(is_normalized(axis));

	if (approx_equal(angle, 0.f)) return quat::identity; // no angle, no rotation

	float half_angle = angle * 0.5f;
	float c = std::cos(half_angle);
	float s = std::sin(half_angle);
	return quat(axis.x * s, axis.y * s, axis.z * s, c);
}

template<typename TMat>
quat from_rotation_matrix(const TMat& m)
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

inline mat4 orthographic_matrix(float width, float height, float near, float far)
{
	assert(width > 0);
	assert(height > 0);
	assert(near < far);

	float far_minus_near = far - near;
	float right = width / 2.f;
	float top = height / 2.f;

	return mat4(
		1.f / right, 0, 0, 0,
		0, 1.f / top, 0, 0,
		0, 0, -2.f / far_minus_near, -(far + near) / far_minus_near,
		0, 0, 0, 1
	);
}

inline mat4 orthographic_matrix(float left, float right, float bottom, float top, float near, float far)
{
	assert(left < right);
	assert(bottom < top);
	assert(near < far);

	float doubled_near = 2.f * near;
	float far_minus_near = far - near;
	float right_minus_left = right - left;
	float top_minus_bottom = top - bottom;

	return mat4(
		2.f / right_minus_left, 0, 0, -(right + left) / right_minus_left,
		0, 2.f / top_minus_bottom, 0, -(top + bottom) / top_minus_bottom,
		0, 0, -2.f / far_minus_near, -(far + near) / far_minus_near,
		0, 0, 0, 1
	);
}

inline mat4 perspective_matrix(float left, float right, float bottom, float top, float near, float far)
{
	float doubled_near = 2.f * near;
	float far_minus_near = far - near;
	float right_minus_left = right - left;
	float top_minus_bottom = top - bottom;


	return mat4(
		doubled_near / right_minus_left, 0, (right + left) / right_minus_left, 0,
		0, doubled_near / top_minus_bottom, (top + bottom) / top_minus_bottom, 0,
		0, 0, -(far + near) / far_minus_near, -doubled_near * far / far_minus_near,
		0, 0, -1, 0
	);
}

inline mat4 perspective_matrix(float vert_fov, float wh_ratio, float near, float far) 
{
	assert(0 < vert_fov && vert_fov < pi);
	assert(0 < near && near < far);

	float fat_minus_near = far - near;
	float rev_tangent = 1.f / std::tan(vert_fov * 0.5f);

	/*
	* TAN = tan(vert_fov / 2.0L)
	* top = near * TAN
	* right = top * wh_ratio = wh_ratio * near * TAN
	*
	* 2near / (right - (-rigth)) = 2near / 2right = near / right =
	* near / (wh_ratio * near * TAN) = 1 / (wh_ratio * TAN)
	*
	* 2near / (top - (-top)) = 2near / 2top = neat / top =
	* near / (near * TAN) = 1 / TAN */

	return mat4(
		(1.f / wh_ratio) * rev_tangent, 0, 0, 0,
		0, rev_tangent, 0, 0,
		0, 0, -(far + near) / fat_minus_near, -2.f * near * far / fat_minus_near,
		0, 0, -1, 0
	);
}

inline float3 position(const mat4& m)
{
	return float3(m.m03, m.m13, m.m23);
}

inline void set_position(mat4& m, const float3& p)
{
	m.m03 = p.x;
	m.m13 = p.y;
	m.m23 = p.z;
}

template<typename TRetMat>
TRetMat rotation_matrix(const quat& q)
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

template<typename TRetMat>
TRetMat rotation_matrix(const float3& axis, float angle)
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

template<typename TRetMat>
TRetMat rotation_matrix(const float3& position, const float3& target, const float3& up)
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

template<typename TRetMat>
TRetMat rotation_matrix_ox(float angle)
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

template<typename TRetMat>
TRetMat rotation_matrix_oy(float angle)
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

template<typename TRetMat>
TRetMat rotation_matrix_oz(float angle)
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

template<typename TRetMat>
TRetMat scale_matrix(const float3& s)
{
	assert(greater_than(s, 0));

	TRetMat m = TRetMat::identity;
	m.m00 = s.x;
	m.m11 = s.y;
	m.m22 = s.z;
	return m;
}

inline mat4 tr_matrix(const float3& p, const quat& q)
{
	mat4 m = rotation_matrix<mat4>(q);
	set_position(m, p);
	return m;
}

inline mat4 tr_matrix(const float3& position, const float3& target, const float3& up)
{
	mat4 m = rotation_matrix<mat4>(position, target, up);
	set_position(m, position);
	return m;
}

inline mat4 translation_matrix(const float3& p)
{
	mat4 m = mat4::identity;
	set_position(m, p);
	return m;
}

inline mat4 trs_matrix(const float3& p, const quat& q, const float3& s)
{
	return tr_matrix(p, q) * scale_matrix<mat4>(s);
}

inline mat4 view_matrix(const float3& position, const float3& target, const float3& up)
{
	assert(position != target);
	assert(is_normalized(up));

	float3 forward = normalize(target - position);
	float3 right = normalize(cross(up, forward));
	float3 new_up = normalize(cross(forward, right));

	// transpose(rotation_matrix<mat4>(position, target, up)) * translation_matrix(-position)
	mat4 r = mat4::identity;
	r.m00 = right.x;	r.m01 = right.y;	r.m02 = right.z;	r.m03 = dot(right, -position);
	r.m10 = new_up.x;	r.m11 = new_up.y;	r.m12 = new_up.z;	r.m13 = dot(new_up, -position);
	r.m20 = forward.x;	r.m21 = forward.y;	r.m22 = forward.z;	r.m23 = dot(forward, -position);
	
	return r;
}

} // namespace cg

#endif // CG_MATH_TRANSFORM_H_
