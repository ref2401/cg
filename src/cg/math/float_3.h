#ifndef CG_MATH_FLOAT_3_H_
#define CG_MATH_FLOAT_3_H_

#include <cassert>
#include <cmath>
#include <cstdint>
#include <algorithm>
#include <ostream>
#include <type_traits>
#include "cg/math/utils.h"


namespace cg {

struct float_3 {
	static const float_3 zero;
	static const float_3 unit_x;
	static const float_3 unit_y;
	static const float_3 unit_z;
	static const float_3 unit_xyz;


	float_3();

	explicit float_3(float val);

	float_3(float x, float y, float z);


	// Adds val to every component of the vector.
	template<typename Numeric>
	float_3& operator+=(const Numeric& val);

	// Adds the specified vector v to this vector.
	float_3& operator+=(const float_3& val);

	// Subtracts val from every component of the vector.
	template<typename Numeric>
	float_3& operator-=(const Numeric& val);

	// Subtracts  the specified vector v this vector.
	float_3& operator-=(const float_3& val);

	// Multiplies every component of the vector by val.
	template<typename Numeric>
	float_3& operator*=(const Numeric& val);

	// Devides every component of the vector by val.
	template<typename Numeric>
	float_3& operator/=(const Numeric& val);


	union {
		struct { float x, y, z; };
		struct { float r, g, b; };
		struct { float width, height, depth; };
	};
};

inline float_3::float_3() : x(0), y(0), z(0) {}

inline float_3::float_3(float val) : x(val), y(val), z(val) {}

inline float_3::float_3(float x, float y, float z) : x(x), y(y), z(z) {}


template<typename Numeric>
inline float_3& float_3::operator+=(const Numeric& val)
{
	static_assert(std::is_integral<Numeric>::value || std::is_floating_point<Numeric>::value,
		"Numeric type must be an integer or floating point value.");
	x += val;
	y += val;
	z += val;
	return *this;
}

inline float_3& float_3::operator+=(const float_3& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}

template<typename Numeric>
inline float_3& float_3::operator-=(const Numeric& val)
{
	static_assert(std::is_integral<Numeric>::value || std::is_floating_point<Numeric>::value,
		"Numeric type must be an integer or floating point value.");
	x -= val;
	y -= val;
	z -= val;
	return *this;
}

inline float_3& float_3::operator-=(const float_3& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}

template<typename Numeric>
inline float_3& float_3::operator*=(const Numeric& val)
{
	static_assert(std::is_integral<Numeric>::value || std::is_floating_point<Numeric>::value,
		"Numeric type must be an integer or floating point value.");
	x *= val;
	y *= val;
	z *= val;
	return *this;
}

template<typename Numeric>
inline float_3& float_3::operator/=(const Numeric& val)
{
	static_assert(std::is_integral<Numeric>::value || std::is_floating_point<Numeric>::value,
		"Numeric type must be an integer or floating point value.");
	x /= val;
	y /= val;
	z /= val;
	return *this;
}


inline bool operator==(const float_3& lhs, const float_3& rhs)
{
	return approx_equal(lhs.x, rhs.x)
		&& approx_equal(lhs.y, rhs.y)
		&& approx_equal(lhs.z, rhs.z);
}

inline bool operator!=(const float_3& lhs, const float_3& rhs)
{
	return !(lhs == rhs);
}

// operator+
template<typename Numeric>
inline float_3 operator+(float_3 v, Numeric val)
{
	static_assert(std::is_integral<Numeric>::value || std::is_floating_point<Numeric>::value,
		"Numeric type must be an integer or floating point value.");
	return v += val;
}

template<typename Numeric>
inline float_3 operator+(Numeric val, float_3 v)
{
	static_assert(std::is_integral<Numeric>::value || std::is_floating_point<Numeric>::value,
		"Numeric type must be an integer or floating point value.");
	return v += val;
}

inline float_3 operator+(float_3 lhs, const float_3& rhs)
{
	return lhs += rhs;
}

// operator-
template<typename Numeric>
inline float_3 operator-(float_3 v, Numeric val)
{
	static_assert(std::is_integral<Numeric>::value || std::is_floating_point<Numeric>::value,
		"Numeric type must be an integer or floating point value.");
	return v -= val;
}

template<typename Numeric>
inline float_3 operator-(Numeric val, float_3 v)
{
	static_assert(std::is_integral<Numeric>::value || std::is_floating_point<Numeric>::value,
		"Numeric type must be an integer or floating point value.");
	return float_3(val - v.x, val - v.y, val - v.z);
}

inline float_3 operator-(float_3 lhs, const float_3& rhs)
{
	return lhs -= rhs;
}

// unary operator-
inline float_3 operator-(const float_3& v)
{
	return float_3(-v.x, -v.y, -v.z);
}

//operator*
template<typename Numeric>
inline float_3 operator*(float_3 v, Numeric val)
{
	static_assert(std::is_integral<Numeric>::value || std::is_floating_point<Numeric>::value,
		"Numeric type must be an integer or floating point value.");
	return v *= val;
}

template<typename Numeric>
inline float_3 operator*(Numeric val, float_3 v)
{
	static_assert(std::is_integral<Numeric>::value || std::is_floating_point<Numeric>::value,
		"Numeric type must be an integer or floating point value.");
	return v *= val;
}

// operator/
template<typename Numeric>
inline float_3 operator/(float_3 v, Numeric val)
{
	static_assert(std::is_integral<Numeric>::value || std::is_floating_point<Numeric>::value,
		"Numeric type must be an integer or floating point value.");
	return v /= val;
}

template<typename Numeric>
inline float_3 operator/(Numeric val, float_3 v)
{
	static_assert(std::is_integral<Numeric>::value || std::is_floating_point<Numeric>::value,
		"Numeric type must be an integer or floating point value.");
	return float_3(val / v.x, val / v.y, val / v.z);
}

inline std::ostream& operator<<(std::ostream& out, const cg::float_3& v)
{
	out << "float_3(" << v.x << ", " << v.y << ", " << v.z << ")";
	return out;
}

inline std::wostream& operator<<(std::wostream& out, const cg::float_3& v)
{
	out << "float_3(" << v.x << ", " << v.y << ", " << v.z << ")";
	return out;
}


// Constrains vector v to lie between two further vectors.
// The function processes each component of the vector separately.
// Params:
//		v = The value to constrain
//		v_min =	The lower end of the range into which to constrain v.
//		v_max = The upper end of the range into which to constrain v.
inline float_3 clamp(const float_3& v, 
	const float_3& v_lo = float_3::zero, const float_3& v_hi = float_3::unit_xyz)
{
	assert(v_lo.x <= v_hi.x);
	assert(v_lo.y <= v_hi.y);
	assert(v_lo.z <= v_hi.z);

	return float_3(
		std::min(v_hi.x, std::max(v_lo.x, v.x)),
		std::min(v_hi.y, std::max(v_lo.y, v.y)),
		std::min(v_hi.z, std::max(v_lo.z, v.z))
	);
}

// Calculates the cross product of of the given vectors.
inline float_3 cross(const float_3& lhs, const float_3& rhs)
{
	return float_3(
		(lhs.y *rhs.z) - (lhs.z * rhs.y),
		(lhs.z * rhs.x) - (lhs.x * rhs.z),
		(lhs.x * rhs.y) - (lhs.y * rhs.x)
	);
}

// Calculates the dot product of the given vectors.
inline float dot(const float_3& lhs, const float_3& rhs)
{
	return (lhs.x * rhs.x) + (lhs.y * rhs.y) + (lhs.z * rhs.z);
}

// Calculates the squared length of v.
inline float len_square(const float_3& v)
{
	return (v.x * v.x) + (v.y * v.y) + (v.z * v.z);
}

// Calculates the length of v.
inline float len(const float_3& v)
{
	return std::sqrt(len_square(v));
}

// Linearly interpolates between two values.
// Params:
//		lhs = The start of the range in which to interpolate.
//		rhs = The end of the range in which to interpolate.
//		factor = The value to use to interpolate between lhs & rhs.
//		factor has to lie within the range [0 .. 1].
inline float_3 lerp(const float_3& lhs, const float_3 rhs, float factor)
{
	assert(0.f <= factor && factor <= 1.f);
	return lhs + factor * (rhs - lhs);
}

// Returns new vector which is normalized(unit length) copy of the given one.
inline float_3 normalize(const float_3& v)
{
	float l2 = len_square(v);
	if (approx_equal(l2, 0.f) || approx_equal(l2, 1.f)) return v;

	float factor = 1.f / sqrt(l2);
	return v * factor;
}

// Returns rgb color volor
// (31 .. 24) bytes are ignored.
// red: (23 .. 16) bytes. 
// green: (15 .. 8) bytes. 
// blue: (7 .. 1) bytes.
inline float_3 rgb(uint32_t val)
{
	return float_3(
		((val >> 16) & 0xFF) / 255.f,
		((val >> 8) & 0xFF) / 255.f,
		(val & 0xFF) / 255.f
	);
}

} // namespace cg

#endif // CG_MATH_FLOAT_3_H_
