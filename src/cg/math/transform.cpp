#include "cg/math/math.h"

namespace cg {

// ----- Viewpoint -----

Viewpoint::Viewpoint(float3 position, float3 target, float3 up) noexcept :
	position(position),
	target(target),
	up(up)
{}

mat4 Viewpoint::view_matrix() const noexcept
{
	return cg::view_matrix(position, target, normalize(up));
}

// ----- funcs -----

quat from_axis_angle_rotation(const float3& axis, float angle) noexcept
{
	assert(is_normalized(axis));

	if (approx_equal(angle, 0.f)) return quat::identity; // no angle, no rotation

	float half_angle = angle * 0.5f;
	float c = std::cos(half_angle);
	float s = std::sin(half_angle);
	return quat(axis.x * s, axis.y * s, axis.z * s, c);
}

Viewpoint lerp(const Viewpoint& l, const Viewpoint& r, float factor) noexcept
{
	assert(0.f <= factor && factor <= 1.f);
	return Viewpoint(
		cg::lerp(l.position, r.position, factor),
		cg::lerp(l.target, r.target, factor),
		cg::lerp(l.up, r.up, factor)
	);
}

mat4 orthographic_matrix(float width, float height, float near_z, float far_z) noexcept
{
	assert(width > 0);
	assert(height > 0);
	assert(near_z < far_z);

	float far_minus_near = far_z - near_z;
	float right = width / 2.f;
	float top = height / 2.f;

	return mat4(
		1.f / right, 0, 0, 0,
		0, 1.f / top, 0, 0,
		0, 0, -2.f / far_minus_near, -(far_z + near_z) / far_minus_near,
		0, 0, 0, 1
	);
}

mat4 orthographic_matrix(float left, float right, float bottom, float top, float near_z, float far_z) noexcept
{
	assert(left < right);
	assert(bottom < top);
	assert(near_z < far_z);

	float doubled_near = 2.f * near_z;
	float far_minus_near = far_z - near_z;
	float right_minus_left = right - left;
	float top_minus_bottom = top - bottom;

	return mat4(
		2.f / right_minus_left, 0, 0, -(right + left) / right_minus_left,
		0, 2.f / top_minus_bottom, 0, -(top + bottom) / top_minus_bottom,
		0, 0, -2.f / far_minus_near, -(far_z + near_z) / far_minus_near,
		0, 0, 0, 1
	);
}

mat4 perspective_matrix(float left, float right, float bottom, float top, float near_z, float far_z) noexcept
{
	float doubled_near = 2.f * near_z;
	float far_minus_near = far_z - near_z;
	float right_minus_left = right - left;
	float top_minus_bottom = top - bottom;


	return mat4(
		doubled_near / right_minus_left, 0, (right + left) / right_minus_left, 0,
		0, doubled_near / top_minus_bottom, (top + bottom) / top_minus_bottom, 0,
		0, 0, -(far_z + near_z) / far_minus_near, -doubled_near * far_z / far_minus_near,
		0, 0, -1, 0
	);
}

mat4 perspective_matrix(float vert_fov, float wh_ratio, float near_z, float far_z) noexcept
{
	assert(0 < vert_fov && vert_fov < pi);
	assert(0 < near_z && near_z < far_z);

	float fat_minus_near = far_z - near_z;
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
		0, 0, -(far_z + near_z) / fat_minus_near, -2.f * near_z * far_z / fat_minus_near,
		0, 0, -1, 0
	);
}

mat4 view_matrix(const float3& position, const float3& target, const float3& up) noexcept
{
	assert(position != target);
	assert(is_normalized(up));

	float3 forward = normalize(target - position);
	float3 right = normalize(cross(forward, up));
	float3 new_up = normalize(cross(right, forward));

	mat4 r = mat4::identity;
	r.m00 = right.x;	r.m01 = right.y;	r.m02 = right.z;	r.m03 = dot(right, -position);
	r.m10 = new_up.x;	r.m11 = new_up.y;	r.m12 = new_up.z;	r.m13 = dot(new_up, -position);
	r.m20 = -forward.x;	r.m21 = -forward.y;	r.m22 = -forward.z;	r.m23 = dot(forward, position);

	return r;
}

} // namespace