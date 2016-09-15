#ifndef CG_MATH_MAT4_H_
#define CG_MATH_MAT4_H_

#include <cassert>
#include <cmath>
#include <ostream>
#include "cg/math/float2.h"
#include "cg/math/float3.h"
#include "cg/math/float4.h"
#include "cg/math/utils.h"


namespace cg {

// mat4 is described by 4x4 scalars orderer in a rectangular fashion.
// Each scalar has two indices: row index, column index.
struct mat4 {
	static const mat4 identity;
	static const mat4 zero;


	mat4() noexcept;

	mat4(float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33) noexcept;


	// Returns ox vectoc of a 3D space basis.
	float3 ox() const;

	// Sets the ox vector of a 3D space basis.
	void set_ox(const float3& v);

	// Returns oy vectoc of a 3D space basis.
	float3 oy() const;

	// Sets the oy vector of a 3D space basis.
	void set_oy(const float3& v);

	// Returns oz vectoc of a 3D space basis.
	float3 oz() const;

	// Sets the oz vector of a 3D space basis.
	void set_oz(const float3& v);


	// Adds the matrix m to this matrix.
	mat4& operator+=(const mat4& m);

	// Subtracts the matrix m from this matrix.
	mat4& operator-=(const mat4& m);

	// Multiplies this matrix by val.
	mat4& operator*=(float val);

	// Post-multiplies this matrix with the specified matrix.
	mat4& operator*=(const mat4& m);

	// Devies this matrix by val;
	mat4& operator/=(float val);


	float m00, m01, m02, m03;
	float m10, m11, m12, m13;
	float m20, m21, m22, m23;
	float m30, m31, m32, m33;
};

// Checks whether matrices lhs and rhs are equal.
bool operator==(const mat4& lhs, const mat4& rhs);

// Checks whether matrices lhs and not rhs are equal.
bool operator!=(const mat4& lhs, const mat4& rhs);

// Returns a matrix which is the result of adding the corresponding entries of lhs & rhs.  
mat4 operator+(const mat4& lhs, const mat4 rhs);

// Returns a matrix which is the result of subtracting the corresponding entries of lhs & rhs. 
mat4 operator-(const mat4& lhs, const mat4 rhs);

// Multiplies each entry of m by val.
mat4 operator*(const mat4& m, float val);

// Post-multiplies lhs matrix with rhs.
mat4 operator*(const mat4& lhs, const mat4& rhs);

// Devides each entry of m by val.
mat4 operator/(const mat4& m, float val);

std::ostream& operator<<(std::ostream& out, const mat4& m);

std::wostream& operator<<(std::wostream& out, const mat4& m);

//  Calculates the determinant of the matrix m.
float det(const mat4& m);

// Computes the inverse of the matrix.
mat4 inverse(const mat4& m);

// Determines whether the specified matrix is orthogonal.
bool is_orthogonal(const mat4& m);

// Multiplies matrix by the column vector float4(v.x, v.y, z, w). 
float4 mul(const mat4& m, const float2& v, float z = 0.f, float w = 1.f);

// Multiplies matrix by the column vector float4(v.x, v.y, v.z, w). 
float4 mul(const mat4& m, const float3& v, float w = 1.f);

// Multiplies the given matrix by the column vector. 
float4 mul(const mat4& m, const float4& v);

// Puts the matrix m into a float array in a column major order.
float* put_in_column_major_order(const mat4& m, float* arr);

// Puts the matrix m into a float array in a row major order.
float* put_in_row_major_order(const mat4& m, float* arr);

// Calculates the sum of the elements on the main diagonal. tr(M).
float trace(const mat4& m);

// Reflects the matrix over its main diagonal to obtain transposed matrix.
mat4 transpose(const mat4& m);



inline mat4::mat4() noexcept :
	m00(0), m01(0), m02(0), m03(0),
	m10(0), m11(0), m12(0), m13(0),
	m20(0), m21(0), m22(0), m23(0),
	m30(0), m31(0), m32(0), m33(0) 
{}

inline mat4::mat4(float m00, float m01, float m02, float m03,
	float m10, float m11, float m12, float m13,
	float m20, float m21, float m22, float m23,
	float m30, float m31, float m32, float m33) noexcept :
	m00(m00), m01(m01), m02(m02), m03(m03),
	m10(m10), m11(m11), m12(m12), m13(m13),
	m20(m20), m21(m21), m22(m22), m23(m23),
	m30(m30), m31(m31), m32(m32), m33(m33)
{}


inline float3 mat4::ox() const
{
	return float3(m00, m10, m20);
}

inline void mat4::set_ox(const float3& v)
{
	m00 = v.x;
	m10 = v.y;
	m20 = v.z;
}

inline float3 mat4::oy() const
{
	return float3(m01, m11, m21);
}

inline void mat4::set_oy(const float3& v)
{
	m01 = v.x;
	m11 = v.y;
	m21 = v.z;
}

inline float3 mat4::oz() const
{
	return float3(m02, m12, m22);
}

inline void mat4::set_oz(const float3& v)
{
	m02 = v.x;
	m12 = v.y;
	m22 = v.z;
}


inline mat4& mat4::operator+=(const mat4& m)
{
	m00 += m.m00; m01 += m.m01; m02 += m.m02; m03 += m.m03;
	m10 += m.m10; m11 += m.m11; m12 += m.m12; m13 += m.m13;
	m20 += m.m20; m21 += m.m21; m22 += m.m22; m23 += m.m23;
	m30 += m.m30; m31 += m.m31; m32 += m.m32; m33 += m.m33;
	return *this;
}

inline mat4& mat4::operator-=(const mat4& m)
{
	m00 -= m.m00; m01 -= m.m01; m02 -= m.m02; m03 -= m.m03;
	m10 -= m.m10; m11 -= m.m11; m12 -= m.m12; m13 -= m.m13;
	m20 -= m.m20; m21 -= m.m21; m22 -= m.m22; m23 -= m.m23;
	m30 -= m.m30; m31 -= m.m31; m32 -= m.m32; m33 -= m.m33;
	return *this;
}

inline mat4& mat4::operator*=(float val)
{
	m00 *= val; m01 *= val; m02 *= val; m03 *= val;
	m10 *= val; m11 *= val; m12 *= val; m13 *= val;
	m20 *= val; m21 *= val; m22 *= val; m23 *= val;
	m30 *= val; m31 *= val; m32 *= val; m33 *= val;
	return *this;
}

inline mat4& mat4::operator*=(const mat4& m)
{
	float mp00, mp01, mp02, mp03;
	float mp10, mp11, mp12, mp13;
	float mp20, mp21, mp22, mp23;
	float mp30, mp31, mp32, mp33;

	mp00 = m00 * m.m00 + m01 * m.m10 + m02 * m.m20 + m03 * m.m30;
	mp01 = m00 * m.m01 + m01 * m.m11 + m02 * m.m21 + m03 * m.m31;
	mp02 = m00 * m.m02 + m01 * m.m12 + m02 * m.m22 + m03 * m.m32;
	mp03 = m00 * m.m03 + m01 * m.m13 + m02 * m.m23 + m03 * m.m33;
	
	mp10 = m10 * m.m00 + m11 * m.m10 + m12 * m.m20 + m13 * m.m30;
	mp11 = m10 * m.m01 + m11 * m.m11 + m12 * m.m21 + m13 * m.m31;
	mp12 = m10 * m.m02 + m11 * m.m12 + m12 * m.m22 + m13 * m.m32;
	mp13 = m10 * m.m03 + m11 * m.m13 + m12 * m.m23 + m13 * m.m33;
	
	mp20 = m20 * m.m00 + m21 * m.m10 + m22 * m.m20 + m23 * m.m30;
	mp21 = m20 * m.m01 + m21 * m.m11 + m22 * m.m21 + m23 * m.m31;
	mp22 = m20 * m.m02 + m21 * m.m12 + m22 * m.m22 + m23 * m.m32;
	mp23 = m20 * m.m03 + m21 * m.m13 + m22 * m.m23 + m23 * m.m33;
	
	mp30 = m30 * m.m00 + m31 * m.m10 + m32 * m.m20 + m33 * m.m30;
	mp31 = m30 * m.m01 + m31 * m.m11 + m32 * m.m21 + m33 * m.m31;
	mp32 = m30 * m.m02 + m31 * m.m12 + m32 * m.m22 + m33 * m.m32;
	mp33 = m30 * m.m03 + m31 * m.m13 + m32 * m.m23 + m33 * m.m33;

	m00 = mp00; m01 = mp01; m02 = mp02; m03 = mp03;
	m10 = mp10; m11 = mp11; m12 = mp12; m13 = mp13;
	m20 = mp20; m21 = mp21; m22 = mp22; m23 = mp23;
	m30 = mp30; m31 = mp31; m32 = mp32; m33 = mp33;
	return *this;
}

inline mat4& mat4::operator/=(float val)
{
	assert(!approx_equal(val, 0.f));

	m00 /= val; m01 /= val; m02 /= val; m03 /= val;
	m10 /= val; m11 /= val; m12 /= val; m13 /= val;
	m20 /= val; m21 /= val; m22 /= val; m23 /= val;
	m30 /= val; m31 /= val; m32 /= val; m33 /= val;
	return *this;
}


inline bool operator==(const mat4& lhs, const mat4& rhs)
{
	return approx_equal(lhs.m00, rhs.m00)
		&& approx_equal(lhs.m10, rhs.m10)
		&& approx_equal(lhs.m20, rhs.m20)
		&& approx_equal(lhs.m30, rhs.m30)

		&& approx_equal(lhs.m01, rhs.m01)
		&& approx_equal(lhs.m11, rhs.m11)
		&& approx_equal(lhs.m21, rhs.m21)
		&& approx_equal(lhs.m31, rhs.m31)

		&& approx_equal(lhs.m02, rhs.m02)
		&& approx_equal(lhs.m12, rhs.m12)
		&& approx_equal(lhs.m22, rhs.m22)
		&& approx_equal(lhs.m32, rhs.m32)

		&& approx_equal(lhs.m03, rhs.m03)
		&& approx_equal(lhs.m13, rhs.m13)
		&& approx_equal(lhs.m23, rhs.m23)
		&& approx_equal(lhs.m33, rhs.m33);
}

inline bool operator!=(const mat4& lhs, const mat4& rhs)
{
	return !(lhs == rhs);
}

inline mat4 operator+(const mat4& lhs, const mat4 rhs)
{
	return mat4(
		lhs.m00 + rhs.m00, lhs.m01 + rhs.m01, lhs.m02 + rhs.m02, lhs.m03 + rhs.m03,
		lhs.m10 + rhs.m10, lhs.m11 + rhs.m11, lhs.m12 + rhs.m12, lhs.m13 + rhs.m13,
		lhs.m20 + rhs.m20, lhs.m21 + rhs.m21, lhs.m22 + rhs.m22, lhs.m23 + rhs.m23,
		lhs.m30 + rhs.m30, lhs.m31 + rhs.m31, lhs.m32 + rhs.m32, lhs.m33 + rhs.m33
	);
}

inline mat4 operator-(const mat4& lhs, const mat4 rhs)
{
	return mat4(
		lhs.m00 - rhs.m00, lhs.m01 - rhs.m01, lhs.m02 - rhs.m02, lhs.m03 - rhs.m03,
		lhs.m10 - rhs.m10, lhs.m11 - rhs.m11, lhs.m12 - rhs.m12, lhs.m13 - rhs.m13,
		lhs.m20 - rhs.m20, lhs.m21 - rhs.m21, lhs.m22 - rhs.m22, lhs.m23 - rhs.m23,
		lhs.m30 - rhs.m30, lhs.m31 - rhs.m31, lhs.m32 - rhs.m32, lhs.m33 - rhs.m33
	);
}

inline mat4 operator*(const mat4& m, float val)
{
	return mat4(
		m.m00 * val, m.m01 * val, m.m02 * val, m.m03 * val,
		m.m10 * val, m.m11 * val, m.m12 * val, m.m13 * val,
		m.m20 * val, m.m21 * val, m.m22 * val, m.m23 * val,
		m.m30 * val, m.m31 * val, m.m32 * val, m.m33 * val
	);
}

inline mat4 operator*(float  val, const mat4& m)
{
	return mat4(
		m.m00 * val, m.m01 * val, m.m02 * val, m.m03 * val,
		m.m10 * val, m.m11 * val, m.m12 * val, m.m13 * val,
		m.m20 * val, m.m21 * val, m.m22 * val, m.m23 * val,
		m.m30 * val, m.m31 * val, m.m32 * val, m.m33 * val
	);
}

inline mat4 operator*(const mat4& lhs, const mat4& rhs)
{
	mat4 product = mat4::zero;

	product.m00 = lhs.m00 * rhs.m00 + lhs.m01 * rhs.m10 + lhs.m02 * rhs.m20 + lhs.m03 * rhs.m30;
	product.m01 = lhs.m00 * rhs.m01 + lhs.m01 * rhs.m11 + lhs.m02 * rhs.m21 + lhs.m03 * rhs.m31;
	product.m02 = lhs.m00 * rhs.m02 + lhs.m01 * rhs.m12 + lhs.m02 * rhs.m22 + lhs.m03 * rhs.m32;
	product.m03 = lhs.m00 * rhs.m03 + lhs.m01 * rhs.m13 + lhs.m02 * rhs.m23 + lhs.m03 * rhs.m33;
	product.m10 = lhs.m10 * rhs.m00 + lhs.m11 * rhs.m10 + lhs.m12 * rhs.m20 + lhs.m13 * rhs.m30;
	product.m11 = lhs.m10 * rhs.m01 + lhs.m11 * rhs.m11 + lhs.m12 * rhs.m21 + lhs.m13 * rhs.m31;
	product.m12 = lhs.m10 * rhs.m02 + lhs.m11 * rhs.m12 + lhs.m12 * rhs.m22 + lhs.m13 * rhs.m32;
	product.m13 = lhs.m10 * rhs.m03 + lhs.m11 * rhs.m13 + lhs.m12 * rhs.m23 + lhs.m13 * rhs.m33;
	product.m20 = lhs.m20 * rhs.m00 + lhs.m21 * rhs.m10 + lhs.m22 * rhs.m20 + lhs.m23 * rhs.m30;
	product.m21 = lhs.m20 * rhs.m01 + lhs.m21 * rhs.m11 + lhs.m22 * rhs.m21 + lhs.m23 * rhs.m31;
	product.m22 = lhs.m20 * rhs.m02 + lhs.m21 * rhs.m12 + lhs.m22 * rhs.m22 + lhs.m23 * rhs.m32;
	product.m23 = lhs.m20 * rhs.m03 + lhs.m21 * rhs.m13 + lhs.m22 * rhs.m23 + lhs.m23 * rhs.m33;
	product.m30 = lhs.m30 * rhs.m00 + lhs.m31 * rhs.m10 + lhs.m32 * rhs.m20 + lhs.m33 * rhs.m30;
	product.m31 = lhs.m30 * rhs.m01 + lhs.m31 * rhs.m11 + lhs.m32 * rhs.m21 + lhs.m33 * rhs.m31;
	product.m32 = lhs.m30 * rhs.m02 + lhs.m31 * rhs.m12 + lhs.m32 * rhs.m22 + lhs.m33 * rhs.m32;
	product.m33 = lhs.m30 * rhs.m03 + lhs.m31 * rhs.m13 + lhs.m32 * rhs.m23 + lhs.m33 * rhs.m33;

	return product;
}

inline mat4 operator/(const mat4& m, float  val)
{
	assert(!approx_equal(val, 0.f));

	return mat4(
		m.m00 / val, m.m01 / val, m.m02 / val, m.m03 / val,
		m.m10 / val, m.m11 / val, m.m12 / val, m.m13 / val,
		m.m20 / val, m.m21 / val, m.m22 / val, m.m23 / val,
		m.m30 / val, m.m31 / val, m.m32 / val, m.m33 / val
	);
}

inline std::ostream& operator<<(std::ostream& out, const mat4& m)
{
	out << "mat4("
		<< m.m00 << ", " << m.m01 << ", " << m.m02 << ", " << m.m03 << ",  "
		<< m.m10 << ", " << m.m11 << ", " << m.m12 << ", " << m.m13 << ",  "
		<< m.m20 << ", " << m.m21 << ", " << m.m22 << ", " << m.m23 << ",  "
		<< m.m30 << ", " << m.m31 << ", " << m.m32 << ", " << m.m33 << ")";

	return out;
}

inline std::wostream& operator<<(std::wostream& out, const mat4& m)
{
	out << "mat4("
		<< m.m00 << ", " << m.m01 << ", " << m.m02 << ", " << m.m03 << ",  "
		<< m.m10 << ", " << m.m11 << ", " << m.m12 << ", " << m.m13 << ",  "
		<< m.m20 << ", " << m.m21 << ", " << m.m22 << ", " << m.m23 << ",  "
		<< m.m30 << ", " << m.m31 << ", " << m.m32 << ", " << m.m33 << ")";

	return out;
}

inline float det(const mat4& m)
{
	// find all the required first minors of m.
	float minor00 = m.m11*m.m22*m.m33 + m.m12*m.m23*m.m31 + m.m13*m.m21*m.m32 
		- m.m13*m.m22*m.m31 - m.m12*m.m21*m.m33 - m.m11*m.m23*m.m32;
	float minor01 = m.m10*m.m22*m.m33 + m.m12*m.m23*m.m30 + m.m13*m.m20*m.m32
		- m.m13*m.m22*m.m30 - m.m12*m.m20*m.m33 - m.m10*m.m23*m.m32;
	float minor02 = m.m10*m.m21*m.m33 + m.m11*m.m23*m.m30 + m.m13*m.m20*m.m31
		- m.m13*m.m21*m.m30 - m.m11*m.m20*m.m33 - m.m10*m.m23*m.m31;
	float minor03 = m.m10*m.m21*m.m32 + m.m11*m.m22*m.m30 + m.m12*m.m20*m.m31
		- m.m12*m.m21*m.m30 - m.m11*m.m20*m.m32 - m.m10*m.m22*m.m31;

	return m.m00 * minor00 - m.m01 * minor01 + m.m02 * minor02 - m.m03 * minor03;
}

inline bool is_orthogonal(const mat4& m)
{
	float abs_d = std::abs(det(m));
	return approx_equal(abs_d, 1.f);
}

inline float4 mul(const mat4& m, const float2& v, float z, float w)
{
	return mul(m, float4(v.x, v.y, z, w));
}

inline float4 mul(const mat4& m, const float3& v, float w)
{
	return mul(m, float4(v.x, v.y, v.z, w));
}

inline float4 mul(const mat4& m, const float4& v)
{
	return float4(
		(m.m00 * v.x) + (m.m01 * v.y) + (m.m02 * v.z) + (m.m03 * v.w),
		(m.m10 * v.x) + (m.m11 * v.y) + (m.m12 * v.z) + (m.m13 * v.w),
		(m.m20 * v.x) + (m.m21 * v.y) + (m.m22 * v.z) + (m.m23 * v.w),
		(m.m30 * v.x) + (m.m31 * v.y) + (m.m32 * v.z) + (m.m33 * v.w)
	);
}

inline float* put_in_column_major_order(const mat4& m, float* arr)
{
	arr[0] = m.m00; arr[1] = m.m10; arr[2] = m.m20; arr[3] = m.m30;
	arr[4] = m.m01; arr[5] = m.m11; arr[6] = m.m21; arr[7] = m.m31;
	arr[8] = m.m02; arr[9] = m.m12; arr[10] = m.m22; arr[11] = m.m32;
	arr[12] = m.m03; arr[13] = m.m13; arr[14] = m.m23; arr[15] = m.m33;

	return arr;
}

inline float* put_in_row_major_order(const mat4& m, float* arr)
{
	arr[0] = m.m00; arr[1] = m.m01; arr[2] = m.m02; arr[3] = m.m03;
	arr[4] = m.m10; arr[5] = m.m11; arr[6] = m.m12; arr[7] = m.m13;
	arr[8] = m.m20; arr[9] = m.m21; arr[10] = m.m22; arr[11] = m.m23;
	arr[12] = m.m30; arr[13] = m.m31; arr[14] = m.m32; arr[15] = m.m33;

	return arr;
}

inline float trace(const mat4& m)
{
	return m.m00 + m.m11 + m.m22 + m.m33;
}

inline mat4 transpose(const mat4& m)
{
	return mat4(
		m.m00, m.m10, m.m20, m.m30,
		m.m01, m.m11, m.m21, m.m31,
		m.m02, m.m12, m.m22, m.m32,
		m.m03, m.m13, m.m23, m.m33
	);
}

} // namespace cg

#endif // CG_MATH_MAT4_H_
