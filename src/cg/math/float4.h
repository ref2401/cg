#ifndef CG_MATH_FLOAT4_H_
#define CG_MATH_FLOAT4_H_

#include <cassert>
#include <ostream>
#include "cg/math/float2.h"
#include "cg/math/float3.h"
#include "cg/math/utility.h"


namespace cg {

struct float4 {
	static const float4 unit_x;
	static const float4 unit_y;
	static const float4 unit_z;
	static const float4 unit_w;
	static const float4 unit_xyzw;
	static const float4 zero;


	float4() noexcept : x(0), y(0), z(0), w(0) {}

	explicit float4(float val) noexcept : x(val), y(val), z(val), w(val) {}

	explicit float4(const float3& v3, float w = 1.f) noexcept : x(v3.x), y(v3.y), z(v3.z), w(w) {}

	float4(float x, float y, float z, float w) noexcept : x(x), y(y), z(z), w(w) {}


	// Returns the first two components of this vector.
	inline float2 xy() const noexcept
	{
		return float2(x, y);
	}

	// Returns the first three components of this vector.
	inline float3 xyz() const noexcept
	{
		return float3(x, y, z);
	}

	// Returns the first three components of this vector.
	inline float3 rgb() const noexcept
	{
		return float3(r, g, b);
	}

	// Returns the first two components of this vector.
	float2 uv() const noexcept
	{
		return float2(x, y);
	}


	// Adds val to each component of the vector.
	float4& operator+=(float val) noexcept
	{
		x += val;
		y += val;
		z += val;
		w += val;
		return *this;
	}

	// Adds the specified vector v to this vector.
	float4& operator+=(const float4& v) noexcept
	{
		x += v.x;
		y += v.y;
		z += v.z;
		w += v.w;
		return *this;
	}

	// Subtracts val from every component of the vector.
	float4& operator-=(float val) noexcept
	{
		x -= val;
		y -= val;
		z -= val;
		w -= val;
		return *this;
	}

	// Subtracts  the specified vector v this vector.
	float4& operator-=(const float4& v) noexcept
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		w -= v.w;
		return *this;
	}

	// Multiplies each component of the vector by val.
	float4& operator*=(float val) noexcept
	{
		x *= val;
		y *= val;
		z *= val;
		w *= val;
		return *this;
	}

	// Devides each component of the vector by val.
	float4& operator/=(float val) noexcept
	{
		assert(!approx_equal(val, 0.f));

		x /= val;
		y /= val;
		z /= val;
		w /= val;
		return *this;
	}


	union {
		struct { float x, y, z, w; };
		struct { float r, g, b, a; };
	};
};

// Checks whether lhs is equal to rhs.
inline bool operator==(const float4& lhs, const float4& rhs) noexcept
{
	return approx_equal(lhs.x, rhs.x)
		&& approx_equal(lhs.y, rhs.y)
		&& approx_equal(lhs.z, rhs.z)
		&& approx_equal(lhs.w, rhs.w);
}

// Checks whether lhs is not equal to rhs.
inline bool operator!=(const float4& lhs, const float4& rhs) noexcept
{
	return !(lhs == rhs);
}

// Adds val to each component of v.
inline float4 operator+(const float4& v, float val) noexcept
{
	return float4(v.x + val, v.y + val, v.z + val, v.w + val);
}

// Adds val to each component of v.
inline float4 operator+(float val, const float4& v) noexcept
{
	return float4(v.x + val, v.y + val, v.z + val, v.w + val);
}

// Adds rhs vector to lhs.
inline float4 operator+(const float4& lhs, const float4& rhs) noexcept
{
	return float4(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w);
}

// Subtracts val from each component of v.
inline float4 operator-(const float4& v, float val) noexcept
{
	return float4(v.x - val, v.y - val, v.z - val, v.w - val);
}

// Subtracts each component of v from val.
inline float4 operator-(float val, const float4& v) noexcept
{
	return float4(val - v.x, val - v.y, val - v.z, val - v.w);
}

// Subtracts rhs vector from lhs.
inline float4 operator-(const float4& lhs, const float4& rhs) noexcept
{
	return float4(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w);
}

// Negates each component of v.
inline float4 operator-(const float4& v) noexcept
{
	return float4(-v.x, -v.y, -v.z, -v.w);
}

// Multiplies each component of v by val.
inline float4 operator*(const float4& v, float val) noexcept
{
	return float4(v.x * val, v.y * val, v.z * val, v.w * val);
}

// Multiplies each component of v by val.
inline float4 operator*(float val, const float4& v) noexcept
{
	return float4(v.x * val, v.y * val, v.z * val, v.w * val);
}

// Devides each component of v by val.
inline float4 operator/(const float4& v, float val) noexcept
{
	assert(!approx_equal(val, 0.f));
	return float4(v.x / val, v.y / val, v.z / val, v.w / val);
}

// Devides val by each component of v.
inline float4 operator/(float val, const float4& v) noexcept
{
	return float4(val / v.x, val / v.y, val / v.z, val / v.w);
}

inline std::ostream& operator<<(std::ostream& out, const float4& v)
{
	out << "float4(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
	return out;
}

inline std::wostream& operator<<(std::wostream& out, const float4& v)
{
	out << "float4(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
	return out;
}

// Constrains vector v to lie between two further vectors.
// The function processes each component of the vector separately.
// Params:
//		v = The value to constrain
//		v_lo =	The lower end of the range into which to constrain v.
//		v_hi = The upper end of the range into which to constrain v.
inline float4 clamp(const float4& v, const float4& v_lo = float4::zero, 
	const float4& v_hi = float4::unit_xyzw) noexcept
{
	return float4(
		clamp(v.x, v_lo.x, v_hi.x),
		clamp(v.y, v_lo.y, v_hi.y),
		clamp(v.z, v_lo.z, v_hi.z),
		clamp(v.w, v_lo.w, v_hi.w)
	);
}

// Calculates the dot product of the given vectors.
inline float dot(const float4& lhs, const float4& rhs) noexcept
{
	return (lhs.x * rhs.x) + (lhs.y * rhs.y) + (lhs.z * rhs.z) + (lhs.w * rhs.w);
}

// Returns true if every component of v is greater than val.
inline bool greater_than(const float4& v, float val) noexcept
{
	return (v.x > val) && (v.y > val) && (v.z > val) && (v.w > val);
}

// Calculates the squared length of v.
inline float len_squared(const float4& v) noexcept
{
	return (v.x * v.x) + (v.y * v.y) + (v.z * v.z) + (v.w * v.w);
}

// Calculates the length of v.
inline float len(const float4& v) noexcept
{
	return std::sqrt(len_squared(v));
}

// Checks whether the specified vector is normalized.
inline bool is_normalized(const float4& v) noexcept
{
	return approx_equal(len_squared(v), 1.f);
}

// Linearly interpolates between two values.
// Params:
//		lhs = The start of the range in which to interpolate.
//		rhs = The end of the range in which to interpolate.
//		factor = The value to use to interpolate between lhs & rhs.
//		factor has to lie within the range [0 .. 1].
inline float4 lerp(const float4& lhs, const float4 rhs, float factor) noexcept
{
	assert(0.f <= factor && factor <= 1.f);
	return lhs + factor * (rhs - lhs);
}

// Returns new vector which is normalized(unit length) copy of the given one.
inline float4 normalize(const float4& v) noexcept
{
	float l2 = len_squared(v);
	if (approx_equal(l2, 0.f) || approx_equal(l2, 1.f)) return v;

	float factor = 1.f / sqrt(l2);
	return v * factor;
}

// Returns rgb color volor
// red: (32..24) bytes, 
// green : (23..16) bytes,
// blue : (15..8) bytes,
// alpha : (7..0) bytes
inline float4 rgba(uint32_t val) noexcept
{
	return float4(
		((val >> 24) & 0xFF) / 255.f,
		((val >> 16) & 0xFF) / 255.f,
		((val >> 8) & 0xFF) / 255.f,
		(val & 0xFF) / 255.f
	);
}

} // namespace cg

#endif CG_MATH_FLOAT4_H_
