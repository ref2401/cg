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


	float2() noexcept;

	explicit float2(float val) noexcept;

	float2(float x, float y) noexcept;


	// Adds val to every component of the vector.
	float2& operator+=(float val);

	// Adds the specified vector v to this vector.
	float2& operator+=(const float2& val);

	// Subtracts val from every component of the vector.
	float2& operator-=(float val);

	// Subtracts  the specified vector v this vector.
	float2& operator-=(const float2& val);

	// Multiplies each component of the vector by val.
	float2& operator*=(float val);

	// Devides each component of the vector by val.
	float2& operator/=(float val);


	union {
		struct { float x, y; };
		struct { float u, v; };
	};
};

// Checks whether lhs is equal to rhs.
bool operator==(const float2& lhs, const float2& rhs);

// Checks whether lhs is not equal to rhs.
bool operator!=(const float2& lhs, const float2& rhs);

// Adds val to each component of v.
float2 operator+(const float2& v, float val);

// Adds val to each component of v.	
float2 operator+(float val, const float2& v);

// Adds rhs vector to lhs.
float2 operator+(const float2& lhs, const float2& rhs);

// Subtracts val from each component of v.
float2 operator-(const float2& v, float val);

// Subtracts each component of v from val.
float2 operator-(float val, const float2& v);

// Subtracts rhs vector from lhs.
float2 operator-(const float2& lhs, const float2& rhs);

// Negates each component of v.
float2 operator-(const float2& v);

// Multiplies each component of v by val.
float2 operator*(const float2& v, float val);

// Multiplies each component of v by val.
float2 operator*(float val, const float2& v);

// Devides each component of v by val.
float2 operator/(const float2& v, float val);

// Devides val by each component of v.
float2 operator/(float& val, const float2& v);

std::ostream& operator<<(std::ostream& out, const float2& v);

std::wostream& operator<<(std::wostream& out, const float2& v);

// Constrains vector v to lie between two further vectors.
// The function processes each component of the vector separately.
// Params:
//		v = The value to constrain
//		v_lo =	The lower end of the range into which to constrain v.
//		v_hi = The upper end of the range into which to constrain v.
float2 clamp(const float2& v, const float2& v_lo = float2::zero, const float2& v_hi = float2::unit_xy);

// Calculates the dot product of the given vectors.
float dot(const float2& lhs, const float2& rhs);

// Returns true if every component of v is greater than val.
bool greater_than(const float2& v, float val);

// Checks whether the specified vector is normalized.
bool is_normalized(const float2& v);

// Calculates the squared length of v.
float len_squared(const float2& v);

// Calculates the length of v.
float len(const float2& v);

// Linearly interpolates between two values.
// Params:
//		lhs = The start of the range in which to interpolate.
//		rhs = The end of the range in which to interpolate.
//		factor = The value to use to interpolate between lhs & rhs.
//		factor has to lie within the range [0 .. 1].
float2 lerp(const float2& lhs, const float2 rhs, float factor);

// Returns new vector which is normalized(unit length) copy of the given one.
float2 normalize(const float2& v);


inline float2::float2() noexcept : x(0), y(0) {}

inline float2::float2(float val) noexcept : x(val), y(val) {}

inline float2::float2(float x, float y) noexcept : x(x), y(y) {}


inline float2& float2::operator+=(float val)
{
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

inline float2& float2::operator-=(float val)
{
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

inline float2& float2::operator*=(float val)
{
	x *= val;
	y *= val;
	return *this;
}

inline float2& float2::operator/=(float val)
{
	assert(!approx_equal(val, 0.f));

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

inline float2 operator+(const float2& v, float val)
{
	return float2(v.x + val, v.y + val);
}

inline float2 operator+(float val, const float2& v)
{
	return float2(v.x + val, v.y + val);
}

inline float2 operator+(const float2& lhs, const float2& rhs)
{
	return float2(lhs.x + rhs.x, lhs.y + rhs.y);
}

inline float2 operator-(const float2& v, float val)
{
	return float2(v.x - val, v.y - val);
}

inline float2 operator-(float val, const float2& v)
{

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

inline float2 operator*(const float2& v, float val)
{
	return float2(v.x * val, v.y * val);
}

inline float2 operator*(float val, const float2& v)
{
	return float2(v.x * val, v.y * val);
}

inline float2 operator/(const float2& v, float val)
{
	assert(!approx_equal(val, 0.f));

	return float2(v.x / val, v.y / val);
}

inline float2 operator/(float val, const float2& v)
{
	return float2(val / v.x, val / v.y);
}

inline std::ostream& operator<<(std::ostream& out, const float2& v)
{
	out << "float2(" << v.x << ", " << v.y << ")";
	return out;
}

inline std::wostream& operator<<(std::wostream& out, const float2& v)
{
	out << "float2(" << v.x << ", " << v.y  << ")";
	return out;
}

inline float2 clamp(const float2& v, const float2& v_lo, const float2& v_hi)
{
	return float2(
		clamp(v.x, v_lo.x, v_hi.x),
		clamp(v.y, v_lo.y, v_hi.y)
	);
}

inline float dot(const float2& lhs, const float2& rhs)
{
	return (lhs.x * rhs.x) + (lhs.y * rhs.y);
}

inline bool greater_than(const float2& v, float val)
{
	return (v.x > val) && (v.y > val);
}

inline bool is_normalized(const float2& v)
{
	return approx_equal(len_squared(v), 1.f);
}

inline float len_squared(const float2& v)
{
	return (v.x * v.x) + (v.y * v.y);
}

inline float len(const float2& v)
{
	return std::sqrt(len_squared(v));
}

inline float2 lerp(const float2& lhs, const float2 rhs, float factor)
{
	assert(0.f <= factor && factor <= 1.f);
	return lhs + factor * (rhs - lhs);
}

inline float2 normalize(const float2& v)
{
	float l2 = len_squared(v);
	if (approx_equal(l2, 0.f) || approx_equal(l2, 1.f)) return v;

	float factor = 1.f / sqrt(l2);
	return v * factor;
}

} // namespace

#endif // CG_MATH_FLOAT2_H_
