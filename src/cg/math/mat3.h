#ifndef CG_MATH_MAT3_H_
#define CG_MATH_MAT3_H_

#include <cassert>
#include <ostream>
#include "cg/math/float2.h"
#include "cg/math/float3.h"
#include "cg/math/utils.h"

namespace cg {

// mat3 is described by 3x3 scalars orderer in a rectangular fashion.
// Each scalar has two indices: row index, column index.
struct mat3 {
	static const mat3 identity;
	static const mat3 zero;

	mat3();

	mat3(float m00, float m01, float m02,
		float m10, float m11, float m12,
		float m20, float m21, float m22);


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
	mat3& operator+=(const mat3& m);

	// Subtracts the matrix m from this matrix.
	mat3& operator-=(const mat3& m);

	// Multiplies this matrix by val.
	mat3& operator*=(float val);

	// Post-multiplies this matrix with the specified matrix.
	mat3& operator*=(const mat3& m);

	// Devies this matrix by val;
	mat3& operator/=(float val);

	float m00, m01, m02;
	float m10, m11, m12;
	float m20, m21, m22;
};

// Checks whether matrices lhs and rhs are equal.
bool operator==(const mat3& lhs, const mat3& rhs);

// Checks whether matrices lhs and not rhs are equal.
bool operator!=(const mat3& lhs, const mat3& rhs);

// Returns a matrix which is the result of adding the corresponding entries of lhs & rhs.  
mat3 operator+(const mat3& lhs, const mat3 rhs);

// Returns a matrix which is the result of subtracting the corresponding entries of lhs & rhs. 
mat3 operator-(const mat3& lhs, const mat3 rhs);

// Multiplies each entry of m by val.
mat3 operator*(const mat3& m, float val);

// Post-multiplies lhs matrix with rhs.
mat3 operator*(const mat3& lhs, const mat3& rhs);

// Devides each entry of m by val.
mat3 operator/(const mat3& m, float val);

std::ostream& operator<<(std::ostream& out, const mat3& m);

std::wostream& operator<<(std::wostream& out, const mat3& m);

//  Calculates the determinant of the matrix m.
float det(const mat3& m);

// Computes the inverse of the matrix.
mat3 inverse(const mat3& m);

// Multiplies matrix by the column vector float3(v.x, v.y, z). 
float3 mul(const mat3& m, const float2& v, float z = 0.f);

// Multiplies matrix by the column vector v. 
float3 mul(const mat3& m, const float3& v);

// Puts the matrix m into a float array in a column major order.
float* put_in_column_major_order(const mat3& m, float* arr);

// Puts the matrix m into a float array in a row major order.
float* put_in_row_major_order(const mat3& m, float* arr);

// Calculates the sum of the elements on the main diagonal. tr(M).
float trace(const mat3& m);

// Reflects the matrix over its main diagonal to obtain transposed matrix.
mat3 transpose(const mat3& m);



inline mat3::mat3() :
	m00(0), m01(0), m02(0), 
	m10(0), m11(0), m12(0), 
	m20(0), m21(0), m22(0)
{}

inline mat3::mat3(float m00, float m01, float m02,
	float m10, float m11, float m12,
	float m20, float m21, float m22) :
	m00(m00), m01(m01), m02(m02), 
	m10(m10), m11(m11), m12(m12), 
	m20(m20), m21(m21), m22(m22)
{}


inline float3 mat3::ox() const
{
	return float3(m00, m10, m20);
}

inline void mat3::set_ox(const float3& v)
{
	m00 = v.x;
	m10 = v.y;
	m20 = v.z;
}

inline float3 mat3::oy() const
{
	return float3(m01, m11, m21);
}

inline void mat3::set_oy(const float3& v)
{
	m01 = v.x;
	m11 = v.y;
	m21 = v.z;
}

inline float3 mat3::oz() const
{
	return float3(m02, m12, m22);
}

inline void mat3::set_oz(const float3& v)
{
	m02 = v.x;
	m12 = v.y;
	m22 = v.z;
}


inline mat3& mat3::operator+=(const mat3& m)
{
	m00 += m.m00; m01 += m.m01; m02 += m.m02;
	m10 += m.m10; m11 += m.m11; m12 += m.m12;
	m20 += m.m20; m21 += m.m21; m22 += m.m22;
	return *this;
}

inline mat3& mat3::operator-=(const mat3& m)
{
	m00 -= m.m00; m01 -= m.m01; m02 -= m.m02;
	m10 -= m.m10; m11 -= m.m11; m12 -= m.m12;
	m20 -= m.m20; m21 -= m.m21; m22 -= m.m22;
	return *this;
}

inline mat3& mat3::operator*=(float val)
{
	m00 *= val; m01 *= val; m02 *= val;
	m10 *= val; m11 *= val; m12 *= val;
	m20 *= val; m21 *= val; m22 *= val;
	return *this;
}

inline mat3& mat3::operator*=(const mat3& m)
{
	float mp00, mp01, mp02;
	float mp10, mp11, mp12;
	float mp20, mp21, mp22;

	mp00 = m00 * m.m00 + m01 * m.m10 + m02 * m.m20;
	mp01 = m00 * m.m01 + m01 * m.m11 + m02 * m.m21;
	mp02 = m00 * m.m02 + m01 * m.m12 + m02 * m.m22;

	mp10 = m10 * m.m00 + m11 * m.m10 + m12 * m.m20;
	mp11 = m10 * m.m01 + m11 * m.m11 + m12 * m.m21;
	mp12 = m10 * m.m02 + m11 * m.m12 + m12 * m.m22;

	mp20 = m20 * m.m00 + m21 * m.m10 + m22 * m.m20;
	mp21 = m20 * m.m01 + m21 * m.m11 + m22 * m.m21;
	mp22 = m20 * m.m02 + m21 * m.m12 + m22 * m.m22;

	m00 = mp00; m01 = mp01; m02 = mp02;
	m10 = mp10; m11 = mp11; m12 = mp12;
	m20 = mp20; m21 = mp21; m22 = mp22;
	return *this;
}

inline mat3& mat3::operator/=(float val)
{
	assert(!approx_equal(val, 0.f));

	m00 /= val; m01 /= val; m02 /= val;
	m10 /= val; m11 /= val; m12 /= val;
	m20 /= val; m21 /= val; m22 /= val;
	return *this;
}


inline bool operator==(const mat3& lhs, const mat3& rhs)
{
	return approx_equal(lhs.m00, rhs.m00)
		&& approx_equal(lhs.m10, rhs.m10)
		&& approx_equal(lhs.m20, rhs.m20)

		&& approx_equal(lhs.m01, rhs.m01)
		&& approx_equal(lhs.m11, rhs.m11)
		&& approx_equal(lhs.m21, rhs.m21)

		&& approx_equal(lhs.m02, rhs.m02)
		&& approx_equal(lhs.m12, rhs.m12)
		&& approx_equal(lhs.m22, rhs.m22);
}

inline bool operator!=(const mat3& lhs, const mat3& rhs)
{
	return !(lhs == rhs);
}

inline mat3 operator+(const mat3& lhs, const mat3 rhs)
{
	return mat3(
		lhs.m00 + rhs.m00, lhs.m01 + rhs.m01, lhs.m02 + rhs.m02,
		lhs.m10 + rhs.m10, lhs.m11 + rhs.m11, lhs.m12 + rhs.m12,
		lhs.m20 + rhs.m20, lhs.m21 + rhs.m21, lhs.m22 + rhs.m22
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

inline mat3 operator*(const mat3& m, float val)
{
	return mat3(
		m.m00 * val, m.m01 * val, m.m02 * val,
		m.m10 * val, m.m11 * val, m.m12 * val,
		m.m20 * val, m.m21 * val, m.m22 * val
	);
}

inline mat3 operator*(float  val, const mat3& m)
{
	return mat3(
		m.m00 * val, m.m01 * val, m.m02 * val,
		m.m10 * val, m.m11 * val, m.m12 * val,
		m.m20 * val, m.m21 * val, m.m22 * val
	);
}

inline mat3 operator*(const mat3& lhs, const mat3& rhs)
{
	mat3 product;

	product.m00 = lhs.m00 * rhs.m00 + lhs.m01 * rhs.m10 + lhs.m02 * rhs.m20;
	product.m01 = lhs.m00 * rhs.m01 + lhs.m01 * rhs.m11 + lhs.m02 * rhs.m21;
	product.m02 = lhs.m00 * rhs.m02 + lhs.m01 * rhs.m12 + lhs.m02 * rhs.m22;
	
	product.m10 = lhs.m10 * rhs.m00 + lhs.m11 * rhs.m10 + lhs.m12 * rhs.m20;
	product.m11 = lhs.m10 * rhs.m01 + lhs.m11 * rhs.m11 + lhs.m12 * rhs.m21;
	product.m12 = lhs.m10 * rhs.m02 + lhs.m11 * rhs.m12 + lhs.m12 * rhs.m22;
	
	product.m20 = lhs.m20 * rhs.m00 + lhs.m21 * rhs.m10 + lhs.m22 * rhs.m20;
	product.m21 = lhs.m20 * rhs.m01 + lhs.m21 * rhs.m11 + lhs.m22 * rhs.m21;
	product.m22 = lhs.m20 * rhs.m02 + lhs.m21 * rhs.m12 + lhs.m22 * rhs.m22;

	return product;
}

inline mat3 operator/(const mat3& m, float  val)
{
	assert(!approx_equal(val, 0.f));

	return mat3(
		m.m00 / val, m.m01 / val, m.m02 / val,
		m.m10 / val, m.m11 / val, m.m12 / val,
		m.m20 / val, m.m21 / val, m.m22 / val
	);
}

inline std::ostream& operator<<(std::ostream& out, const mat3& m)
{
	out << "mat3("
		<< m.m00 << ", " << m.m01 << ", " << m.m02 << ",  "
		<< m.m10 << ", " << m.m11 << ", " << m.m12 << ",  "
		<< m.m20 << ", " << m.m21 << ", " << m.m22 << ")";

	return out;
}

inline std::wostream& operator<<(std::wostream& out, const mat3& m)
{
	out << "mat3("
		<< m.m00 << ", " << m.m01 << ", " << m.m02 << ",  "
		<< m.m10 << ", " << m.m11 << ", " << m.m12 << ",  "
		<< m.m20 << ", " << m.m21 << ", " << m.m22 << ")";

	return out;
}

inline float det(const mat3& m)
{
	return (m.m00 * m.m11 * m.m22) + (m.m01 * m.m12 * m.m20) + (m.m02 * m.m10 * m.m21)
		- (m.m02 * m.m11 * m.m20) - (m.m01 * m.m10 * m.m22) - (m.m00 * m.m12 * m.m21);
}

inline float3 mul(const mat3& m, const float2& v, float z)
{
	return mul(m, float3(v.x, v.y, z));
}

inline float3 mul(const mat3& m, const float3& v)
{
	return float3(
		(m.m00 * v.x) + (m.m01 * v.y) + (m.m02 * v.z),
		(m.m10 * v.x) + (m.m11 * v.y) + (m.m12 * v.z),
		(m.m20 * v.x) + (m.m21 * v.y) + (m.m22 * v.z)
	);
}

inline float* put_in_column_major_order(const mat3& m, float* arr)
{
	arr[0] = m.m00; arr[1] = m.m10; arr[2] = m.m20;
	arr[3] = m.m01; arr[4] = m.m11; arr[5] = m.m21;
	arr[6] = m.m02; arr[7] = m.m12; arr[8] = m.m22;
	return arr;
}

inline float* put_in_row_major_order(const mat3& m, float* arr)
{
	arr[0] = m.m00; arr[1] = m.m01; arr[2] = m.m02;
	arr[3] = m.m10; arr[4] = m.m11; arr[5] = m.m12;
	arr[6] = m.m20; arr[7] = m.m21; arr[8] = m.m22;

	return arr;
}

inline float trace(const mat3& m)
{
	return m.m00 + m.m11 + m.m22;
}

inline mat3 transpose(const mat3& m)
{
	return mat3(
		m.m00, m.m10, m.m20,
		m.m01, m.m11, m.m21,
		m.m02, m.m12, m.m22
	);
}

} // namespace cg

#endif // CG_MATH_MAT3_H_
