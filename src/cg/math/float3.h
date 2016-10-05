#ifndef CG_MATH_FLOAT3_H_
#define CG_MATH_FLOAT3_H_

#include <cassert>
#include <cmath>
#include <cstdint>
#include <ostream>
#include "cg/math/float2.h"
#include "cg/math/utility.h"


namespace cg {

struct float3 {
	static const float3 unit_x;
	static const float3 unit_y;
	static const float3 unit_z;
	static const float3 unit_xyz;
	static const float3 zero;


	float3() noexcept : x(0), y(0), z(0) {}

	explicit float3(float val) noexcept : x(val), y(val), z(val) {}

	float3(float x, float y, float z) noexcept : x(x), y(y), z(z) {}


	// Returns the first two components of this vector.
	float2 xy() const noexcept
	{
		return float2(x, y);
	}

	// Returns the first two components of this vector.
	float2 uv() const noexcept
	{
		return float2(x, y);
	}


	// Adds val to every component of the vector.
	float3& operator+=(float val) noexcept
	{
		x += val;
		y += val;
		z += val;
		return *this;
	}

	// Adds the specified vector v to this vector.
	float3& operator+=(const float3& v) noexcept
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	// Subtracts val from every component of the vector.
	float3& operator-=(float val) noexcept
	{
		x -= val;
		y -= val;
		z -= val;
		return *this;
	}

	// Subtracts  the specified vector v this vector.
	float3& operator-=(const float3& v) noexcept
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	// Multiplies each component of the vector by val.
	float3& operator*=(float val) noexcept
	{
		x *= val;
		y *= val;
		z *= val;
		return *this;
	}

	// Devides each component of the vector by val.
	float3& operator/=(float val) noexcept
	{
		assert(!approx_equal(val, 0.f));

		x /= val;
		y /= val;
		z /= val;
		return *this;
	}


	union {
		struct { float x, y, z; };
		struct { float r, g, b; };
		struct { float width, height, depth; };
	};
};

// Checks whether lhs is equal to rhs.
inline bool operator==(const float3& lhs, const float3& rhs) noexcept
{
	return approx_equal(lhs.x, rhs.x)
		&& approx_equal(lhs.y, rhs.y)
		&& approx_equal(lhs.z, rhs.z);
}

// Checks whether lhs is not equal to rhs.
inline bool operator!=(const float3& lhs, const float3& rhs) noexcept
{
	return !(lhs == rhs);
}

// Adds val to each component of v.
inline float3 operator+(const float3& v, float val) noexcept
{
	return float3(v.x + val, v.y + val, v.z + val);
}

// Adds val to each component of v.
inline float3 operator+(float val, const float3& v) noexcept
{
	return float3(v.x + val, v.y + val, v.z + val);
}

// Adds rhs vector to lhs.
inline float3 operator+(const float3& lhs, const float3& rhs) noexcept
{
	return float3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
}

// Subtracts val from each component of v.
inline float3 operator-(const float3& v, float val) noexcept
{
	return float3(v.x - val, v.y - val, v.z - val);
}

// Subtracts each component of v from val.
inline float3 operator-(float val, const float3& v) noexcept
{
	return float3(val - v.x, val - v.y, val - v.z);
}

// Subtracts rhs vector from lhs.
inline float3 operator-(const float3& lhs, const float3& rhs) noexcept
{
	return float3(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
}

// Negates each component of v.
inline float3 operator-(const float3& v) noexcept
{
	return float3(-v.x, -v.y, -v.z);
}

// Multiplies each component of v by val.
inline float3 operator*(const float3& v, float val) noexcept
{
	return float3(v.x * val, v.y * val, v.z * val);
}

// Multiplies each component of v by val.
inline float3 operator*(float val, const float3& v) noexcept
{
	return float3(v.x * val, v.y * val, v.z * val);
}

// Devides each component of v by val.
inline float3 operator/(const float3& v, float val) noexcept
{
	assert(!approx_equal(val, 0.f));

	return float3(v.x / val, v.y / val, v.z / val);
}

// Devides val by each component of v.
inline float3 operator/(float val, const float3& v) noexcept
{
	return float3(val / v.x, val / v.y, val / v.z);
}

inline std::ostream& operator<<(std::ostream& out, const float3& v) noexcept
{
	out << "float3(" << v.x << ", " << v.y << ", " << v.z << ")";
	return out;
}

inline std::wostream& operator<<(std::wostream& out, const float3& v) noexcept
{
	out << "float3(" << v.x << ", " << v.y << ", " << v.z << ")";
	return out;
}


// Constrains vector v to lie between two further vectors.
// The function processes each component of the vector separately.
// Params:
//		v = The value to constrain
//		v_lo =	The lower end of the range into which to constrain v.
//		v_hi = The upper end of the range into which to constrain v.
inline float3 clamp(const float3& v, const float3& v_lo = float3::zero, 
	const float3& v_hi = float3::unit_xyz)  noexcept
{
	return float3(
		clamp(v.x, v_lo.x, v_hi.x),
		clamp(v.y, v_lo.y, v_hi.y),
		clamp(v.z, v_lo.z, v_hi.z)
	);
}

// Calculates the cross product of of the given vectors.
inline float3 cross(const float3& lhs, const float3& rhs) noexcept
{
	return float3(
		(lhs.y *rhs.z) - (lhs.z * rhs.y),
		(lhs.z * rhs.x) - (lhs.x * rhs.z),
		(lhs.x * rhs.y) - (lhs.y * rhs.x)
	);
}

// Calculates the dot product of the given vectors.
inline float dot(const float3& lhs, const float3& rhs) noexcept
{
	return (lhs.x * rhs.x) + (lhs.y * rhs.y) + (lhs.z * rhs.z);
}

// Returns true if every component of v is greater than val.
inline bool greater_than(const float3& v, float val) noexcept
{
	return (v.x > val) && (v.y > val) && (v.z > val);
}

// Calculates the squared length of v.
inline float len_squared(const float3& v) noexcept
{
	return (v.x * v.x) + (v.y * v.y) + (v.z * v.z);
}

// Calculates the length of v.
inline float len(const float3& v) noexcept
{
	return std::sqrt(len_squared(v));
}

// Checks whether the specified vector is normalized.
inline bool is_normalized(const float3& v) noexcept
{
	return approx_equal(len_squared(v), 1.f);
}

// Linearly interpolates between two values.
// Params:
//		lhs = The start of the range in which to interpolate.
//		rhs = The end of the range in which to interpolate.
//		factor = The value to use to interpolate between lhs & rhs.
//		factor has to lie within the range [0 .. 1].
inline float3 lerp(const float3& lhs, const float3 rhs, float factor) noexcept
{
	assert(0.f <= factor && factor <= 1.f);
	return lhs + factor * (rhs - lhs);
}

// Returns new vector which is normalized(unit length) copy of the given one.
inline float3 normalize(const float3& v) noexcept
{
	float l2 = len_squared(v);
	if (approx_equal(l2, 0.f) || approx_equal(l2, 1.f)) return v;

	float factor = 1.f / sqrt(l2);
	return v * factor;
}

// Returns rgb color volor
// (31 .. 24) bytes are ignored.
// red: (23 .. 16) bytes. 
// green: (15 .. 8) bytes. 
// blue: (7 .. 1) bytes.
inline float3 rgb(uint32_t val) noexcept
{
	return float3(
		((val >> 16) & 0xFF) / 255.f,
		((val >> 8) & 0xFF) / 255.f,
		(val & 0xFF) / 255.f
	);
}

} // namespace cg

#endif // CG_MATH_FLOAT3_H_
