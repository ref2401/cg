#ifndef CG_MATH_MAT3_H_
#define CG_MATH_MAT3_H_

#include <cassert>
#include <cmath>
#include <ostream>
#include "cg/math/float2.h"
#include "cg/math/float3.h"
#include "cg/math/utility.h"


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


	// Adds the matrix m to this matrix.
	mat3& operator+=(const mat3& m) noexcept
	{
		m00 += m.m00; m01 += m.m01; m02 += m.m02;
		m10 += m.m10; m11 += m.m11; m12 += m.m12;
		m20 += m.m20; m21 += m.m21; m22 += m.m22;
		return *this;
	}

	// Subtracts the matrix m from this matrix.
	mat3& operator-=(const mat3& m) noexcept
	{
		m00 -= m.m00; m01 -= m.m01; m02 -= m.m02;
		m10 -= m.m10; m11 -= m.m11; m12 -= m.m12;
		m20 -= m.m20; m21 -= m.m21; m22 -= m.m22;
		return *this;
	}

	// Multiplies this matrix by val.
	mat3& operator*=(float val) noexcept
	{
		m00 *= val; m01 *= val; m02 *= val;
		m10 *= val; m11 *= val; m12 *= val;
		m20 *= val; m21 *= val; m22 *= val;
		return *this;
	}

	// Post-multiplies this matrix with the specified matrix.
	mat3& operator*=(const mat3& m) noexcept;

	// Devies this matrix by val;
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

// Checks whether matrices lhs and rhs are equal.
bool operator==(const mat3& lhs, const mat3& rhs) noexcept;

// Checks whether matrices lhs and not rhs are equal.
inline bool operator!=(const mat3& lhs, const mat3& rhs) noexcept
{
	return !(lhs == rhs);
}

// Returns a matrix which is the result of adding the corresponding entries of lhs & rhs.  
inline mat3 operator+(const mat3& lhs, const mat3 rhs) noexcept
{
	return mat3(
		lhs.m00 + rhs.m00, lhs.m01 + rhs.m01, lhs.m02 + rhs.m02,
		lhs.m10 + rhs.m10, lhs.m11 + rhs.m11, lhs.m12 + rhs.m12,
		lhs.m20 + rhs.m20, lhs.m21 + rhs.m21, lhs.m22 + rhs.m22
	);
}

// Returns a matrix which is the result of subtracting the corresponding entries of lhs & rhs. 
inline mat3 operator-(const mat3& lhs, const mat3 rhs)
{
	return mat3(
		lhs.m00 - rhs.m00, lhs.m01 - rhs.m01, lhs.m02 - rhs.m02,
		lhs.m10 - rhs.m10, lhs.m11 - rhs.m11, lhs.m12 - rhs.m12,
		lhs.m20 - rhs.m20, lhs.m21 - rhs.m21, lhs.m22 - rhs.m22
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

// Devides each entry of m by val.
inline mat3 operator/(const mat3& m, float val) noexcept
{
	assert(!approx_equal(val, 0.f));

	return mat3(
		m.m00 / val, m.m01 / val, m.m02 / val,
		m.m10 / val, m.m11 / val, m.m12 / val,
		m.m20 / val, m.m21 / val, m.m22 / val
	);
}

std::ostream& operator<<(std::ostream& out, const mat3& m);

std::wostream& operator<<(std::wostream& out, const mat3& m);

//  Calculates the determinant of the matrix m.
inline float det(const mat3& m) noexcept
{
	return (m.m00 * m.m11 * m.m22) + (m.m01 * m.m12 * m.m20) + (m.m02 * m.m10 * m.m21)
		- (m.m02 * m.m11 * m.m20) - (m.m01 * m.m10 * m.m22) - (m.m00 * m.m12 * m.m21);
}

// Computes the inverse of the matrix.
mat3 inverse(const mat3& m);

// Determines whether the specified matrix is orthogonal.
inline bool is_orthogonal(const mat3& m) noexcept
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

// Puts the matrix m into a float array in a column major order.
inline float* put_in_column_major_order(const mat3& m, float* arr) noexcept
{
	arr[0] = m.m00; arr[1] = m.m10; arr[2] = m.m20;
	arr[3] = m.m01; arr[4] = m.m11; arr[5] = m.m21;
	arr[6] = m.m02; arr[7] = m.m12; arr[8] = m.m22;
	return arr;
}

// Puts the matrix m into a float array in a row major order.
inline float* put_in_row_major_order(const mat3& m, float* arr) noexcept
{
	arr[0] = m.m00; arr[1] = m.m01; arr[2] = m.m02;
	arr[3] = m.m10; arr[4] = m.m11; arr[5] = m.m12;
	arr[6] = m.m20; arr[7] = m.m21; arr[8] = m.m22;

	return arr;
}

// Calculates the sum of the elements on the main diagonal. tr(M).
inline float trace(const mat3& m) noexcept
{
	return m.m00 + m.m11 + m.m22;
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

} // namespace cg

#endif // CG_MATH_MAT3_H_
