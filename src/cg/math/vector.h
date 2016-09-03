#ifndef CG_MATH_VECTOR_H_
#define CG_MATH_VECTOR_H_

#include <cmath>
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

	explicit float_3(float v);

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
		struct {
			float x;
			float y;
			float z;
		};

		struct {
			float r;
			float g;
			float b;
		};

		struct {
			float width;
			float height;
			float depth;
		};
	};
};

inline float_3::float_3() : x(0), y(0), z(0) {}

inline float_3::float_3(float v) : x(v), y(v), z(v) {}

inline float_3::float_3(float x, float y, float z) : x(x), y(y), z(z) {}

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


// Calculates a dot product of the given vectors.
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
	return std::sqrt(cg::len_square(v));
}


} // namespace cg

#endif // CG_MATH_VECTOR_H_
