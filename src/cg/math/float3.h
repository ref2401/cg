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


	float3() noexcept;

	explicit float3(float val) noexcept;

	float3(float x, float y, float z) noexcept;


	// Returns the first two components of this vector.
	float2 xy() const;

	// Returns the first two components of this vector.
	float2 uv() const;


	// Adds val to every component of the vector.
	float3& operator+=(float val);

	// Adds the specified vector v to this vector.
	float3& operator+=(const float3& val);

	// Subtracts val from every component of the vector.
	float3& operator-=(float val);

	// Subtracts  the specified vector v this vector.
	float3& operator-=(const float3& val);

	// Multiplies each component of the vector by val.
	float3& operator*=(float val);

	// Devides each component of the vector by val.
	float3& operator/=(float val);


	union {
		struct { float x, y, z; };
		struct { float r, g, b; };
		struct { float width, height, depth; };
	};
};

// Checks whether lhs is equal to rhs.
bool operator==(const float3& lhs, const float3& rhs);

// Checks whether lhs is not equal to rhs.
bool operator!=(const float3& lhs, const float3& rhs);

// Adds val to each component of v.
float3 operator+(const float3& v, float val);

// Adds val to each component of v.
float3 operator+(float& val, const float3& v);

// Adds rhs vector to lhs.
float3 operator+(const float3& lhs, const float3& rhs);

// Subtracts val from each component of v.
float3 operator-(const float3& v, float val);

// Subtracts each component of v from val.
float3 operator-(float val, const float3& v);

// Subtracts rhs vector from lhs.
float3 operator-(const float3& lhs, const float3& rhs);

// Negates each component of v.
float3 operator-(const float3& v);

// Multiplies each component of v by val.
float3 operator*(const float3& v, float val);

// Multiplies each component of v by val.
float3 operator*(float val, const float3& v);

// Devides each component of v by val.
float3 operator/(const float3& v, float val);

// Devides val by each component of v.
float3 operator/(float val, const float3& v);

std::ostream& operator<<(std::ostream& out, const float3& v);

std::wostream& operator<<(std::wostream& out, const float3& v);

// Constrains vector v to lie between two further vectors.
// The function processes each component of the vector separately.
// Params:
//		v = The value to constrain
//		v_lo =	The lower end of the range into which to constrain v.
//		v_hi = The upper end of the range into which to constrain v.
float3 clamp(const float3& v, const float3& v_lo = float3::zero, const float3& v_hi = float3::unit_xyz);

// Calculates the cross product of of the given vectors.
float3 cross(const float3& lhs, const float3& rhs);

// Calculates the dot product of the given vectors.
float dot(const float3& lhs, const float3& rhs);

// Returns true if every component of v is greater than val.
bool greater_than(const float3& v, float val);

// Checks whether the specified vector is normalized.
bool is_normalized(const float3& v);

// Calculates the squared length of v.
float len_squared(const float3& v);

// Calculates the length of v.
float len(const float3& v);

// Linearly interpolates between two values.
// Params:
//		lhs = The start of the range in which to interpolate.
//		rhs = The end of the range in which to interpolate.
//		factor = The value to use to interpolate between lhs & rhs.
//		factor has to lie within the range [0 .. 1].
float3 lerp(const float3& lhs, const float3 rhs, float factor);

// Returns new vector which is normalized(unit length) copy of the given one.
float3 normalize(const float3& v);

// Returns rgb color volor
// (31 .. 24) bytes are ignored.
// red: (23 .. 16) bytes. 
// green: (15 .. 8) bytes. 
// blue: (7 .. 1) bytes.
float3 rgb(uint32_t val);



inline float3::float3() noexcept : x(0), y(0), z(0) {}

inline float3::float3(float val) noexcept : x(val), y(val), z(val) {}

inline float3::float3(float x, float y, float z) noexcept : x(x), y(y), z(z) {}


inline float2 float3::xy() const
{
	return float2(x, y);
}

inline float2 float3::uv() const
{
	return float2(x, y);
}


inline float3& float3::operator+=(float val)
{
	x += val;
	y += val;
	z += val;
	return *this;
}

inline float3& float3::operator+=(const float3& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}

inline float3& float3::operator-=(float val)
{
	x -= val;
	y -= val;
	z -= val;
	return *this;
}

inline float3& float3::operator-=(const float3& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}

inline float3& float3::operator*=(float val)
{
	x *= val;
	y *= val;
	z *= val;
	return *this;
}

inline float3& float3::operator/=(float val)
{
	assert(!approx_equal(val, 0.f));

	x /= val;
	y /= val;
	z /= val;
	return *this;
}


inline bool operator==(const float3& lhs, const float3& rhs)
{
	return approx_equal(lhs.x, rhs.x)
		&& approx_equal(lhs.y, rhs.y)
		&& approx_equal(lhs.z, rhs.z);
}

inline bool operator!=(const float3& lhs, const float3& rhs)
{
	return !(lhs == rhs);
}

inline float3 operator+(const float3& v, float val)
{
	return float3(v.x + val, v.y + val, v.z + val);
}

inline float3 operator+(float val, const float3& v)
{
	return float3(v.x + val, v.y + val, v.z + val);
}

inline float3 operator+(const float3& lhs, const float3& rhs)
{
	return float3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
}

inline float3 operator-(const float3& v, float val)
{
	return float3(v.x - val, v.y - val, v.z - val);
}

inline float3 operator-(float val, const float3& v)
{
	return float3(val - v.x, val - v.y, val - v.z);
}

inline float3 operator-(const float3& lhs, const float3& rhs)
{
	return float3(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
}

inline float3 operator-(const float3& v)
{
	return float3(-v.x, -v.y, -v.z);
}

inline float3 operator*(const float3& v, float val)
{
	return float3(v.x * val, v.y * val, v.z * val);
}

inline float3 operator*(float val, const float3& v)
{
	return float3(v.x * val, v.y * val, v.z * val);
}

inline float3 operator/(const float3& v, float val)
{
	assert(!approx_equal(val, 0.f));

	return float3(v.x / val, v.y / val, v.z / val);
}

inline float3 operator/(float val, const float3& v)
{
	return float3(val / v.x, val / v.y, val / v.z);
}

inline std::ostream& operator<<(std::ostream& out, const float3& v)
{
	out << "float3(" << v.x << ", " << v.y << ", " << v.z << ")";
	return out;
}

inline std::wostream& operator<<(std::wostream& out, const float3& v)
{
	out << "float3(" << v.x << ", " << v.y << ", " << v.z << ")";
	return out;
}


inline float3 clamp(const float3& v, const float3& v_lo, const float3& v_hi)
{
	return float3(
		clamp(v.x, v_lo.x, v_hi.x),
		clamp(v.y, v_lo.y, v_hi.y),
		clamp(v.z, v_lo.z, v_hi.z)
	);
}

inline float3 cross(const float3& lhs, const float3& rhs)
{
	return float3(
		(lhs.y *rhs.z) - (lhs.z * rhs.y),
		(lhs.z * rhs.x) - (lhs.x * rhs.z),
		(lhs.x * rhs.y) - (lhs.y * rhs.x)
	);
}

inline float dot(const float3& lhs, const float3& rhs)
{
	return (lhs.x * rhs.x) + (lhs.y * rhs.y) + (lhs.z * rhs.z);
}

inline bool greater_than(const float3& v, float val)
{
	return (v.x > val) && (v.y > val) && (v.z > val);
}

inline bool is_normalized(const float3& v)
{
	return approx_equal(len_squared(v), 1.f);
}

inline float len_squared(const float3& v)
{
	return (v.x * v.x) + (v.y * v.y) + (v.z * v.z);
}

inline float len(const float3& v)
{
	return std::sqrt(len_squared(v));
}

inline float3 lerp(const float3& lhs, const float3 rhs, float factor)
{
	assert(0.f <= factor && factor <= 1.f);
	return lhs + factor * (rhs - lhs);
}

inline float3 normalize(const float3& v)
{
	float l2 = len_squared(v);
	if (approx_equal(l2, 0.f) || approx_equal(l2, 1.f)) return v;

	float factor = 1.f / sqrt(l2);
	return v * factor;
}

inline float3 rgb(uint32_t val)
{
	return float3(
		((val >> 16) & 0xFF) / 255.f,
		((val >> 8) & 0xFF) / 255.f,
		(val & 0xFF) / 255.f
	);
}

} // namespace cg

#endif // CG_MATH_FLOAT3_H_
