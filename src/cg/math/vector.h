#ifndef CG_MATH_VECTOR_H_
#define CG_MATH_VECTOR_H_

#include <cassert>
#include <cmath>
#include <cstdint>
#include <ostream>
#include <type_traits>
#include "cg/math/utility.h"


namespace cg {

struct float2 {
	static const float2 unit_x;
	static const float2 unit_y;
	static const float2 unit_xy;
	static const float2 zero;


	float2() noexcept : x(0), y(0) {}

	explicit float2(float val) noexcept : x(val), y(val) {}

	float2(float x, float y) noexcept : x(x), y(y) {}


	float2& operator+=(float val) noexcept
	{
		x += val;
		y += val;
		return *this;
	}

	float2& operator+=(const float2& v) noexcept
	{
		x += v.x;
		y += v.y;
		return *this;
	}

	float2& operator-=(float val) noexcept
	{
		x -= val;
		y -= val;
		return *this;
	}

	float2& operator-=(const float2& v) noexcept
	{
		x -= v.x;
		y -= v.y;
		return *this;
	}

	float2& operator*=(float val) noexcept
	{
		x *= val;
		y *= val;
		return *this;
	}

	float2& operator/=(float val) noexcept
	{
		assert(!approx_equal(val, 0.f));

		x /= val;
		y /= val;
		return *this;
	}


	union {
		struct { float x, y; };
		struct { float u, v; };
		float data[2];
	};
};

struct float3 {
	static const float3 unit_x;
	static const float3 unit_y;
	static const float3 unit_z;
	static const float3 unit_xy;
	static const float3 unit_xyz;
	static const float3 zero;


	float3() noexcept : x(0), y(0), z(0) {}

	explicit float3(float val) noexcept : x(val), y(val), z(val) {}

	float3(const float2& v, float z) : x(v.x), y(v.y), z(z) {}

	float3(float x, float y, float z) noexcept : x(x), y(y), z(z) {}


	float3& operator+=(float val) noexcept
	{
		x += val;
		y += val;
		z += val;
		return *this;
	}

	float3& operator+=(const float3& v) noexcept
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	float3& operator-=(float val) noexcept
	{
		x -= val;
		y -= val;
		z -= val;
		return *this;
	}

	float3& operator-=(const float3& v) noexcept
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	float3& operator*=(float val) noexcept
	{
		x *= val;
		y *= val;
		z *= val;
		return *this;
	}

	float3& operator/=(float val) noexcept
	{
		assert(!approx_equal(val, 0.f));

		x /= val;
		y /= val;
		z /= val;
		return *this;
	}


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


	union {
		struct { float x, y, z; };
		struct { float r, g, b; };
		struct { float width, height, depth; };
		float data[3];
	};
};

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


	float4& operator+=(float val) noexcept
	{
		x += val;
		y += val;
		z += val;
		w += val;
		return *this;
	}

	float4& operator+=(const float4& v) noexcept
	{
		x += v.x;
		y += v.y;
		z += v.z;
		w += v.w;
		return *this;
	}

	float4& operator-=(float val) noexcept
	{
		x -= val;
		y -= val;
		z -= val;
		w -= val;
		return *this;
	}

	float4& operator-=(const float4& v) noexcept
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		w -= v.w;
		return *this;
	}

	float4& operator*=(float val) noexcept
	{
		x *= val;
		y *= val;
		z *= val;
		w *= val;
		return *this;
	}

	float4& operator/=(float val) noexcept
	{
		assert(!approx_equal(val, 0.f));

		x /= val;
		y /= val;
		z /= val;
		w /= val;
		return *this;
	}


	// Returns the first two components of this vector.
	float2 xy() const noexcept
	{
		return float2(x, y);
	}

	// Returns the first three components of this vector.
	float3 xyz() const noexcept
	{
		return float3(x, y, z);
	}

	// Returns the first three components of this vector.
	float3 rgb() const noexcept
	{
		return float3(r, g, b);
	}

	// Returns the first two components of this vector.
	float2 uv() const noexcept
	{
		return float2(x, y);
	}


	union {
		struct { float x, y, z, w; };
		struct { float r, g, b, a; };
		float data[4];
	};
};

// In mathematics, the quaternions are a number system that extends the complex numbers.
// Quaternions extends a rotation in three dimensions to a rotation in four dimensions.
// This avoids "gimbal lock" and allows for smooth continuous rotation.
// q = xi + yj + zk + a1, where x, y, z, a are real numbers and i, j, k are imaginary units.
// For any quaternion 'a' is called scalar part and 'xi + yj + zk' is called its vector part.
struct quat {
	static const quat i;
	static const quat j;
	static const quat k;
	static const quat identity;
	static const quat zero;


	quat() noexcept : x(0), y(0), z(0), a(0) {}

	quat(float x, float y, float z, float a) noexcept : x(x), y(y), z(z), a(a) {}

	quat(const float3& v, float a) noexcept : x(v.x), y(v.y), z(v.z), a(a) {}


	quat& operator+=(const quat& q) noexcept
	{
		x += q.x;
		y += q.y;
		z += q.z;
		a += q.a;
		return *this;
	}

	quat& operator-=(const quat& q) noexcept
	{
		x -= q.x;
		y -= q.y;
		z -= q.z;
		a -= q.a;
		return *this;
	}

	quat& operator*=(float val) noexcept
	{
		x *= val;
		y *= val;
		z *= val;
		a *= val;
		return *this;
	}

	// Calculates the Hamilton product of this and the specified quaterions.
	// Stores the result in this quaternion.
	quat& operator*=(const quat& q) noexcept
	{
		float xp = (a * q.x) + (x * q.a) + (y * q.z) - (z * q.y);
		float yp = (a * q.y) + (y * q.a) + (z * q.x) - (x * q.z);
		float zp = (a * q.z) + (z * q.a) + (x * q.y) - (y * q.x);
		float ap = (a * q.a) - (x * q.x) - (y * q.y) - (z * q.z);

		x = xp;
		y = yp;
		z = zp;
		a = ap;
		return *this;
	}

	quat& operator/=(float val) noexcept
	{
		assert(!approx_equal(val, 0.f));

		x /= val;
		y /= val;
		z /= val;
		a /= val;
		return *this;
	}


	float x, y, z, a;
};

struct ubyte4 {
	static const ubyte4 unit_x;
	static const ubyte4 unit_y;
	static const ubyte4 unit_z;
	static const ubyte4 unit_w;
	static const ubyte4 unit_xyzw;
	static const ubyte4 zero;


	ubyte4() noexcept : x(0), y(0), z(0), w(0) {}

	explicit ubyte4(uint8_t v) noexcept : x(v), y(v), z(v), w(v) {}

	ubyte4(uint8_t x, uint8_t y, uint8_t z, uint8_t w) noexcept : x(x), y(y), z(z), w(w) {}


	ubyte4& operator+=(uint8_t val) noexcept
	{
		x += val;
		y += val;
		z += val;
		w += val;
		return *this;
	}
	
	ubyte4& operator+=(const ubyte4& v) noexcept
	{
		x += v.x;
		y += v.y;
		z += v.z;
		w += v.w;
		return *this;
	}
	
	ubyte4& operator-=(uint8_t val) noexcept
	{
		assert(x >= val);
		assert(y >= val);
		assert(z >= val);
		assert(w >= val);

		x -= val;
		y -= val;
		z -= val;
		w -= val;
		return *this;
	}
	
	ubyte4& operator-=(const ubyte4& v) noexcept
	{
		assert(x >= v.x);
		assert(y >= v.y);
		assert(z >= v.z);
		assert(w >= v.w);

		x -= v.x;
		y -= v.y;
		z -= v.z;
		w -= v.w;
		return *this;
	}
	
	ubyte4& operator*=(uint8_t val) noexcept
	{
		x *= val;
		y *= val;
		z *= val;
		w *= val;
		return *this;
	}
	
	ubyte4& operator/=(uint8_t val) noexcept
	{
		assert(val != 0);

		x /= val;
		y /= val;
		z /= val;
		w /= val;
		return *this;
	}


	union {
		struct { uint8_t x, y, z, w; };
		struct { uint8_t r, g, b, a; };
		uint8_t data[4];
	};
};

struct uint2 {
	static const uint2 zero;
	static const uint2 unit_x;
	static const uint2 unit_y;
	static const uint2 unit_xy;


	uint2() noexcept : x(0), y(0) {}

	explicit uint2(uint32_t v) noexcept : x(v), y(v) {}

	uint2(uint32_t x, uint32_t y) noexcept : x(x), y(y) {}


	uint2& operator+=(uint32_t val) noexcept
	{
		x += val;
		y += val;
		return *this;
	}

	uint2& operator+=(const uint2& v) noexcept
	{
		x += v.x;
		y += v.y;
		return *this;
	}

	uint2& operator-=(uint32_t val) noexcept
	{
		assert(x >= val);
		assert(y >= val);

		x -= val;
		y -= val;
		return *this;
	}

	uint2& operator-=(const uint2& v) noexcept
	{
		assert(x >= v.x);
		assert(y >= v.y);

		x -= v.x;
		y -= v.y;
		return *this;
	}

	uint2& operator*=(uint32_t val) noexcept
	{
		x *= val;
		y *= val;
		return *this;
	}

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
		uint32_t data[2];
	};
};

struct uint4 final {
	static const uint4 unit_x;
	static const uint4 unit_y;
	static const uint4 unit_z;
	static const uint4 unit_w;
	static const uint4 unit_xyzw;
	static const uint4 zero;


	uint4() noexcept : x(0), y(0), z(0), w(0) {}

	explicit uint4(uint32_t v) noexcept : x(v), y(v), z(v), w(v) {}

	uint4(uint32_t x, uint32_t y, uint32_t z, uint32_t w) noexcept 
		: x(x), y(y), z(z), w(w)
	{}


	uint4& operator+=(uint32_t val) noexcept
	{
		x += val;
		y += val;
		z += val;
		w += val;
		return *this;
	}

	uint4& operator+=(const uint4& v) noexcept
	{
		x += v.x;
		y += v.y;
		z += v.z;
		w += v.w;
		return *this;
	}

	uint4& operator-=(uint32_t val) noexcept
	{
		assert(x >= val);
		assert(y >= val);
		assert(z >= val);
		assert(w >= val);

		x -= val;
		y -= val;
		z -= val;
		w -= val;
		return *this;
	}

	uint4& operator-=(const uint4& v) noexcept
	{
		assert(x >= v.x);
		assert(y >= v.y);
		assert(z >= v.z);
		assert(w >= v.w);

		x -= v.x;
		y -= v.y;
		z -= v.z;
		w -= v.w;
		return *this;
	}

	uint4& operator*=(uint32_t val) noexcept
	{
		x *= val;
		y *= val;
		z *= val;
		w *= val;
		return *this;
	}

	uint4& operator/=(uint32_t val) noexcept
	{
		assert(val != 0);

		x /= val;
		y /= val;
		z /= val;
		w /= val;
		return *this;
	}


	union {
		struct { uint32_t x, y, z, w; };
		struct { uint32_t r, g, b, a; };
		uint32_t data[4];
	};
};

inline bool operator==(const float2& lhs, const float2& rhs) noexcept
{
	return approx_equal(lhs.x, rhs.x)
		&& approx_equal(lhs.y, rhs.y);
}

inline bool operator!=(const float2& lhs, const float2& rhs) noexcept
{
	return !(lhs == rhs);
}

inline bool operator==(const float3& lhs, const float3& rhs) noexcept
{
	return approx_equal(lhs.x, rhs.x)
		&& approx_equal(lhs.y, rhs.y)
		&& approx_equal(lhs.z, rhs.z);
}

inline bool operator!=(const float3& lhs, const float3& rhs) noexcept
{
	return !(lhs == rhs);
}

inline bool operator==(const float4& lhs, const float4& rhs) noexcept
{
	return approx_equal(lhs.x, rhs.x)
		&& approx_equal(lhs.y, rhs.y)
		&& approx_equal(lhs.z, rhs.z)
		&& approx_equal(lhs.w, rhs.w);
}

inline bool operator!=(const float4& lhs, const float4& rhs) noexcept
{
	return !(lhs == rhs);
}

inline bool operator==(const quat& lhs, const quat& rhs)
{
	return approx_equal(lhs.x, rhs.x)
		&& approx_equal(lhs.y, rhs.y)
		&& approx_equal(lhs.z, rhs.z)
		&& approx_equal(lhs.a, rhs.a);
}

inline bool operator!=(const quat& lhs, const quat& rhs)
{
	return !(lhs == rhs);
}

inline bool operator==(const ubyte4& lhs, const ubyte4& rhs) noexcept
{
	return (lhs.x == rhs.x)
		&& (lhs.y == rhs.y)
		&& (lhs.z == rhs.z)
		&& (lhs.w == rhs.w);
}

inline bool operator!=(const ubyte4& lhs, const ubyte4& rhs) noexcept
{
	return !(lhs == rhs);
}

inline bool operator==(const uint2& lhs, const uint2& rhs) noexcept
{
	return (lhs.x == rhs.x) && (lhs.y == rhs.y);
}

inline bool operator!=(const uint2& lhs, const uint2& rhs) noexcept
{
	return !(lhs == rhs);
}

inline bool operator==(const uint4& lhs, const uint4& rhs) noexcept
{
	return (lhs.x == rhs.x)
		&& (lhs.y == rhs.y)
		&& (lhs.z == rhs.z)
		&& (lhs.w == rhs.w);
}

inline bool operator!=(const uint4& lhs, const uint4& rhs) noexcept
{
	return !(lhs == rhs);
}

inline float2 operator+(const float2& v, float val) noexcept
{
	return float2(v.x + val, v.y + val);
}
	
inline float2 operator+(float val, const float2& v) noexcept
{
	return float2(v.x + val, v.y + val);
}

inline float2 operator+(const float2& lhs, const float2& rhs) noexcept
{
	return float2(lhs.x + rhs.x, lhs.y + rhs.y);
}

inline float3 operator+(const float3& v, float val) noexcept
{
	return float3(v.x + val, v.y + val, v.z + val);
}

inline float3 operator+(float val, const float3& v) noexcept
{
	return float3(v.x + val, v.y + val, v.z + val);
}

inline float3 operator+(const float3& lhs, const float3& rhs) noexcept
{
	return float3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
}

inline float4 operator+(const float4& v, float val) noexcept
{
	return float4(v.x + val, v.y + val, v.z + val, v.w + val);
}

inline float4 operator+(float val, const float4& v) noexcept
{
	return float4(v.x + val, v.y + val, v.z + val, v.w + val);
}

inline float4 operator+(const float4& lhs, const float4& rhs) noexcept
{
	return float4(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w);
}

inline quat operator+(const quat& lhs, const quat& rhs)
{
	return quat(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.a + rhs.a);
}

inline ubyte4 operator+(const ubyte4& v, uint8_t val) noexcept
{
	return ubyte4(v.x + val, v.y + val, v.z + val, v.w + val);
}

inline ubyte4 operator+(uint8_t val, const ubyte4& v) noexcept
{
	return ubyte4(v.x + val, v.y + val, v.z + val, v.w + val);
}

inline ubyte4 operator+(const ubyte4& lhs, const ubyte4& rhs) noexcept
{
	return ubyte4(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w);
}

inline uint2 operator+(const uint2& v, uint32_t val) noexcept
{
	return uint2(v.x + val, v.y + val);
}

inline uint2 operator+(uint32_t val, const uint2& v) noexcept
{
	return uint2(v.x + val, v.y + val);
}

inline uint2 operator+(const uint2& lhs, const uint2& rhs) noexcept
{
	return uint2(lhs.x + rhs.x, lhs.y + rhs.y);
}

inline uint4 operator+(const uint4& v, uint32_t val) noexcept
{
	return uint4(v.x + val, v.y + val, v.z + val, v.w + val);
}

inline uint4 operator+(uint32_t val, const uint4& v) noexcept
{
	return uint4(v.x + val, v.y + val, v.z + val, v.w + val);
}

inline uint4 operator+(const uint4& lhs, const uint4& rhs) noexcept
{
	return uint4(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w);
}

inline float2 operator-(const float2& v, float val) noexcept
{
	return float2(v.x - val, v.y - val);
}

inline float2 operator-(float val, const float2& v) noexcept
{
	return float2(val - v.x, val - v.y);
}

inline float2 operator-(const float2& lhs, const float2& rhs) noexcept
{
	return float2(lhs.x - rhs.x, lhs.y - rhs.y);
}

inline float3 operator-(const float3& v, float val) noexcept
{
	return float3(v.x - val, v.y - val, v.z - val);
}

inline float3 operator-(float val, const float3& v) noexcept
{
	return float3(val - v.x, val - v.y, val - v.z);
}

inline float3 operator-(const float3& lhs, const float3& rhs) noexcept
{
	return float3(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
}

inline float4 operator-(const float4& v, float val) noexcept
{
	return float4(v.x - val, v.y - val, v.z - val, v.w - val);
}

inline float4 operator-(float val, const float4& v) noexcept
{
	return float4(val - v.x, val - v.y, val - v.z, val - v.w);
}

inline float4 operator-(const float4& lhs, const float4& rhs) noexcept
{
	return float4(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w);
}

inline quat operator-(const quat& lhs, const quat& rhs)
{
	return quat(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.a - rhs.a);
}

inline ubyte4 operator-(const ubyte4& v, uint8_t val) noexcept
{
	assert(v.x >= val);
	assert(v.y >= val);
	assert(v.z >= val);
	assert(v.w >= val);
	return ubyte4(v.x - val, v.y - val, v.z - val, v.w - val);
}

inline ubyte4 operator-(uint8_t val, const ubyte4& v) noexcept
{
	assert(val >= v.x);
	assert(val >= v.y);
	assert(val >= v.z);
	assert(val >= v.w);
	return ubyte4(val - v.x, val - v.y, val - v.z, val - v.w);
}

inline ubyte4 operator-(const ubyte4& lhs, const ubyte4& rhs) noexcept
{
	assert(lhs.x >= rhs.x);
	assert(lhs.y >= rhs.y);
	assert(lhs.z >= rhs.z);
	assert(lhs.w >= rhs.w);
	return ubyte4(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w);
}

inline uint2 operator-(const uint2& v, uint32_t val) noexcept
{
	assert(v.x >= val);
	assert(v.y >= val);

	return uint2(v.x - val, v.y - val);
}

inline uint2 operator-(uint32_t val, const uint2& v) noexcept
{
	assert(val >= v.x);
	assert(val >= v.y);
	return uint2(val - v.x, val - v.y);
}

inline  uint2 operator-(const uint2& lhs, const uint2& rhs) noexcept
{
	assert(lhs.x >= rhs.x);
	assert(lhs.y >= rhs.y);
	return uint2(lhs.x - rhs.x, lhs.y - rhs.y);
}

inline uint4 operator-(const uint4& v, uint32_t val) noexcept
{
	assert(v.x >= val);
	assert(v.y >= val);
	assert(v.z >= val);
	assert(v.w >= val);
	return uint4(v.x - val, v.y - val, v.z - val, v.w - val);
}

inline uint4 operator-(uint32_t val, const uint4& v) noexcept
{
	assert(val >= v.x);
	assert(val >= v.y);
	assert(val >= v.z);
	assert(val >= v.w);
	return uint4(val - v.x, val - v.y, val - v.z, val - v.w);
}

inline uint4 operator-(const uint4& lhs, const uint4& rhs) noexcept
{
	assert(lhs.x >= rhs.x);
	assert(lhs.y >= rhs.y);
	assert(lhs.z >= rhs.z);
	assert(lhs.w >= rhs.w);
	return uint4(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w);
}

// Negates each component of v.
inline float2 operator-(const float2& v) noexcept
{
	return float2(-v.x, -v.y);
}

// Negates each component of v.
inline float3 operator-(const float3& v) noexcept
{
	return float3(-v.x, -v.y, -v.z);
}

// Negates each component of v.
inline float4 operator-(const float4& v) noexcept
{
	return float4(-v.x, -v.y, -v.z, -v.w);
}

inline quat operator-(const quat& q)
{
	return quat(-q.x, -q.y, -q.z, -q.a);
}

inline float2 operator*(const float2& v, float val) noexcept
{
	return float2(v.x * val, v.y * val);
}

inline float2 operator*(float val, const float2& v) noexcept
{
	return float2(v.x * val, v.y * val);
}

inline float3 operator*(const float3& v, float val) noexcept
{
	return float3(v.x * val, v.y * val, v.z * val);
}

inline float3 operator*(float val, const float3& v) noexcept
{
	return float3(v.x * val, v.y * val, v.z * val);
}

inline float4 operator*(const float4& v, float val) noexcept
{
	return float4(v.x * val, v.y * val, v.z * val, v.w * val);
}

inline float4 operator*(float val, const float4& v) noexcept
{
	return float4(v.x * val, v.y * val, v.z * val, v.w * val);
}

inline quat operator*(const quat& q, float val) noexcept
{
	return quat(q.x * val, q.y * val, q.z * val, q.a * val);
}

inline quat operator*(float val, const quat& q) noexcept
{
	return quat(q.x * val, q.y * val, q.z * val, q.a * val);
}

// Calculates the Hamilton product of lsh and rhs quaternions.
inline quat operator*(const quat& lhs, const quat& rhs) noexcept
{
	return quat(
		(lhs.a * rhs.x) + (lhs.x * rhs.a) + (lhs.y * rhs.z) - (lhs.z * rhs.y),
		(lhs.a * rhs.y) + (lhs.y * rhs.a) + (lhs.z * rhs.x) - (lhs.x * rhs.z),
		(lhs.a * rhs.z) + (lhs.z * rhs.a) + (lhs.x * rhs.y) - (lhs.y * rhs.x),
		(lhs.a * rhs.a) - (lhs.x * rhs.x) - (lhs.y * rhs.y) - (lhs.z * rhs.z)
	);
}

inline ubyte4 operator*(const ubyte4& v, uint8_t val) noexcept
{
	return ubyte4(v.x * val, v.y * val, v.z * val, v.w * val);
}

inline ubyte4 operator*(uint8_t val, const ubyte4& v) noexcept
{
	return ubyte4(v.x * val, v.y * val, v.z * val, v.w * val);
}

inline uint2 operator*(const uint2& v, uint32_t val) noexcept
{
	return uint2(v.x * val, v.y * val);
}

inline uint2 operator*(uint32_t val, const uint2& v) noexcept
{
	return uint2(v.x * val, v.y * val);
}

inline uint4 operator*(const uint4& v, uint32_t val) noexcept
{
	return uint4(v.x * val, v.y * val, v.z * val, v.w * val);
}

inline uint4 operator*(uint32_t val, const uint4& v) noexcept
{
	return uint4(v.x * val, v.y * val, v.z * val, v.w * val);
}

inline float2 operator/(const float2& v, float val) noexcept
{
	assert(!approx_equal(val, 0.f));

	return float2(v.x / val, v.y / val);
}

inline float2 operator/(float val, const float2& v) noexcept
{
	return float2(val / v.x, val / v.y);
}

inline float3 operator/(const float3& v, float val) noexcept
{
	assert(!approx_equal(val, 0.f));

	return float3(v.x / val, v.y / val, v.z / val);
}

inline float3 operator/(float val, const float3& v) noexcept
{
	return float3(val / v.x, val / v.y, val / v.z);
}

inline float4 operator/(const float4& v, float val) noexcept
{
	assert(!approx_equal(val, 0.f));
	return float4(v.x / val, v.y / val, v.z / val, v.w / val);
}

inline float4 operator/(float val, const float4& v) noexcept
{
	return float4(val / v.x, val / v.y, val / v.z, val / v.w);
}

inline quat operator/(const quat& q, float val) noexcept
{
	assert(!approx_equal(val, 0.f));

	return quat(q.x / val, q.y / val, q.z / val, q.a / val);
}

inline quat operator/(float val, const quat& q) noexcept
{
	return quat(val / q.x, val / q.y, val / q.z, val / q.a);
}

inline ubyte4 operator/(const ubyte4& v, uint8_t val) noexcept
{
	assert(val != 0);
	return ubyte4(v.x / val, v.y / val, v.z / val, v.w / val);
}

inline ubyte4 operator/(uint8_t val, const ubyte4& v) noexcept
{
	assert(v.x != 0);
	assert(v.y != 0);
	assert(v.z != 0);
	assert(v.w != 0);
	return ubyte4(val / v.x, val / v.y, val / v.z, val / v.w);
}

inline uint2 operator/(const uint2& v, uint32_t val) noexcept
{
	assert(val != 0);
	return uint2(v.x / val, v.y / val);
}

inline uint2 operator/(uint32_t val, const uint2& v) noexcept
{
	return uint2(val / v.x, val / v.y);
}

inline uint4 operator/(const uint4& v, uint32_t val) noexcept
{
	assert(val != 0);
	return uint4(v.x / val, v.y / val, v.z / val, v.w / val);
}

inline uint4 operator/(uint32_t val, const uint4& v) noexcept
{
	assert(v.x != 0);
	assert(v.y != 0);
	assert(v.z != 0);
	assert(v.w != 0);
	return uint4(val / v.x, val / v.y, val / v.z, val / v.w);
}

std::ostream& operator<<(std::ostream& out, const float2& v);

std::wostream& operator<<(std::wostream& out, const float2& v);

std::ostream& operator<<(std::ostream& out, const float3& v);

std::wostream& operator<<(std::wostream& out, const float3& v);

std::ostream& operator<<(std::ostream& out, const float4& v);

std::wostream& operator<<(std::wostream& out, const float4& v);

std::ostream& operator<<(std::ostream& out, const quat& q);

std::wostream& operator<<(std::wostream& out, const quat& q);

std::ostream& operator<<(std::ostream& out, const ubyte4& v);

std::wostream& operator<<(std::wostream& out, const ubyte4& v);

std::ostream& operator<<(std::ostream& out, const uint2& v);

std::wostream& operator<<(std::wostream& out, const uint2& v);

std::ostream& operator<<(std::ostream& out, const uint4& v);

std::wostream& operator<<(std::wostream& out, const uint4& v);

// Constrains vector v to lie between two further vectors.
// The function processes each component of the vector separately.
// Params:
//		v = The value to constrain
//		v_lo =	The lower end of the range into which to constrain v.
//		v_hi = The upper end of the range into which to constrain v.
inline float2 clamp(const float2& v, const float2& v_lo = float2::zero,
	const float2& v_hi = float2::unit_xy) noexcept
{
	return float2(
		clamp(v.x, v_lo.x, v_hi.x),
		clamp(v.y, v_lo.y, v_hi.y)
	);
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

// Gets the conjugation result of the given quaternion.
inline quat conjugate(const quat& q) noexcept
{
	return quat(-q.x, -q.y, -q.z, q.a);
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
inline float dot(const float2& lhs, const float2& rhs) noexcept
{
	return (lhs.x * rhs.x) + (lhs.y * rhs.y);
}

// Calculates the dot product of the given vectors.
inline float dot(const float3& lhs, const float3& rhs) noexcept
{
	return (lhs.x * rhs.x) + (lhs.y * rhs.y) + (lhs.z * rhs.z);
}

// Calculates the dot product of the given vectors.
inline float dot(const float4& lhs, const float4& rhs) noexcept
{
	return (lhs.x * rhs.x) + (lhs.y * rhs.y) + (lhs.z * rhs.z) + (lhs.w * rhs.w);
}

// Returns true if every component of v is greater than val.
inline bool greater_than(const float2& v, float val) noexcept
{
	return (v.x > val) && (v.y > val);
}

// Returns true if every component of v is greater than val.
inline bool greater_than(const float3& v, float val) noexcept
{
	return (v.x > val) && (v.y > val) && (v.z > val);
}

// Returns true if every component of v is greater than val.
inline bool greater_than(const float4& v, float val) noexcept
{
	return (v.x > val) && (v.y > val) && (v.z > val) && (v.w > val);
}

// Checks whether each component of v is greater than the specified value val.
inline bool greater_than(const ubyte4& v, uint8_t val) noexcept
{
	return (v.x > val) && (v.y > val) && (v.z > val) && (v.w > val);
}

// Checks whether each component of v is greater than the specified value val.
inline bool greater_than(const uint2& v, uint32_t val) noexcept
{
	return (v.x > val) && (v.y > val);
}

// Returns true if every component of v is greater than val.
inline bool greater_than(const uint4& v, uint32_t val) noexcept
{
	return (v.x > val) && (v.y > val) && (v.z > val) && (v.w > val);
}

// Calculates the squared length of v.
inline float len_squared(const float2& v) noexcept
{
	return (v.x * v.x) + (v.y * v.y);
}

// Calculates the squared length of v.
inline float len_squared(const float3& v) noexcept
{
	return (v.x * v.x) + (v.y * v.y) + (v.z * v.z);
}

// Calculates the squared length of v.
inline float len_squared(const float4& v) noexcept
{
	return (v.x * v.x) + (v.y * v.y) + (v.z * v.z) + (v.w * v.w);
}

// Calculates the squared length of q.
inline float len_squared(const quat& q) noexcept
{
	return (q.x * q.x) + (q.y * q.y) + (q.z * q.z) + (q.a * q.a);
}

// Calculates the length of v.
inline float len(const float2& v) noexcept
{
	return std::sqrt(len_squared(v));
}

// Calculates the length of v.
inline float len(const float3& v) noexcept
{
	return std::sqrt(len_squared(v));
}

// Calculates the length of v.
inline float len(const float4& v) noexcept
{
	return std::sqrt(len_squared(v));
}

// Calculates the length of q.
inline float len(const quat& q) noexcept
{
	return std::sqrt(len_squared(q));
}

// Checks whether the specified vector is normalized.
inline bool is_normalized(const float2& v) noexcept
{
	return approx_equal(len_squared(v), 1.f);
}

// Checks whether the specified vector is normalized.
inline bool is_normalized(const float3& v) noexcept
{
	return approx_equal(len_squared(v), 1.f);
}

// Checks whether the specified vector is normalized.
inline bool is_normalized(const float4& v) noexcept
{
	return approx_equal(len_squared(v), 1.f);
}

// Checks whether the specified quaternion is normalized.
inline bool is_normalized(const quat& q) noexcept
{
	return approx_equal(len_squared(q), 1.0f);
}

// Computes the inverse(reciprocal) of the given quaternion. q* / (|q|^2)
inline quat inverse(const quat& q) noexcept
{
	float l2 = len_squared(q);
	assert(!approx_equal(l2, 0.f)); // A quaternion with len = 0 isn't invertible.

	float scalar = 1.f / l2;
	return conjugate(q) * scalar;
}

// Linearly interpolates between two values.
// Params:
// -	lhs:	The start of the range in which to interpolate.
// -	rhs:	The end of the range in which to interpolate.
// -	factor:	The value to use to interpolate between lhs & rhs.
//				Factor has to lie within the range [0 .. 1].
inline float2 lerp(const float2& lhs, const float2& rhs, float factor) noexcept
{
	assert(0.f <= factor && factor <= 1.f);
	return lhs + factor * (rhs - lhs);
}

// Linearly interpolates between two values.
// Params:
// -	lhs:	The start of the range in which to interpolate.
// -	rhs:	The end of the range in which to interpolate.
// -	factor:	The value to use to interpolate between lhs & rhs.
//				Factor has to lie within the range [0 .. 1].
inline float3 lerp(const float3& lhs, const float3& rhs, float factor) noexcept
{
	assert(0.f <= factor && factor <= 1.f);
	return lhs + factor * (rhs - lhs);
}

// Linearly interpolates between two values.
// Params:
// -	lhs:	The start of the range in which to interpolate.
// -	rhs:	The end of the range in which to interpolate.
// -	factor:	The value to use to interpolate between lhs & rhs.
//				Factor has to lie within the range [0 .. 1].
inline float4 lerp(const float4& lhs, const float4& rhs, float factor) noexcept
{
	assert(0.f <= factor && factor <= 1.f);
	return lhs + factor * (rhs - lhs);
}

// Returns new vector which is normalized(unit length) copy of the given one.
inline float2 normalize(const float2& v) noexcept
{
	float l2 = len_squared(v);
	if (approx_equal(l2, 0.0f) || approx_equal(l2, 1.0f)) return v;

	float factor = 1.0f / sqrt(l2);
	return v * factor;
}

// Returns new vector which is normalized(unit length) copy of the given one.
inline float3 normalize(const float3& v) noexcept
{
	float l2 = len_squared(v);
	if (approx_equal(l2, 0.0f) || approx_equal(l2, 1.0f)) return v;

	float factor = 1.0f / sqrt(l2);
	return v * factor;
}

// Returns new vector which is normalized(unit length) copy of the given one.
inline float4 normalize(const float4& v) noexcept
{
	float l2 = len_squared(v);
	if (approx_equal(l2, 0.0f) || approx_equal(l2, 1.0f)) return v;

	float factor = 1.0f / sqrt(l2);
	return v * factor;
}

// Returns a new quaternion which is normalized(unit length) copy of the given quaternion.
inline quat normalize(const quat& q) noexcept
{
	float l2 = len_squared(q);

	if (approx_equal(l2, 0.f) || approx_equal(l2, 1.f)) return q;

	float factor = 1.f / sqrt(l2);
	return q * factor;
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

// Returns rgb color volor
// red: (32..24) bytes, 
// green : (23..16) bytes,
// blue : (15..8) bytes,
// alpha : (7..0) bytes
inline float4 rgba(uint32_t val) noexcept
{
	return float4(
		((val >> 24) & 0xFF) / 255.0f,
		((val >> 16) & 0xFF) / 255.0f,
		((val >> 8) & 0xFF) / 255.0f,
		(val & 0xFF) / 255.0f
	);
}

// Performs spherical-interpolation between unit quaternions (geometrical slerp).
quat slerp(const quat& q, const quat& r, float factor);

} // namespace

#endif // CG_MATH_VECTOR_H_ 