#ifndef CG_MATH_UINT2_H_
#define CG_MATH_UINT2_H_

#include <cassert>
#include <cstdint>
#include <type_traits>
#include <iostream>


namespace cg {

struct uint2 {
	static const uint2 zero;
	static const uint2 unit_x;
	static const uint2 unit_y;
	static const uint2 unit_xy;


	uint2() noexcept : x(0), y(0) {}

	explicit uint2(uint32_t v) noexcept : x(v), y(v) {}

	uint2(uint32_t x, uint32_t y) noexcept : x(x), y(y) {}


	// Adds val to every component of the vector.
	uint2& operator+=(uint32_t val) noexcept
	{
		x += val;
		y += val;
		return *this;
	}

	// Adds the specified vector v to this vector.
	uint2& operator+=(const uint2& v) noexcept
	{
		x += v.x;
		y += v.y;
		return *this;
	}

	// Subtracts val from every component of the vector.
	uint2& operator-=(uint32_t val) noexcept
	{
		assert(x >= val);
		assert(y >= val);

		x -= val;
		y -= val;
		return *this;
	}

	// Subtracts  the specified vector v this vector.
	uint2& operator-=(const uint2& v) noexcept
	{
		assert(x >= v.x);
		assert(y >= v.y);

		x -= v.x;
		y -= v.y;
		return *this;
	}

	// Multiplies each component of the vector by val.
	uint2& operator*=(uint32_t val) noexcept
	{
		x *= val;
		y *= val;
		return *this;
	}

	// Devides each component of the vector by val.s
	uint2& operator/=(uint32_t val) noexcept
	{
		assert(val != 0);

		x /= val;
		y /= val;
		return *this;
	}


	// Calculates the proportional relationship between this.width and this.height.
	template<typename TRet = float>
	TRet aspect_ratio() const noexcept
	{
		static_assert(std::is_integral<TRet>::value || std::is_floating_point<TRet>::value,
			"TRet type must be an integer or a floating point value.");
		return static_cast<TRet>(static_cast<TRet>(width) / height);
	}

	// Calculates the square of a rect this.width x this.height.
	template<typename TRet = uint32_t>
	TRet square() const noexcept
	{
		static_assert(std::is_integral<TRet>::value || std::is_floating_point<TRet>::value,
			"TRet type must be an integer or a floating point value.");
		return static_cast<TRet>(static_cast<TRet>(width) * height);
	}


	union {
		struct { uint32_t x, y; };
		struct { uint32_t width, height; };
	};
};


// Checks whether lhs is equal to rhs.
inline bool operator==(const uint2& lhs, const uint2& rhs) noexcept
{
	return (lhs.x == rhs.x) && (lhs.y == rhs.y);
}

// Checks whether lhs is not equal to rhs.
inline bool operator!=(const uint2& lhs, const uint2& rhs) noexcept
{
	return !(lhs == rhs);
}

// Adds val to each component of v.
inline uint2 operator+(const uint2& v, uint32_t val) noexcept
{
	return uint2(v.x + val, v.y + val);
}

// Adds val to each component of v.
inline uint2 operator+(uint32_t val, const uint2& v) noexcept
{
	return uint2(v.x + val, v.y + val);
}

// Adds rhs vector to lhs.
inline uint2 operator+(const uint2& lhs, const uint2& rhs) noexcept
{
	return uint2(lhs.x + rhs.x, lhs.y + rhs.y);
}

// Subtracts val from each component of v.
inline uint2 operator-(const uint2& v, uint32_t val) noexcept
{
	assert(v.x >= val);
	assert(v.y >= val);

	return uint2(v.x - val, v.y - val);
}

// Subtracts each component of v from val.
inline uint2 operator-(uint32_t val, const uint2& v) noexcept
{
	assert(val >= v.x);
	assert(val >= v.y);
	return uint2(val - v.x, val - v.y);
}

// Subtracts rhs vector from lhs.
inline  uint2 operator-(const uint2& lhs, const uint2& rhs) noexcept
{
	assert(lhs.x >= rhs.x);
	assert(lhs.y >= rhs.y);
	return uint2(lhs.x - rhs.x, lhs.y - rhs.y);
}

// Multiplies each component of v by val.
inline uint2 operator*(const uint2& v, uint32_t val) noexcept
{
	return uint2(v.x * val, v.y * val);
}

// Multiplies each component of v by val.
inline uint2 operator*(uint32_t val, const uint2& v) noexcept
{
	return uint2(v.x * val, v.y * val);
}

// Devides each component of v by val.
inline uint2 operator/(const uint2& v, uint32_t val) noexcept
{
	assert(val != 0);
	return uint2(v.x / val, v.y / val);
}

// Devides val by each component of v.
inline uint2 operator/(uint32_t val, const uint2& v) noexcept
{
	return uint2(val / v.x, val / v.y);
}

std::ostream& operator<<(std::ostream& out, const uint2& v);

std::wostream& operator<<(std::wostream& out, const uint2& v);

// Checks whether each component of v is greater than the specified value val.
inline bool greater_than(const uint2& v, uint32_t val) noexcept
{
	return (v.x > val) && (v.y > val);
}


} // namespace cg

#endif // CG_MATH_UINT2_H_
