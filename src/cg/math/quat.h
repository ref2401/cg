#ifndef CG_MATH_QUAT_H_
#define CG_MATH_QUAT_H_

#include <ostream>
#include <type_traits>
#include "cg/math/float3.h"
#include "cg/math/float4.h"
#include "cg/math/utils.h"


namespace cg {

// In mathematics, the quaternions are a number system that extends the complex numbers.
// Quaternions extends a rotation in three dimensions to a rotation in four dimensions.
// This avoids "gimbal lock" and allows for smooth continuous rotation.
// q = xi + yj + zk + a1, where x, y, z, a are real numbers and i, j, k are imaginary units.
// For any quaternion 'a' is called scalar part and 'xi + yj + zk' is called its vector part.
struct quat {
	static const quat i;
	static const quat j;
	static const quat k;
	static const quat identity;
	static const quat zero;


	quat(float x, float y, float z, float a);

	quat(const float3& v, float a);


	// Adds the specified quaternion q to this quaternion.
	quat& operator+=(const quat& q);

	// Subtracts the specified quaternion q from this quaternion.
	quat& operator-=(const quat& q);

	// Multiplies each component of this quaternion by val.
	template<typename Numeric>
	quat& operator*=(const Numeric& val);

	// Calculates the Hamilton product of this and the specified quaterions.
	// Stores the result in this quaternion.
	quat& operator*=(const quat& q);

	// Devides each component of this quaternion by val.
	template<typename Numeric>
	quat& operator/=(const Numeric& val);


	float x, y, z, a;
};

inline quat::quat(float x, float y, float z, float a) : x(x), y(y), z(z), a(a) {}

inline quat::quat(const float3& v, float a) : x(v.x), y(v.y), z(v.z), a(a) {}


inline quat& quat::operator+=(const quat& q)
{
	x += q.x;
	y += q.y;
	z += q.z;
	a += q.a;
	return *this;
}

inline quat& quat::operator-=(const quat& q)
{
	x -= q.x;
	y -= q.y;
	z -= q.z;
	a -= q.a;
	return *this;
}

template<typename Numeric>
inline quat& quat::operator*=(const Numeric& val)
{
	static_assert(std::is_integral<Numeric>::value || std::is_floating_point<Numeric>::value,
		"Numeric type must be an integer or floating point value.");
	x *= val;
	y *= val;
	z *= val;
	a *= val;
	return *this;
}

inline quat& quat::operator*=(const quat& q)
{
	float xp = (a * q.x) + (x * q.a) + (y * q.z) - (z * q.y);
	float yp = (a * q.y) + (y * q.a) + (z * q.x) - (x * q.z);
	float zp = (a * q.z) + (z * q.a) + (x * q.y) - (y * q.x);
	float ap = (a * q.a) - (x * q.x) - (y * q.y) - (z * q.z);

	x = xp;
	y = yp;
	z = zp;
	a = ap;
	return *this;
}

template<typename Numeric>
inline quat& quat::operator/=(const Numeric& val)
{
	static_assert(std::is_integral<Numeric>::value || std::is_floating_point<Numeric>::value,
		"Numeric type must be an integer or floating point value.");
	x /= val;
	y /= val;
	z /= val;
	a /= val;
	return *this;
}


inline bool operator==(const quat& lhs, const quat& rhs)
{
	return approx_equal(lhs.x, rhs.x)
		&& approx_equal(lhs.y, rhs.y)
		&& approx_equal(lhs.z, rhs.z)
		&& approx_equal(lhs.a, rhs.a);
}

inline bool operator!=(const quat& lhs, const quat& rhs)
{
	return !(lhs == rhs);
}

inline quat operator+(const quat& lhs, const quat& rhs)
{
	return quat(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.a + rhs.a);
}

inline quat operator-(const quat& lhs, const quat& rhs)
{
	return quat(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.a - rhs.a);
}

inline quat operator-(const quat& q)
{
	return quat(-q.x, -q.y, -q.z, -q.a);
}

template<typename Numeric>
inline quat operator*(const quat& q, const Numeric& val)
{
	static_assert(std::is_integral<Numeric>::value || std::is_floating_point<Numeric>::value,
		"Numeric type must be an integer or floating point value.");
	return quat(q.x * val, q.y * val, q.z * val, q.a * val);
}

template<typename Numeric>
inline quat operator*(const Numeric& val, const quat& q)
{
	static_assert(std::is_integral<Numeric>::value || std::is_floating_point<Numeric>::value,
		"Numeric type must be an integer or floating point value.");
	return quat(q.x * val, q.y * val, q.z * val, q.a * val);
}

inline quat operator*(const quat& lhs, const quat& rhs)
{
	return quat(
		(lhs.a * rhs.x) + (lhs.x * rhs.a) + (lhs.y * rhs.z) - (lhs.z * rhs.y),
		(lhs.a * rhs.y) + (lhs.y * rhs.a) + (lhs.z * rhs.x) - (lhs.x * rhs.z),
		(lhs.a * rhs.z) + (lhs.z * rhs.a) + (lhs.x * rhs.y) - (lhs.y * rhs.x),
		(lhs.a * rhs.a) - (lhs.x * rhs.x) - (lhs.y * rhs.y) - (lhs.z * rhs.z)
	);
}

template<typename Numeric>
inline quat operator/(const quat& q, const Numeric& val)
{
	static_assert(std::is_integral<Numeric>::value || std::is_floating_point<Numeric>::value,
		"Numeric type must be an integer or floating point value.");
	return quat(q.x / val, q.y / val, q.z / val, q.a / val);
}

template<typename Numeric>
inline quat operator/(const Numeric& val, const quat& q)
{
	static_assert(std::is_integral<Numeric>::value || std::is_floating_point<Numeric>::value,
		"Numeric type must be an integer or floating point value.");
	return quat(val / q.x, val / q.y, val / q.z, val / q.a);
}

inline std::ostream& operator<<(std::ostream& out, const quat& q)
{
	out << "quat(" << q.x << ", " << q.y << ", " << q.z << ", " << q.a << ")";
	return out;
}

inline std::wostream& operator<<(std::wostream& out, const quat& q)
{
	out << "quat(" << q.x << ", " << q.y << ", " << q.z << ", " << q.a << ")";
	return out;
}

} // namespace cg

#endif // CG_MATH_QUAT_H_
