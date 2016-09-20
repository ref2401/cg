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


	float4() noexcept;

	explicit float4(float val) noexcept;

	explicit float4(const float3& v3, float w = 1.f) noexcept;

	float4(float x, float y, float z, float w) noexcept;


	// Returns the first two components of this vector.
	float2 xy() const;

	// Returns the first three components of this vector.
	float3 xyz() const;

	// Returns the first three components of this vector.
	float3 rgb() const;

	// Returns the first two components of this vector.
	float2 uv() const;


	// Adds val to each component of the vector.
	float4& operator+=(float val);

	// Adds the specified vector v to this vector.
	float4& operator+=(const float4& val);

	// Subtracts val from every component of the vector.
	float4& operator-=(float val);

	// Subtracts  the specified vector v this vector.
	float4& operator-=(const float4& val);

	// Multiplies each component of the vector by val.
	float4& operator*=(float val);

	// Devides each component of the vector by val.
	float4& operator/=(float val);


	union {
		struct { float x, y, z, w; };
		struct { float r, g, b, a; };
	};
};

// Checks whether lhs is equal to rhs.
bool operator==(const float4& lhs, const float4& rhs);

// Checks whether lhs is not equal to rhs.
bool operator!=(const float4& lhs, const float4& rhs);

// Adds val to each component of v.
float4 operator+(const float4& v, float val);

// Adds val to each component of v.
float4 operator+(float val, const float4& v);

// Adds rhs vector to lhs.
float4 operator+(const float4& lhs, const float4& rhs);

// Subtracts val from each component of v.
float4 operator-(const float4& v, float val);

// Subtracts each component of v from val.
float4 operator-(float val, float4 v);

// Subtracts rhs vector from lhs.
float4 operator-(float4 lhs, const float4& rhs);

// Negates each component of v.
float4 operator-(const float4& v);

// Multiplies each component of v by val.
float4 operator*(const float4& v, float val);

// Multiplies each component of v by val.
float4 operator*(float val, const float4& v);

// Devides each component of v by val.
float4 operator/(const float4& v, const float val);

// Devides val by each component of v.
float4 operator/(float val, const float4& v);

std::ostream& operator<<(std::ostream& out, const float4& v);

std::wostream& operator<<(std::wostream& out, const float4& v);

// Constrains vector v to lie between two further vectors.
// The function processes each component of the vector separately.
// Params:
//		v = The value to constrain
//		v_lo =	The lower end of the range into which to constrain v.
//		v_hi = The upper end of the range into which to constrain v.
float4 clamp(const float4& v, const float4& v_lo = float4::zero, const float4& v_hi = float4::unit_xyzw);

// Calculates the dot product of the given vectors.
float dot(const float4& lhs, const float4& rhs);

// Returns true if every component of v is greater than val.
bool greater_than(const float4& v, float val);

// Checks whether the specified vector is normalized.
bool is_normalized(const float4& v);

// Calculates the squared length of v.
float len_squared(const float4& v);

// Calculates the length of v.
float len(const float4& v);

// Linearly interpolates between two values.
// Params:
//		lhs = The start of the range in which to interpolate.
//		rhs = The end of the range in which to interpolate.
//		factor = The value to use to interpolate between lhs & rhs.
//		factor has to lie within the range [0 .. 1].
float4 lerp(const float4& lhs, const float4 rhs, float factor);

// Returns new vector which is normalized(unit length) copy of the given one.
float4 normalize(const float4& v);

// Returns rgb color volor
// red: (32..24) bytes, 
// green : (23..16) bytes,
// blue : (15..8) bytes,
// alpha : (7..0) bytes
float4 rgba(uint32_t val);



inline float4::float4() noexcept : x(0), y(0), z(0), w(0) {}

inline float4::float4(float val) noexcept : x(val), y(val), z(val), w(val) {}

inline float4::float4(const float3& v3, float w) noexcept : x(v3.x), y(v3.y), z(v3.z), w(w) {}

inline float4::float4(float x, float y, float z, float w) noexcept : x(x), y(y), z(z), w(w) {}


inline float2 float4::xy() const
{
	return float2(x, y);
}

inline float3 float4::xyz() const
{
	return float3(x, y, z);
}

inline float3 float4::rgb() const
{
	return float3(r, g, b);
}

inline float2 float4::uv() const
{
	return float2(x, y);
}


inline float4& float4::operator+=(float val)
{
	x += val;
	y += val;
	z += val;
	w += val;
	return *this;
}

inline float4& float4::operator+=(const float4& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	w += v.w;
	return *this;
}

inline float4& float4::operator-=(float val)
{
	x -= val;
	y -= val;
	z -= val;
	w -= val;
	return *this;
}

inline float4& float4::operator-=(const float4& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	w -= v.w;
	return *this;
}

inline float4& float4::operator*=(float val)
{
	x *= val;
	y *= val;
	z *= val;
	w *= val;
	return *this;
}

inline float4& float4::operator/=(float val)
{
	assert(!approx_equal(val, 0.f));

	x /= val;
	y /= val;
	z /= val;
	w /= val;
	return *this;
}


inline bool operator==(const float4& lhs, const float4& rhs)
{
	return approx_equal(lhs.x, rhs.x)
		&& approx_equal(lhs.y, rhs.y)
		&& approx_equal(lhs.z, rhs.z)
		&& approx_equal(lhs.w, rhs.w);
}

inline bool operator!=(const float4& lhs, const float4& rhs)
{
	return !(lhs == rhs);
}

inline float4 operator+(const float4& v, float val)
{
	return float4(v.x + val, v.y + val, v.z + val, v.w + val);
}

inline float4 operator+(float val, const float4& v)
{
	return float4(v.x + val, v.y + val, v.z + val, v.w + val);
}

inline float4 operator+(const float4& lhs, const float4& rhs)
{
	return float4(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w);
}

inline float4 operator-(const float4& v, float val)
{
	return float4(v.x - val, v.y - val, v.z - val, v.w - val);
}

inline float4 operator-(float val, float4 v)
{
	return float4(val - v.x, val - v.y, val - v.z, val - v.w);
}

inline float4 operator-(float4 lhs, const float4& rhs)
{
	return float4(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w);
}

inline float4 operator-(const float4& v)
{
	return float4(-v.x, -v.y, -v.z, -v.w);
}

inline float4 operator*(const float4& v, float val)
{
	return float4(v.x * val, v.y * val, v.z * val, v.w * val);
}

inline float4 operator*(float val, const float4& v)
{
	return float4(v.x * val, v.y * val, v.z * val, v.w * val);
}

inline float4 operator/(const float4& v, float val)
{
	assert(!approx_equal(val, 0.f));

	return float4(v.x / val, v.y / val, v.z / val, v.w / val);
}

inline float4 operator/(float val, const float4& v)
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

inline float4 clamp(const float4& v, const float4& v_lo, const float4& v_hi)
{
	return float4(
		clamp(v.x, v_lo.x, v_hi.x),
		clamp(v.y, v_lo.y, v_hi.y),
		clamp(v.z, v_lo.z, v_hi.z),
		clamp(v.w, v_lo.w, v_hi.w)
	);
}

inline float dot(const float4& lhs, const float4& rhs)
{
	return (lhs.x * rhs.x) + (lhs.y * rhs.y) + (lhs.z * rhs.z) + (lhs.w * rhs.w);
}

inline bool greater_than(const float4& v, float val)
{
	return (v.x > val) && (v.y > val) && (v.z > val) && (v.w > val);
}

inline bool is_normalized(const float4& v)
{
	return approx_equal(len_squared(v), 1.f);
}

inline float len_squared(const float4& v)
{
	return (v.x * v.x) + (v.y * v.y) + (v.z * v.z) + (v.w * v.w);
}

inline float len(const float4& v)
{
	return std::sqrt(len_squared(v));
}

inline float4 lerp(const float4& lhs, const float4 rhs, float factor)
{
	assert(0.f <= factor && factor <= 1.f);
	return lhs + factor * (rhs - lhs);
}

inline float4 normalize(const float4& v)
{
	float l2 = len_squared(v);
	if (approx_equal(l2, 0.f) || approx_equal(l2, 1.f)) return v;

	float factor = 1.f / sqrt(l2);
	return v * factor;
}

inline float4 rgba(uint32_t val)
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
