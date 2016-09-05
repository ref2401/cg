#ifndef CG_MATH_FLOAT4_H_
#define CG_MATH_FLOAT4_H_

#include <type_traits>
#include "cg/math/float3.h"
#include "cg/math/utils.h"

namespace cg {

struct float4 {
	static const float4 unit_x;
	static const float4 unit_y;
	static const float4 unit_z;
	static const float4 unit_w;
	static const float4 unit_xyzw;
	static const float4 zero;


	float4();

	explicit float4(float val);

	explicit float4(const float3& v3, float w = 1.f);

	float4(float x, float y, float z, float w);


	// Returns the first three components of this vector.
	float3 xyz() const;

	// Returns the first three components of this vector.
	float3 rgb() const;


	// Adds val to each component of the vector.
	template<typename Numeric>
	float4& operator+=(const Numeric& val);

	// Adds the specified vector v to this vector.
	float4& operator+=(const float4& val);

	// Subtracts val from every component of the vector.
	template<typename Numeric>
	float4& operator-=(const Numeric& val);

	// Subtracts  the specified vector v this vector.
	float4& operator-=(const float4& val);

	// Multiplies each component of the vector by val.
	template<typename Numeric>
	float4& operator*=(const Numeric& val);

	// Devides each component of the vector by val.
	template<typename Numeric>
	float4& operator/=(const Numeric& val);


	union {
		struct { float x, y, z, w; };
		struct { float r, g, b, a; };
	};
};

inline float4::float4() : x(0), y(0), z(0), w(0) {}

inline float4::float4(float val) : x(val), y(val), z(val), w(val) {}

inline float4::float4(const float3& v3, float w)
	: x(v3.x), y(v3.y), z(v3.z), w(w) {}

inline float4::float4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}


inline float3 float4::xyz() const
{
	return float3(x, y, z);
}

inline float3 float4::rgb() const
{
	return float3(r, g, b);
}


template<typename Numeric>
inline float4& float4::operator+=(const Numeric& val)
{
	static_assert(std::is_integral<Numeric>::value || std::is_floating_point<Numeric>::value,
		"Numeric type must be an integer or floating point value.");
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

template<typename Numeric>
inline float4& float4::operator-=(const Numeric& val)
{
	static_assert(std::is_integral<Numeric>::value || std::is_floating_point<Numeric>::value,
		"Numeric type must be an integer or floating point value.");
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

template<typename Numeric>
inline float4& float4::operator*=(const Numeric& val)
{
	static_assert(std::is_integral<Numeric>::value || std::is_floating_point<Numeric>::value,
		"Numeric type must be an integer or floating point value.");
	x *= val;
	y *= val;
	z *= val;
	w *= val;
	return *this;
}

template<typename Numeric>
inline float4& float4::operator/=(const Numeric& val)
{
	static_assert(std::is_integral<Numeric>::value || std::is_floating_point<Numeric>::value,
		"Numeric type must be an integer or floating point value.");
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

template<typename Numeric>
inline float4 operator+(const float4& v, const Numeric& val)
{
	static_assert(std::is_integral<Numeric>::value || std::is_floating_point<Numeric>::value,
		"Numeric type must be an integer or floating point value.");
	return float4(v.x + val, v.y + val, v.z + val, v.w + val);
}

template<typename Numeric>
inline float4 operator+(const Numeric& val, const float4& v)
{
	static_assert(std::is_integral<Numeric>::value || std::is_floating_point<Numeric>::value,
		"Numeric type must be an integer or floating point value.");
	return float4(v.x + val, v.y + val, v.z + val, v.w + val);
}

inline float4 operator+(const float4& lhs, const float4& rhs)
{
	return float4(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w);
}

template<typename Numeric>
inline float4 operator-(const float4& v, const Numeric& val)
{
	static_assert(std::is_integral<Numeric>::value || std::is_floating_point<Numeric>::value,
		"Numeric type must be an integer or floating point value.");
	return float4(v.x - val, v.y - val, v.z - val, v.w - val);
}

template<typename Numeric>
inline float4 operator-(const Numeric& val, float4 v)
{
	static_assert(std::is_integral<Numeric>::value || std::is_floating_point<Numeric>::value,
		"Numeric type must be an integer or floating point value.");
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

template<typename Numeric>
inline float4 operator*(const float4& v, const Numeric& val)
{
	static_assert(std::is_integral<Numeric>::value || std::is_floating_point<Numeric>::value,
		"Numeric type must be an integer or floating point value.");
	return float4(v.x * val, v.y * val, v.z * val, v.w * val);
}

template<typename Numeric>
inline float4 operator*(const Numeric& val, const float4& v)
{
	static_assert(std::is_integral<Numeric>::value || std::is_floating_point<Numeric>::value,
		"Numeric type must be an integer or floating point value.");
	return float4(v.x * val, v.y * val, v.z * val, v.w * val);
}

template<typename Numeric>
inline float4 operator/(const float4& v, const Numeric& val)
{
	static_assert(std::is_integral<Numeric>::value || std::is_floating_point<Numeric>::value,
		"Numeric type must be an integer or floating point value.");
	return float4(v.x / val, v.y / val, v.z / val, v.w / val);
}

template<typename Numeric>
inline float4 operator/(const Numeric& val, const float4& v)
{
	static_assert(std::is_integral<Numeric>::value || std::is_floating_point<Numeric>::value,
		"Numeric type must be an integer or floating point value.");
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
inline float4 clamp(const float4& v,
	const float4& v_lo = float4::zero, const float4& v_hi = float4::unit_xyzw)
{
	return float4(
		clamp(v.x, v_lo.x, v_hi.x),
		clamp(v.y, v_lo.y, v_hi.y),
		clamp(v.z, v_lo.z, v_hi.z),
		clamp(v.w, v_lo.w, v_hi.w)
	);
}

// Calculates the dot product of the given vectors.
inline float dot(const float4& lhs, const float4& rhs)
{
	return (lhs.x * rhs.x) + (lhs.y * rhs.y) + (lhs.z * rhs.z) + (lhs.w * rhs.w);
}

// Calculates the squared length of v.
inline float len_squared(const float4& v)
{
	return (v.x * v.x) + (v.y * v.y) + (v.z * v.z) + (v.w * v.w);
}

// Calculates the length of v.
inline float len(const float4& v)
{
	return std::sqrt(len_squared(v));
}

// Checks whether the specified vector is normalized.
inline bool is_normalized(const float4& v)
{
	return approx_equal(len_squared(v), 1.f);
}

// Linearly interpolates between two values.
// Params:
//		lhs = The start of the range in which to interpolate.
//		rhs = The end of the range in which to interpolate.
//		factor = The value to use to interpolate between lhs & rhs.
//		factor has to lie within the range [0 .. 1].
inline float4 lerp(const float4& lhs, const float4 rhs, float factor)
{
	assert(0.f <= factor && factor <= 1.f);
	return lhs + factor * (rhs - lhs);
}

// Returns new vector which is normalized(unit length) copy of the given one.
inline float4 normalize(const float4& v)
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
