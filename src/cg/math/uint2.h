#ifndef CG_MATH_UINT2_H_
#define CG_MATH_UINT2_H_

#include <cassert>
#include <cstdint>
#include <type_traits>
#include <iostream>


namespace cg {

struct uint2 {
	static const uint2 zero;


	uint2();

	explicit uint2(uint32_t v);

	uint2(uint32_t x, uint32_t y);


	// Calculates the proportional relationship between this.width and this.height.
	template<typename TRet = float>
	TRet aspect_ratio() const;

	// Calculates the square of a rect this.width x this.height.
	template<typename TRet = uint32_t>
	TRet square() const;


	// Adds val to every component of the vector.
	uint2& operator+=(uint32_t val);

	// Adds the specified vector v to this vector.
	uint2& operator+=(const uint2& val);

	// Subtracts val from every component of the vector.
	uint2& operator-=(uint32_t val);

	// Subtracts  the specified vector v this vector.
	uint2& operator-=(const uint2& val);

	// Multiplies each component of the vector by val.
	uint2& operator*=(uint32_t val);

	// Devides each component of the vector by val.s
	uint2& operator/=(uint32_t val);


	union {
		struct { uint32_t x, y; };
		struct { uint32_t width, height; };
	};
};

// Checks whether lhs is equal to rhs.
bool operator==(const uint2& lhs, const uint2& rhs);

// Checks whether lhs is not equal to rhs.
bool operator!=(const uint2& lhs, const uint2& rhs);

// Adds val to each component of v.
uint2 operator+(const uint2& v, uint32_t val);

// Adds val to each component of v.
uint2 operator+(uint32_t val, const uint2& v);

// Adds rhs vector to lhs.
uint2 operator+(const uint2& lhs, const uint2& rhs);

// Subtracts val from each component of v.
uint2 operator-(const uint2& v, uint32_t val);

// Subtracts each component of v from val.
uint2 operator-(uint32_t val, const uint2& v);

// Subtracts rhs vector from lhs.
uint2 operator-(const uint2& lhs, const uint2& rhs);

// Multiplies each component of v by val.
uint2 operator*(const uint2& v, uint32_t val);

// Multiplies each component of v by val.
uint2 operator*(uint32_t val, const uint2& v);

// Devides each component of v by val.
uint2 operator/(const uint2& v, uint32_t val);

// Devides val by each component of v.
uint2 operator/(uint32_t val, const uint2& v);

std::ostream& operator<<(std::ostream& out, const uint2& v);

std::wostream& operator<<(std::wostream& out, const uint2& v);

// Checks whether each component of v is greater than the specified value val.
bool greater_than(const uint2& v, uint32_t val);



inline uint2::uint2() : x(0), y(0) {}

inline uint2::uint2(uint32_t v) : x(v), y(v) {}

inline uint2::uint2(uint32_t x, uint32_t y) : x(x), y(y) {}


template<typename TRet>
TRet uint2::aspect_ratio() const
{
	static_assert(std::is_integral<TRet>::value || std::is_floating_point<TRet>::value,
		"TRet type must be an integer or a floating point value.");
	return static_cast<TRet>(static_cast<TRet>(width) / height);
}

template<typename TRet>
TRet uint2::square() const 
{
	static_assert(std::is_integral<TRet>::value || std::is_floating_point<TRet>::value,
		"TRet type must be an integer or a floating point value.");
	return static_cast<TRet>(static_cast<TRet>(width) * height);
}


inline uint2& uint2::operator+=(uint32_t val)
{
	x += val;
	y += val;
	return *this;
}

inline uint2& uint2::operator+=(const uint2& v)
{
	x += v.x;
	y += v.y;
	return *this;
}

inline uint2& uint2::operator-=(uint32_t val)
{
	assert(x >= val);
	assert(y >= val);

	x -= val;
	y -= val;
	return *this;
}

inline uint2& uint2::operator-=(const uint2& v)
{
	assert(x >= v.x);
	assert(y >= v.y);

	x -= v.x;
	y -= v.y;
	return *this;
}

inline uint2& uint2::operator*=(uint32_t val)
{
	x *= val;
	y *= val;
	return *this;
}

inline uint2& uint2::operator/=(uint32_t val)
{
	assert(val != 0);

	x /= val;
	y /= val;
	return *this;
}


inline bool operator==(const uint2& lhs, const uint2& rhs)
{
	return (lhs.x == rhs.x) && (lhs.y == rhs.y);
}

inline bool operator!=(const uint2& lhs, const uint2& rhs)
{
	return !(lhs == rhs);
}

inline uint2 operator+(const uint2& v, uint32_t val)
{
	return uint2(v.x + val, v.y + val);
}

inline uint2 operator+(uint32_t val, const uint2& v)
{
	return uint2(v.x + val, v.y + val);
}

inline uint2 operator+(const uint2& lhs, const uint2& rhs)
{
	return uint2(lhs.x + rhs.x, lhs.y + rhs.y);
}

inline uint2 operator-(const uint2& v, uint32_t val)
{
	assert(v.x >= val);
	assert(v.y >= val);

	return uint2(v.x - val, v.y - val);
}

inline uint2 operator-(uint32_t val, const uint2& v)
{
	assert(val >= v.x);
	assert(val >= v.y);

	return uint2(val - v.x, val - v.y);
}

inline uint2 operator-(const uint2& lhs, const uint2& rhs)
{
	assert(lhs.x >= rhs.x);
	assert(lhs.y >= rhs.y);

	return uint2(lhs.x - rhs.x, lhs.y - rhs.y);
}

inline uint2 operator*(const uint2& v, uint32_t val)
{
	return uint2(v.x * val, v.y * val);
}

inline uint2 operator*(uint32_t val, const uint2& v)
{
	return uint2(v.x * val, v.y * val);
}

inline uint2 operator/(const uint2& v, uint32_t val)
{
	assert(val != 0);

	return uint2(v.x / val, v.y / val);
}

inline uint2 operator/(uint32_t val, const uint2& v)
{
	return uint2(val / v.x, val / v.y);
}

inline std::ostream& operator<<(std::ostream& out, const uint2& v)
{
	out << "uint2(" << v.x << ", " << v.y << ")";
	return out;
}

inline std::wostream& operator<<(std::wostream& out, const uint2& v)
{
	out << "uint2(" << v.x << ", " << v.y << ")";
	return out;
}


inline bool greater_than(const uint2& v, uint32_t val)
{
	return (v.x > val) && (v.y > val);
}

} // namespace cg

#endif // CG_MATH_UINT2_H_
