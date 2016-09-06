#ifndef CG_MATH_FLOAT2_H_
#define CG_MATH_FLOAT2_H_

#include <cassert>
#include <cmath>
#include <cstdint>
#include <ostream>
#include <type_traits>
#include "cg/math/utils.h"


namespace cg {

struct float2 {
	static const float2 unit_x;
	static const float2 unit_y;
	static const float2 unit_xy;
	static const float2 zero;


	float2();

	explicit float2(float val);

	float2(float x, float y);


	// Adds val to every component of the vector.
	template<typename Numeric>
	float2& operator+=(const Numeric& val);

	// Adds the specified vector v to this vector.
	float2& operator+=(const float2& val);

	// Subtracts val from every component of the vector.
	template<typename Numeric>
	float2& operator-=(const Numeric& val);

	// Subtracts  the specified vector v this vector.
	float2& operator-=(const float2& val);

	// Multiplies each component of the vector by val.
	template<typename Numeric>
	float2& operator*=(const Numeric& val);

	// Devides each component of the vector by val.
	template<typename Numeric>
	float2& operator/=(const Numeric& val);


	union {
		struct { float x, y; };

		struct { float u, v; };
	};
};

inline float2::float2() : x(0), y(0) {}

inline float2::float2(float val) : x(val), y(val) {}

inline float2::float2(float x, float y) : x(x), y(y) {}


template<typename Numeric>
inline float2& float2::operator+=(const Numeric& val)
{
	static_assert(std::is_integral<Numeric>::value || std::is_floating_point<Numeric>::value,
		"Numeric type must be an integer or a floating point value.");
	x += val;
	y += val;
	return *this;
}

inline float2& float2::operator+=(const float2& v)
{
	x += v.x;
	y += v.y;
	return *this;
}

template<typename Numeric>
inline float2& float2::operator-=(const Numeric& val)
{
	static_assert(std::is_integral<Numeric>::value || std::is_floating_point<Numeric>::value,
		"Numeric type must be an integer or a floating point value.");
	x -= val;
	y -= val;
	return *this;
}

inline float2& float2::operator-=(const float2& v)
{
	x -= v.x;
	y -= v.y;
	return *this;
}

template<typename Numeric>
inline float2& float2::operator*=(const Numeric& val)
{
	static_assert(std::is_integral<Numeric>::value || std::is_floating_point<Numeric>::value,
		"Numeric type must be an integer or a floating point value.");
	x *= val;
	y *= val;
	return *this;
}

template<typename Numeric>
inline float2& float2::operator/=(const Numeric& val)
{
	static_assert(std::is_integral<Numeric>::value || std::is_floating_point<Numeric>::value,
		"Numeric type must be an integer or a floating point value.");
	assert(!approx_equal(val, 0));

	x /= val;
	y /= val;
	return *this;
}


inline bool operator==(const float2& lhs, const float2& rhs)
{
	return approx_equal(lhs.x, rhs.x)
		&& approx_equal(lhs.y, rhs.y);
}

inline bool operator!=(const float2& lhs, const float2& rhs)
{
	return !(lhs == rhs);
}

template<typename Numeric>
inline float2 operator+(const float2& v, const Numeric& val)
{
	static_assert(std::is_integral<Numeric>::value || std::is_floating_point<Numeric>::value,
		"Numeric type must be an integer or a floating point value.");
	return float2(v.x + val, v.y + val);
}

template<typename Numeric>
inline float2 operator+(const Numeric& val, const float2& v)
{
	static_assert(std::is_integral<Numeric>::value || std::is_floating_point<Numeric>::value,
		"Numeric type must be an integer or a floating point value.");
	return float2(v.x + val, v.y + val);
}

inline float2 operator+(const float2& lhs, const float2& rhs)
{
	return float2(lhs.x + rhs.x, lhs.y + rhs.y);
}

template<typename Numeric>
inline float2 operator-(const float2& v, const Numeric& val)
{
	static_assert(std::is_integral<Numeric>::value || std::is_floating_point<Numeric>::value,
		"Numeric type must be an integer or a floating point value.");
	return float2(v.x - val, v.y - val);
}

template<typename Numeric>
inline float2 operator-(const Numeric& val, const float2& v)
{
	static_assert(std::is_integral<Numeric>::value || std::is_floating_point<Numeric>::value,
		"Numeric type must be an integer or a floating point value.");
	return float2(val - v.x, val - v.y);
}

inline float2 operator-(const float2& lhs, const float2& rhs)
{
	return float2(lhs.x - rhs.x, lhs.y - rhs.y);
}

inline float2 operator-(const float2& v)
{
	return float2(-v.x, -v.y);
}

template<typename Numeric>
inline float2 operator*(const float2& v, const Numeric& val)
{
	static_assert(std::is_integral<Numeric>::value || std::is_floating_point<Numeric>::value,
		"Numeric type must be an integer or a floating point value.");
	return float2(v.x * val, v.y * val);
}

template<typename Numeric>
inline float2 operator*(const Numeric& val, const float2& v)
{
	static_assert(std::is_integral<Numeric>::value || std::is_floating_point<Numeric>::value,
		"Numeric type must be an integer or a floating point value.");
	return float2(v.x * val, v.y * val);
}

template<typename Numeric>
inline float2 operator/(const float2& v, const Numeric& val)
{
	static_assert(std::is_integral<Numeric>::value || std::is_floating_point<Numeric>::value,
		"Numeric type must be an integer or a floating point value.");
	assert(!approx_equal(val, 0));

	return float2(v.x / val, v.y / val);
}

template<typename Numeric>
inline float2 operator/(const Numeric& val, const float2& v)
{
	static_assert(std::is_integral<Numeric>::value || std::is_floating_point<Numeric>::value,
		"Numeric type must be an integer or a floating point value.");
	return float2(val / v.x, val / v.y);
}

inline std::ostream& operator<<(std::ostream& out, const cg::float2& v)
{
	out << "float2(" << v.x << ", " << v.y << ")";
	return out;
}

inline std::wostream& operator<<(std::wostream& out, const cg::float2& v)
{
	out << "float2(" << v.x << ", " << v.y  << ")";
	return out;
}


// Constrains vector v to lie between two further vectors.
// The function processes each component of the vector separately.
// Params:
//		v = The value to constrain
//		v_lo =	The lower end of the range into which to constrain v.
//		v_hi = The upper end of the range into which to constrain v.
inline float2 clamp(const float2& v,
	const float2& v_lo = float2::zero, const float2& v_hi = float2::unit_xy)
{
	return float2(
		clamp(v.x, v_lo.x, v_hi.x),
		clamp(v.y, v_lo.y, v_hi.y)
	);
}

// Calculates the dot product of the given vectors.
inline float dot(const float2& lhs, const float2& rhs)
{
	return (lhs.x * rhs.x) + (lhs.y * rhs.y);
}

// Calculates the squared length of v.
inline float len_squared(const float2& v)
{
	return (v.x * v.x) + (v.y * v.y);
}

// Calculates the length of v.
inline float len(const float2& v)
{
	return std::sqrt(len_squared(v));
}

// Checks whether the specified vector is normalized.
inline bool is_normalized(const float2& v)
{
	return approx_equal(len_squared(v), 1.f);
}

// Linearly interpolates between two values.
// Params:
//		lhs = The start of the range in which to interpolate.
//		rhs = The end of the range in which to interpolate.
//		factor = The value to use to interpolate between lhs & rhs.
//		factor has to lie within the range [0 .. 1].
inline float2 lerp(const float2& lhs, const float2 rhs, float factor)
{
	assert(0.f <= factor && factor <= 1.f);
	return lhs + factor * (rhs - lhs);
}

// Returns new vector which is normalized(unit length) copy of the given one.
inline float2 normalize(const float2& v)
{
	float l2 = len_squared(v);
	if (approx_equal(l2, 0.f) || approx_equal(l2, 1.f)) return v;

	float factor = 1.f / sqrt(l2);
	return v * factor;
}


} // namespace

#endif // CG_MATH_FLOAT2_H_
