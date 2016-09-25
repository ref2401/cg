#include "cg/math/math.h"

#include <cassert>
#include <cmath>


namespace cg {

const float2 float2::unit_x(1, 0);
const float2 float2::unit_y(0, 1);
const float2 float2::unit_xy(1);
const float2 float2::zero(0);

const float3 float3::unit_x(1, 0, 0);
const float3 float3::unit_y(0, 1, 0);
const float3 float3::unit_z(0, 0, 1);
const float3 float3::unit_xyz(1);
const float3 float3::zero(0);

const float4 float4::unit_x(1, 0, 0, 0);
const float4 float4::unit_y(0, 1, 0, 0);
const float4 float4::unit_z(0, 0, 1, 0);
const float4 float4::unit_w(0, 0, 0, 1);
const float4 float4::unit_xyzw(1);
const float4 float4::zero(0);

const mat3 mat3::identity(1, 0, 0, 0, 1, 0, 0, 0, 1);
const mat3 mat3::zero;

const mat4 mat4::identity(1, 0, 0, 0,  0, 1, 0, 0,  0, 0, 1, 0,  0, 0, 0, 1);
const mat4 mat4::zero;

const quat quat::i(1, 0, 0, 0);
const quat quat::j(0, 1, 0, 0);
const quat quat::k(0, 0, 1, 0);
const quat quat::identity(0, 0, 0, 1);
const quat quat::zero(0, 0, 0, 0);

const uint2 uint2::zero(0);


mat3 inverse(const mat3& m)
{
	// inverse is found by Cramer’s rule.

	// Check whether m is a singular matix
	float d = det(m);
	assert(!approx_equal(d, 0.f));

	// construct the adjugate matrix.
	// cofactor00 cofactor10 cofactor20
	// cofactor01 cofactor11 cofactor21
	// cofactor02 cofactor12 cofactor22
	mat3 adj;
	adj.m00 = m.m11*m.m22 - m.m12*m.m21;
	adj.m01 = -(m.m01*m.m22 - m.m02*m.m21);
	adj.m02 = m.m01*m.m12 - m.m02*m.m11;

	adj.m10 = -(m.m10*m.m22 - m.m12*m.m20);
	adj.m11 = m.m00*m.m22 - m.m02*m.m20;
	adj.m12 = -(m.m00*m.m12 - m.m02*m.m10);

	adj.m20 = m.m10*m.m21 - m.m11*m.m20;
	adj.m21 = -(m.m00*m.m21 - m.m01*m.m20);
	adj.m22 = m.m00*m.m11 - m.m01*m.m10;

	float inv_d = 1.f / d;
	return adj * inv_d;
}

mat4 inverse(const mat4& m) noexcept
{
	// inverse is found by Cramer’s rule.

	// Check whether m is a singular matix
	float d = det(m);
	assert(!approx_equal(d, 0.f));


	// construct the adjugate matrix.
	// cofactor00 cofactor10 cofactor20 cofactor30
	// cofactor01 cofactor11 cofactor21 cofactor31
	// cofactor02 cofactor12 cofactor22 cofactor32
	// cofactor03 cofactor13 cofactor23 cofactor33
	mat4 adj;
	adj.m00 = m.m11*m.m22*m.m33 + m.m12*m.m23*m.m31 + m.m13*m.m21*m.m32 - m.m13*m.m22*m.m31 - m.m12*m.m21*m.m33 - m.m11*m.m23*m.m32;
	adj.m01 = -(m.m01*m.m22*m.m33 + m.m02*m.m23*m.m31 + m.m03*m.m21*m.m32 - m.m03*m.m22*m.m31 - m.m02*m.m21*m.m33 - m.m01*m.m23*m.m32);
	adj.m02 = m.m01*m.m12*m.m33 + m.m02*m.m13*m.m31 + m.m03*m.m11*m.m32 - m.m03*m.m12*m.m31 - m.m02*m.m11*m.m33 - m.m01*m.m13*m.m32;
	adj.m03 = -(m.m01*m.m12*m.m23 + m.m02*m.m13*m.m21 + m.m03*m.m11*m.m22 - m.m03*m.m12*m.m21 - m.m02*m.m11*m.m23 - m.m01*m.m13*m.m22);

	adj.m10 = -(m.m10*m.m22*m.m33 + m.m12*m.m23*m.m30 + m.m13*m.m20*m.m32 - m.m13*m.m22*m.m30 - m.m12*m.m20*m.m33 - m.m10*m.m23*m.m32);
	adj.m11 = m.m00*m.m22*m.m33 + m.m02*m.m23*m.m30 + m.m03*m.m20*m.m32 - m.m03*m.m22*m.m30 - m.m02*m.m20*m.m33 - m.m00*m.m23*m.m32;
	adj.m12 = -(m.m00*m.m12*m.m33 + m.m02*m.m13*m.m30 + m.m03*m.m10*m.m32 - m.m03*m.m12*m.m30 - m.m02*m.m10*m.m33 - m.m00*m.m13*m.m32);
	adj.m13 = m.m00*m.m12*m.m23 + m.m02*m.m13*m.m20 + m.m03*m.m10*m.m22 - m.m03*m.m12*m.m20 - m.m02*m.m10*m.m23 - m.m00*m.m13*m.m22;

	adj.m20 = m.m10*m.m21*m.m33 + m.m11*m.m23*m.m30 + m.m13*m.m20*m.m31 - m.m13*m.m21*m.m30 - m.m11*m.m20*m.m33 - m.m10*m.m23*m.m31;
	adj.m21 = -(m.m00*m.m21*m.m33 + m.m01*m.m23*m.m30 + m.m03*m.m20*m.m31 - m.m03*m.m21*m.m30 - m.m01*m.m20*m.m33 - m.m00*m.m23*m.m31);
	adj.m22 = m.m00*m.m11*m.m33 + m.m01*m.m13*m.m30 + m.m03*m.m10*m.m31 - m.m03*m.m11*m.m30 - m.m01*m.m10*m.m33 - m.m00*m.m13*m.m31;
	adj.m23 = -(m.m00*m.m11*m.m23 + m.m01*m.m13*m.m20 + m.m03*m.m10*m.m21 - m.m03*m.m11*m.m20 - m.m01*m.m10*m.m23 - m.m00*m.m13*m.m21);

	adj.m30 = -(m.m10*m.m21*m.m32 + m.m11*m.m22*m.m30 + m.m12*m.m20*m.m31 - m.m12*m.m21*m.m30 - m.m11*m.m20*m.m32 - m.m10*m.m22*m.m31);
	adj.m31 = m.m00*m.m21*m.m32 + m.m01*m.m22*m.m30 + m.m02*m.m20*m.m31 - m.m02*m.m21*m.m30 - m.m01*m.m20*m.m32 - m.m00*m.m22*m.m31;
	adj.m32 = -(m.m00*m.m11*m.m32 + m.m01*m.m12*m.m30 + m.m02*m.m10*m.m31 - m.m02*m.m11*m.m30 - m.m01*m.m10*m.m32 - m.m00*m.m12*m.m31);
	adj.m33 = m.m00*m.m11*m.m22 + m.m01*m.m12*m.m20 + m.m02*m.m10*m.m21 - m.m02*m.m11*m.m20 - m.m01*m.m10*m.m22 - m.m00*m.m12*m.m21;

	float inv_d = 1.f / d;
	return adj * inv_d;
}

quat slerp(const quat& q, const quat& r, float factor)
{
	assert(is_normalized(q));
	assert(is_normalized(r));
	assert(0.f <= factor && factor <= 1.f);

	float cos_omega = (q.x * r.x) + (q.y * r.y) + (q.z * r.z) + (q.a * r.z);
	quat q1 = r;
	if (cos_omega < 0) {
		cos_omega = -cos_omega;
		q1 = -q1;
	}

	float f0;
	float f1;
	if (cos_omega > 0.9999f) {
		// fallback to the linear interpolation
		f0 = 1.f - factor;
		f1 = factor;
	}
	else {
		float omega = std::acos(cos_omega);
		float inv_sin = 1.f / std::sin(omega);
		f0 = std::sin((1.f - factor) * omega) * inv_sin;
		f1 = std::sin(factor * omega) * inv_sin;
	}

	return normalize(f0 * q + f1 * q1);
}

} // namespace cg