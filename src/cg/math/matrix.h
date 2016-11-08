#ifndef CG_MATH_MATRIX_H_
#define CG_MATH_MATRIX_H_

#include <cassert>
#include <cmath>
#include <ostream>
#include "cg/math/vector.h"


namespace cg {

// mat3 is described by 3x3 scalars orderer in a rectangular fashion.
// Each scalar has two indices: row index, column index.
struct mat3 {
	static const mat3 identity;
	static const mat3 zero;

	mat3() noexcept :
	m00(0), m01(0), m02(0),
		m10(0), m11(0), m12(0),
		m20(0), m21(0), m22(0)
	{}

	mat3(float m00, float m01, float m02,
		float m10, float m11, float m12,
		float m20, float m21, float m22) noexcept :
	m00(m00), m01(m01), m02(m02),
		m10(m10), m11(m11), m12(m12),
		m20(m20), m21(m21), m22(m22)
	{}


	mat3& operator+=(const mat3& m) noexcept
	{
		m00 += m.m00; m01 += m.m01; m02 += m.m02;
		m10 += m.m10; m11 += m.m11; m12 += m.m12;
		m20 += m.m20; m21 += m.m21; m22 += m.m22;
		return *this;
	}

	mat3& operator-=(const mat3& m) noexcept
	{
		m00 -= m.m00; m01 -= m.m01; m02 -= m.m02;
		m10 -= m.m10; m11 -= m.m11; m12 -= m.m12;
		m20 -= m.m20; m21 -= m.m21; m22 -= m.m22;
		return *this;
	}

	mat3& operator*=(float val) noexcept
	{
		m00 *= val; m01 *= val; m02 *= val;
		m10 *= val; m11 *= val; m12 *= val;
		m20 *= val; m21 *= val; m22 *= val;
		return *this;
	}

	// Post-multiplies this matrix with the specified matrix.
	mat3& operator*=(const mat3& m) noexcept;

	mat3& operator/=(float val) noexcept
	{
		assert(!approx_equal(val, 0.f));

		m00 /= val; m01 /= val; m02 /= val;
		m10 /= val; m11 /= val; m12 /= val;
		m20 /= val; m21 /= val; m22 /= val;
		return *this;
	}


	// Returns ox vectoc of a 3D space basis.
	float3 ox() const noexcept
	{
		return float3(m00, m10, m20);
	}

	// Sets the ox vector of a 3D space basis.
	void set_ox(const float3& v) noexcept
	{
		m00 = v.x;
		m10 = v.y;
		m20 = v.z;
	}

	// Returns oy vectoc of a 3D space basis.
	float3 oy() const noexcept
	{
		return float3(m01, m11, m21);
	}

	// Sets the oy vector of a 3D space basis.
	void set_oy(const float3& v) noexcept
	{
		m01 = v.x;
		m11 = v.y;
		m21 = v.z;
	}

	// Returns oz vectoc of a 3D space basis.
	float3 oz() const noexcept
	{
		return float3(m02, m12, m22);
	}

	// Sets the oz vector of a 3D space basis.
	void set_oz(const float3& v) noexcept
	{
		m02 = v.x;
		m12 = v.y;
		m22 = v.z;
	}


	float m00, m01, m02;
	float m10, m11, m12;
	float m20, m21, m22;
};

// mat4 is described by 4x4 scalars orderer in a rectangular fashion.
// Each scalar has two indices: row index, column index.
struct mat4 {
	static const mat4 identity;
	static const mat4 zero;


	mat4() noexcept
		:	m00(0), m01(0), m02(0), m03(0),
		m10(0), m11(0), m12(0), m13(0),
		m20(0), m21(0), m22(0), m23(0),
		m30(0), m31(0), m32(0), m33(0)
	{}

	mat4(float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33) noexcept
		: 	m00(m00), m01(m01), m02(m02), m03(m03),
		m10(m10), m11(m11), m12(m12), m13(m13),
		m20(m20), m21(m21), m22(m22), m23(m23),
		m30(m30), m31(m31), m32(m32), m33(m33)
	{}


	mat4& operator+=(const mat4& m) noexcept
	{
		m00 += m.m00; m01 += m.m01; m02 += m.m02; m03 += m.m03;
		m10 += m.m10; m11 += m.m11; m12 += m.m12; m13 += m.m13;
		m20 += m.m20; m21 += m.m21; m22 += m.m22; m23 += m.m23;
		m30 += m.m30; m31 += m.m31; m32 += m.m32; m33 += m.m33;
		return *this;
	}

	mat4& operator-=(const mat4& m) noexcept
	{
		m00 -= m.m00; m01 -= m.m01; m02 -= m.m02; m03 -= m.m03;
		m10 -= m.m10; m11 -= m.m11; m12 -= m.m12; m13 -= m.m13;
		m20 -= m.m20; m21 -= m.m21; m22 -= m.m22; m23 -= m.m23;
		m30 -= m.m30; m31 -= m.m31; m32 -= m.m32; m33 -= m.m33;
		return *this;
	}

	mat4& operator*=(float val) noexcept
	{
		m00 *= val; m01 *= val; m02 *= val; m03 *= val;
		m10 *= val; m11 *= val; m12 *= val; m13 *= val;
		m20 *= val; m21 *= val; m22 *= val; m23 *= val;
		m30 *= val; m31 *= val; m32 *= val; m33 *= val;
		return *this;
	}

	// Post-multiplies this matrix with the specified matrix.
	mat4& operator*=(const mat4& m) noexcept;

	mat4& operator/=(float val) noexcept
	{
		assert(!approx_equal(val, 0.f));

		m00 /= val; m01 /= val; m02 /= val; m03 /= val;
		m10 /= val; m11 /= val; m12 /= val; m13 /= val;
		m20 /= val; m21 /= val; m22 /= val; m23 /= val;
		m30 /= val; m31 /= val; m32 /= val; m33 /= val;
		return *this;
	}

	explicit operator mat3() const noexcept
	{
		return mat3(m00, m01, m02, m10, m11, m12, m20, m21, m22);
	}


	// Returns ox vectoc of a 3D space basis.
	float3 ox() const noexcept
	{
		return float3(m00, m10, m20);
	}

	// Sets the ox vector of a 3D space basis.
	void set_ox(const float3& v) noexcept
	{
		m00 = v.x;
		m10 = v.y;
		m20 = v.z;
	}

	// Returns oy vectoc of a 3D space basis.
	float3 oy() const noexcept
	{
		return float3(m01, m11, m21);
	}

	// Sets the oy vector of a 3D space basis.
	void set_oy(const float3& v) noexcept
	{
		m01 = v.x;
		m11 = v.y;
		m21 = v.z;
	}

	// Returns oz vectoc of a 3D space basis.
	float3 oz() const noexcept
	{
		return float3(m02, m12, m22);
	}

	// Sets the oz vector of a 3D space basis.
	void set_oz(const float3& v) noexcept
	{
		m02 = v.x;
		m12 = v.y;
		m22 = v.z;
	}


	float m00, m01, m02, m03;
	float m10, m11, m12, m13;
	float m20, m21, m22, m23;
	float m30, m31, m32, m33;
};


bool operator==(const mat3& lhs, const mat3& rhs) noexcept;

inline bool operator!=(const mat3& lhs, const mat3& rhs) noexcept
{
	return !(lhs == rhs);
}

bool operator==(const mat4& lhs, const mat4& rhs) noexcept;

inline bool operator!=(const mat4& lhs, const mat4& rhs) noexcept
{
	return !(lhs == rhs);
}

inline mat3 operator+(const mat3& lhs, const mat3 rhs) noexcept
{
	return mat3(
		lhs.m00 + rhs.m00, lhs.m01 + rhs.m01, lhs.m02 + rhs.m02,
		lhs.m10 + rhs.m10, lhs.m11 + rhs.m11, lhs.m12 + rhs.m12,
		lhs.m20 + rhs.m20, lhs.m21 + rhs.m21, lhs.m22 + rhs.m22
	);
}

inline mat4 operator+(const mat4& lhs, const mat4 rhs) noexcept
{
	return mat4(
		lhs.m00 + rhs.m00, lhs.m01 + rhs.m01, lhs.m02 + rhs.m02, lhs.m03 + rhs.m03,
		lhs.m10 + rhs.m10, lhs.m11 + rhs.m11, lhs.m12 + rhs.m12, lhs.m13 + rhs.m13,
		lhs.m20 + rhs.m20, lhs.m21 + rhs.m21, lhs.m22 + rhs.m22, lhs.m23 + rhs.m23,
		lhs.m30 + rhs.m30, lhs.m31 + rhs.m31, lhs.m32 + rhs.m32, lhs.m33 + rhs.m33
	);
}

inline mat3 operator-(const mat3& lhs, const mat3 rhs)
{
	return mat3(
		lhs.m00 - rhs.m00, lhs.m01 - rhs.m01, lhs.m02 - rhs.m02,
		lhs.m10 - rhs.m10, lhs.m11 - rhs.m11, lhs.m12 - rhs.m12,
		lhs.m20 - rhs.m20, lhs.m21 - rhs.m21, lhs.m22 - rhs.m22
	);
}

inline mat4 operator-(const mat4& lhs, const mat4 rhs) noexcept
{
	return mat4(
		lhs.m00 - rhs.m00, lhs.m01 - rhs.m01, lhs.m02 - rhs.m02, lhs.m03 - rhs.m03,
		lhs.m10 - rhs.m10, lhs.m11 - rhs.m11, lhs.m12 - rhs.m12, lhs.m13 - rhs.m13,
		lhs.m20 - rhs.m20, lhs.m21 - rhs.m21, lhs.m22 - rhs.m22, lhs.m23 - rhs.m23,
		lhs.m30 - rhs.m30, lhs.m31 - rhs.m31, lhs.m32 - rhs.m32, lhs.m33 - rhs.m33
	);
}

// Multiplies each entry of m by val.
inline mat3 operator*(const mat3& m, float val) noexcept
{
	return mat3(
		m.m00 * val, m.m01 * val, m.m02 * val,
		m.m10 * val, m.m11 * val, m.m12 * val,
		m.m20 * val, m.m21 * val, m.m22 * val
	);
}

// Multiplies each entry of m by val.
inline mat3 operator*(float val, const mat3& m) noexcept
{
	return mat3(
		m.m00 * val, m.m01 * val, m.m02 * val,
		m.m10 * val, m.m11 * val, m.m12 * val,
		m.m20 * val, m.m21 * val, m.m22 * val
	);
}

// Post-multiplies lhs matrix with rhs.
mat3 operator*(const mat3& lhs, const mat3& rhs) noexcept;

// Multiplies each entry of m by val.
inline mat4 operator*(const mat4& m, float val) noexcept
{
	return mat4(
		m.m00 * val, m.m01 * val, m.m02 * val, m.m03 * val,
		m.m10 * val, m.m11 * val, m.m12 * val, m.m13 * val,
		m.m20 * val, m.m21 * val, m.m22 * val, m.m23 * val,
		m.m30 * val, m.m31 * val, m.m32 * val, m.m33 * val
	);
}

// Multiplies each entry of m by val.
inline mat4 operator*(float  val, const mat4& m) noexcept
{
	return mat4(
		m.m00 * val, m.m01 * val, m.m02 * val, m.m03 * val,
		m.m10 * val, m.m11 * val, m.m12 * val, m.m13 * val,
		m.m20 * val, m.m21 * val, m.m22 * val, m.m23 * val,
		m.m30 * val, m.m31 * val, m.m32 * val, m.m33 * val
	);
}

// Post-multiplies lhs matrix with rhs.
mat4 operator*(const mat4& lhs, const mat4& rhs) noexcept;

inline mat3 operator/(const mat3& m, float val) noexcept
{
	assert(!approx_equal(val, 0.f));

	return mat3(
		m.m00 / val, m.m01 / val, m.m02 / val,
		m.m10 / val, m.m11 / val, m.m12 / val,
		m.m20 / val, m.m21 / val, m.m22 / val
	);
}

inline mat4 operator/(const mat4& m, float val) noexcept
{
	assert(!approx_equal(val, 0.f));

	return mat4(
		m.m00 / val, m.m01 / val, m.m02 / val, m.m03 / val,
		m.m10 / val, m.m11 / val, m.m12 / val, m.m13 / val,
		m.m20 / val, m.m21 / val, m.m22 / val, m.m23 / val,
		m.m30 / val, m.m31 / val, m.m32 / val, m.m33 / val
	);
}

std::ostream& operator<<(std::ostream& out, const mat3& m);

std::wostream& operator<<(std::wostream& out, const mat3& m);

std::ostream& operator<<(std::ostream& out, const mat4& m);

std::wostream& operator<<(std::wostream& out, const mat4& m);

//  Calculates the determinant of the matrix m.
inline float det(const mat3& m) noexcept
{
	return (m.m00 * m.m11 * m.m22) + (m.m01 * m.m12 * m.m20) + (m.m02 * m.m10 * m.m21)
		- (m.m02 * m.m11 * m.m20) - (m.m01 * m.m10 * m.m22) - (m.m00 * m.m12 * m.m21);
}

//  Calculates the determinant of the matrix m.
float det(const mat4& m) noexcept;

// Computes the inverse of the matrix.
mat3 inverse(const mat3& m);

// Computes the inverse of the matrix.
mat4 inverse(const mat4& m) noexcept;

// Determines whether the specified matrix is orthogonal.
inline bool is_orthogonal(const mat3& m) noexcept
{
	float abs_d = std::abs(det(m));
	return approx_equal(abs_d, 1.f);
}

// Determines whether the specified matrix is orthogonal.
inline bool is_orthogonal(const mat4& m) noexcept
{
	float abs_d = std::abs(det(m));
	return approx_equal(abs_d, 1.f);
}

// Multiplies matrix by the column vector v. 
inline float3 mul(const mat3& m, const float3& v) noexcept
{
	return float3(
		(m.m00 * v.x) + (m.m01 * v.y) + (m.m02 * v.z),
		(m.m10 * v.x) + (m.m11 * v.y) + (m.m12 * v.z),
		(m.m20 * v.x) + (m.m21 * v.y) + (m.m22 * v.z)
	);
}

// Multiplies matrix by the column vector float3(v.x, v.y, z). 
inline float3 mul(const mat3& m, const float2& v, float z = 0.f) noexcept
{
	return mul(m, float3(v.x, v.y, z));
}

// Multiplies the given matrix by the column vector. 
inline float4 mul(const mat4& m, const float4& v) noexcept
{
	return float4(
		(m.m00 * v.x) + (m.m01 * v.y) + (m.m02 * v.z) + (m.m03 * v.w),
		(m.m10 * v.x) + (m.m11 * v.y) + (m.m12 * v.z) + (m.m13 * v.w),
		(m.m20 * v.x) + (m.m21 * v.y) + (m.m22 * v.z) + (m.m23 * v.w),
		(m.m30 * v.x) + (m.m31 * v.y) + (m.m32 * v.z) + (m.m33 * v.w)
	);
}

// Multiplies matrix by the column vector float4(v.x, v.y, z, w). 
inline float4 mul(const mat4& m, const float2& v, float z = 0.f, float w = 1.f) noexcept
{
	return mul(m, float4(v.x, v.y, z, w));
}

// Multiplies matrix by the column vector float4(v.x, v.y, v.z, w). 
inline float4 mul(const mat4& m, const float3& v, float w = 1.f) noexcept
{
	return mul(m, float4(v.x, v.y, v.z, w));
}

// Puts the matrix m into a float array in a column major order.
inline float* to_array_column_major_order(const mat3& m, float* arr) noexcept
{
	arr[0] = m.m00; arr[1] = m.m10; arr[2] = m.m20;
	arr[3] = m.m01; arr[4] = m.m11; arr[5] = m.m21;
	arr[6] = m.m02; arr[7] = m.m12; arr[8] = m.m22;
	return arr;
}

// Puts the matrix m into a float array in a column major order.
inline float* to_array_column_major_order(const mat4& m, float* arr) noexcept
{
	arr[0] = m.m00; arr[1] = m.m10; arr[2] = m.m20; arr[3] = m.m30;
	arr[4] = m.m01; arr[5] = m.m11; arr[6] = m.m21; arr[7] = m.m31;
	arr[8] = m.m02; arr[9] = m.m12; arr[10] = m.m22; arr[11] = m.m32;
	arr[12] = m.m03; arr[13] = m.m13; arr[14] = m.m23; arr[15] = m.m33;

	return arr;
}

// Puts the matrix m into a float array in a row major order.
inline float* to_array_row_major_order(const mat3& m, float* arr) noexcept
{
	arr[0] = m.m00; arr[1] = m.m01; arr[2] = m.m02;
	arr[3] = m.m10; arr[4] = m.m11; arr[5] = m.m12;
	arr[6] = m.m20; arr[7] = m.m21; arr[8] = m.m22;

	return arr;
}

// Puts the matrix m into a float array in a row major order.
inline float* to_array_row_major_order(const mat4& m, float* arr) noexcept
{
	arr[0] = m.m00; arr[1] = m.m01; arr[2] = m.m02; arr[3] = m.m03;
	arr[4] = m.m10; arr[5] = m.m11; arr[6] = m.m12; arr[7] = m.m13;
	arr[8] = m.m20; arr[9] = m.m21; arr[10] = m.m22; arr[11] = m.m23;
	arr[12] = m.m30; arr[13] = m.m31; arr[14] = m.m32; arr[15] = m.m33;

	return arr;
}

// Calculates the sum of the elements on the main diagonal. tr(M).
inline float trace(const mat3& m) noexcept
{
	return m.m00 + m.m11 + m.m22;
}

// Calculates the sum of the elements on the main diagonal. tr(M).
inline float trace(const mat4& m) noexcept
{
	return m.m00 + m.m11 + m.m22 + m.m33;
}

// Reflects the matrix over its main diagonal to obtain transposed matrix.
inline mat3 transpose(const mat3& m) noexcept
{
	return mat3(
		m.m00, m.m10, m.m20,
		m.m01, m.m11, m.m21,
		m.m02, m.m12, m.m22
	);
}

// Reflects the matrix over its main diagonal to obtain transposed matrix.
inline mat4 transpose(const mat4& m) noexcept
{
	return mat4(
		m.m00, m.m10, m.m20, m.m30,
		m.m01, m.m11, m.m21, m.m31,
		m.m02, m.m12, m.m22, m.m32,
		m.m03, m.m13, m.m23, m.m33
	);
}

} // namespace cg

#endif // CG_MATH_MATRIX_H_