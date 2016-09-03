#ifndef CG_MATH_FLOAT_4_H_
#define CG_MATH_FLOAT_4_H_

#include <type_traits>
#include "cg/math/float_3.h"

namespace cg {

struct float_4 {
	static const float_4 zero;
	static const float_4 unit_x;
	static const float_4 unit_y;
	static const float_4 unit_z;
	static const float_4 unit_w;
	static const float_4 unit_xyzw;


	float_4();

	explicit float_4(float val);

	explicit float_4(const float_3& v3, float w = 1.f);

	float_4(float x, float y, float z, float w);


	// Returns the first three components of this vector.
	float_3 xyz() const;

	// Returns the first three components of this vector.
	float_3 rgb() const;


	// Adds val to every component of the vector.
	template<typename Numeric>
	float_4& operator+=(const Numeric& val);

	// Adds the specified vector v to this vector.
	float_4& operator+=(const float_4& val);

	// Subtracts val from every component of the vector.
	template<typename Numeric>
	float_4& operator-=(const Numeric& val);

	// Subtracts  the specified vector v this vector.
	float_4& operator-=(const float_4& val);

	// Multiplies every component of the vector by val.
	template<typename Numeric>
	float_4& operator*=(const Numeric& val);

	// Devides every component of the vector by val.
	template<typename Numeric>
	float_4& operator/=(const Numeric& val);


	union {
		struct { float x, y, z, w; };
		struct { float r, g, b, a; };
	};
};

inline float_4::float_4() : x(0), y(0), z(0), w(0) {}

inline float_4::float_4(float val) : x(val), y(val), z(val), w(val) {}

inline float_4::float_4(const float_3& v3, float w)
	: x(v3.x), y(v3.y), z(v3.z), w(w) {}

inline float_4::float_4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}


inline float_3 float_4::xyz() const
{
	return float_3(x, y, z);
}

inline float_3 float_4::rgb() const
{
	return float_3(r, g, b);
}


template<typename Numeric>
inline float_4& float_4::operator+=(const Numeric& val)
{
	static_assert(std::is_integral<Numeric>::value || std::is_floating_point<Numeric>::value,
		"Numeric type must be an integer or floating point value.");
	x += val;
	y += val;
	z += val;
	w += val;
	return *this;
}

inline float_4& float_4::operator+=(const float_4& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	w += v.w;
	return *this;
}

template<typename Numeric>
inline float_4& float_4::operator-=(const Numeric& val)
{
	static_assert(std::is_integral<Numeric>::value || std::is_floating_point<Numeric>::value,
		"Numeric type must be an integer or floating point value.");
	x -= val;
	y -= val;
	z -= val;
	w -= val;
	return *this;
}

inline float_4& float_4::operator-=(const float_4& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	w -= v.w;
	return *this;
}

template<typename Numeric>
inline float_4& float_4::operator*=(const Numeric& val)
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
inline float_4& float_4::operator/=(const Numeric& val)
{
	static_assert(std::is_integral<Numeric>::value || std::is_floating_point<Numeric>::value,
		"Numeric type must be an integer or floating point value.");
	x /= val;
	y /= val;
	z /= val;
	w /= val;
	return *this;
}



inline bool operator==(const float_4& lhs, const float_4& rhs)
{
	return approx_equal(lhs.x, rhs.x)
		&& approx_equal(lhs.y, rhs.y)
		&& approx_equal(lhs.z, rhs.z)
		&& approx_equal(lhs.w, rhs.w);
}

inline bool operator!=(const float_4& lhs, const float_4& rhs)
{
	return !(lhs == rhs);
}

// operator+
template<typename Numeric>
inline float_4 operator+(float_4 v, Numeric val)
{
	static_assert(std::is_integral<Numeric>::value || std::is_floating_point<Numeric>::value,
		"Numeric type must be an integer or floating point value.");
	return v += val;
}

template<typename Numeric>
inline float_4 operator+(Numeric val, float_4 v)
{
	static_assert(std::is_integral<Numeric>::value || std::is_floating_point<Numeric>::value,
		"Numeric type must be an integer or floating point value.");
	return v += val;
}

inline float_4 operator+(float_4 lhs, const float_4& rhs)
{
	return lhs += rhs;
}

// operator-
template<typename Numeric>
inline float_4 operator-(float_4 v, Numeric val)
{
	static_assert(std::is_integral<Numeric>::value || std::is_floating_point<Numeric>::value,
		"Numeric type must be an integer or floating point value.");
	return v -= val;
}

template<typename Numeric>
inline float_4 operator-(Numeric val, float_4 v)
{
	static_assert(std::is_integral<Numeric>::value || std::is_floating_point<Numeric>::value,
		"Numeric type must be an integer or floating point value.");
	return float_4(val - v.x, val - v.y, val - v.z, val - v.w);
}

inline float_4 operator-(float_4 lhs, const float_4& rhs)
{
	return lhs -= rhs;
}

// unary operator-
inline float_4 operator-(const float_4& v)
{
	return float_4(-v.x, -v.y, -v.z, -v.w);
}

//operator*
template<typename Numeric>
inline float_4 operator*(float_4 v, Numeric val)
{
	static_assert(std::is_integral<Numeric>::value || std::is_floating_point<Numeric>::value,
		"Numeric type must be an integer or floating point value.");
	return v *= val;
}

template<typename Numeric>
inline float_4 operator*(Numeric val, float_4 v)
{
	static_assert(std::is_integral<Numeric>::value || std::is_floating_point<Numeric>::value,
		"Numeric type must be an integer or floating point value.");
	return v *= val;
}

// operator/
template<typename Numeric>
inline float_4 operator/(float_4 v, Numeric val)
{
	static_assert(std::is_integral<Numeric>::value || std::is_floating_point<Numeric>::value,
		"Numeric type must be an integer or floating point value.");
	return v /= val;
}

template<typename Numeric>
inline float_4 operator/(Numeric val, float_4 v)
{
	static_assert(std::is_integral<Numeric>::value || std::is_floating_point<Numeric>::value,
		"Numeric type must be an integer or floating point value.");
	return float_4(val / v.x, val / v.y, val / v.z, val / v.w);
}

inline std::ostream& operator<<(std::ostream& out, const cg::float_4& v)
{
	out << "float_4(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
	return out;
}

inline std::wostream& operator<<(std::wostream& out, const cg::float_4& v)
{
	out << "float_4(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
	return out;
}


// Constrains vector v to lie between two further vectors.
// The function processes each component of the vector separately.
// Params:
//		v = The value to constrain
//		v_min =	The lower end of the range into which to constrain v.
//		v_max = The upper end of the range into which to constrain v.
inline float_4 clamp(const float_4& v,
	const float_4& v_lo = float_4::zero, const float_4& v_hi = float_4::unit_xyzw)
{
	assert(v_lo.x <= v_hi.x);
	assert(v_lo.y <= v_hi.y);
	assert(v_lo.z <= v_hi.z);
	assert(v_lo.w <= v_hi.w);

	return float_4(
		std::min(v_hi.x, std::max(v_lo.x, v.x)),
		std::min(v_hi.y, std::max(v_lo.y, v.y)),
		std::min(v_hi.z, std::max(v_lo.z, v.z)),
		std::min(v_hi.w, std::max(v_lo.w, v.w))
	);
}

// Calculates the dot product of the given vectors.
inline float dot(const float_4& lhs, const float_4& rhs)
{
	return (lhs.x * rhs.x) + (lhs.y * rhs.y) + (lhs.z * rhs.z) + (lhs.w * rhs.w);
}

// Calculates the squared length of v.
inline float len_square(const float_4& v)
{
	return (v.x * v.x) + (v.y * v.y) + (v.z * v.z) + (v.w * v.w);
}

// Calculates the length of v.
inline float len(const float_4& v)
{
	return std::sqrt(len_square(v));
}

// Linearly interpolates between two values.
// Params:
//		lhs = The start of the range in which to interpolate.
//		rhs = The end of the range in which to interpolate.
//		factor = The value to use to interpolate between lhs & rhs.
//		factor has to lie within the range [0 .. 1].
inline float_4 lerp(const float_4& lhs, const float_4 rhs, float factor)
{
	assert(0.f <= factor && factor <= 1.f);
	return lhs + factor * (rhs - lhs);
}

// Returns new vector which is normalized(unit length) copy of the given one.
inline float_4 normalize(const float_4& v)
{
	float l2 = len_square(v);
	if (approx_equal(l2, 0.f) || approx_equal(l2, 1.f)) return v;

	float factor = 1.f / sqrt(l2);
	return v * factor;
}

// Returns rgb color volor
// red: (32..24) bytes, 
// green : (23..16) bytes,
// blue : (15..8) bytes,
// alpha : (7..0) bytes
inline float_4 rgba(uint32_t val)
{
	return float_4(
		((val >> 24) & 0xFF) / 255.f,
		((val >> 16) & 0xFF) / 255.f,
		((val >> 8) & 0xFF) / 255.f,
		(val & 0xFF) / 255.f
	);
}

} // namespace cg

#endif CG_MATH_FLOAT_4_H_
