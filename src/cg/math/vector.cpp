#include "cg/math/vector.h"

namespace cg {

const float2 float2::unit_x(1, 0);
const float2 float2::unit_y(0, 1);
const float2 float2::unit_xy(1);
const float2 float2::zero(0);

const float3 float3::unit_x(1, 0, 0);
const float3 float3::unit_y(0, 1, 0);
const float3 float3::unit_z(0, 0, 1);
const float3 float3::unit_xy(1, 1, 0);
const float3 float3::unit_xyz(1);
const float3 float3::zero(0);

const float4 float4::unit_x(1, 0, 0, 0);
const float4 float4::unit_y(0, 1, 0, 0);
const float4 float4::unit_z(0, 0, 1, 0);
const float4 float4::unit_w(0, 0, 0, 1);
const float4 float4::unit_xyzw(1);
const float4 float4::zero(0);

const quat quat::i(1, 0, 0, 0);
const quat quat::j(0, 1, 0, 0);
const quat quat::k(0, 0, 1, 0);
const quat quat::identity(0, 0, 0, 1);
const quat quat::zero(0, 0, 0, 0);

const uint2 uint2::zero(0);
const uint2 uint2::unit_x(1, 0);
const uint2 uint2::unit_y(0, 1);
const uint2 uint2::unit_xy(1);


// ----- funcs -----

std::ostream& operator<<(std::ostream& out, const float2& v)
{
	out << "float2(" << v.x << ", " << v.y << ")";
	return out;
}

std::wostream& operator<<(std::wostream& out, const float2& v)
{
	out << "float2(" << v.x << ", " << v.y << ")";
	return out;
}

std::ostream& operator<<(std::ostream& out, const float3& v)
{
	out << "float3(" << v.x << ", " << v.y << ", " << v.z << ")";
	return out;
}

std::wostream& operator<<(std::wostream& out, const float3& v)
{
	out << "float3(" << v.x << ", " << v.y << ", " << v.z << ")";
	return out;
}

std::ostream& operator<<(std::ostream& out, const float4& v)
{
	out << "float4(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
	return out;
}

std::wostream& operator<<(std::wostream& out, const float4& v)
{
	out << "float4(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
	return out;
}

std::ostream& operator<<(std::ostream& out, const quat& q)
{
	out << "quat(" << q.x << ", " << q.y << ", " << q.z << ", " << q.a << ")";
	return out;
}

std::wostream& operator<<(std::wostream& out, const quat& q)
{
	out << "quat(" << q.x << ", " << q.y << ", " << q.z << ", " << q.a << ")";
	return out;
}

std::ostream& operator<<(std::ostream& out, const uint2& v)
{
	out << "uint2(" << v.x << ", " << v.y << ")";
	return out;
}

std::wostream& operator<<(std::wostream& out, const uint2& v)
{
	out << "uint2(" << v.x << ", " << v.y << ")";
	return out;
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
