#include "cg/math/matrix.h"

namespace cg {

const mat3 mat3::identity(1, 0, 0, 0, 1, 0, 0, 0, 1);
const mat3 mat3::zero;

const mat4 mat4::identity(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
const mat4 mat4::zero;

// ----- mat3 ----

mat3& mat3::operator*=(const mat3& m) noexcept
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

// ----- mat4 -----

mat4& mat4::operator*=(const mat4& m) noexcept
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

// ---- funcs -----

bool operator==(const mat3& lhs, const mat3& rhs) noexcept
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

bool operator==(const mat4& lhs, const mat4& rhs) noexcept
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

mat3 operator*(const mat3& lhs, const mat3& rhs) noexcept
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

mat4 operator*(const mat4& lhs, const mat4& rhs) noexcept
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

std::ostream& operator<<(std::ostream& out, const mat3& m)
{
	out << "mat3("
		<< m.m00 << ", " << m.m01 << ", " << m.m02 << ",  "
		<< m.m10 << ", " << m.m11 << ", " << m.m12 << ",  "
		<< m.m20 << ", " << m.m21 << ", " << m.m22 << ")";

	return out;
}

std::wostream& operator<<(std::wostream& out, const mat3& m)
{
	out << "mat3("
		<< m.m00 << ", " << m.m01 << ", " << m.m02 << ",  "
		<< m.m10 << ", " << m.m11 << ", " << m.m12 << ",  "
		<< m.m20 << ", " << m.m21 << ", " << m.m22 << ")";

	return out;
}

std::ostream& operator<<(std::ostream& out, const mat4& m)
{
	out << "mat4("
		<< m.m00 << ", " << m.m01 << ", " << m.m02 << ", " << m.m03 << ",  "
		<< m.m10 << ", " << m.m11 << ", " << m.m12 << ", " << m.m13 << ",  "
		<< m.m20 << ", " << m.m21 << ", " << m.m22 << ", " << m.m23 << ",  "
		<< m.m30 << ", " << m.m31 << ", " << m.m32 << ", " << m.m33 << ")";

	return out;
}

std::wostream& operator<<(std::wostream& out, const mat4& m)
{
	out << "mat4("
		<< m.m00 << ", " << m.m01 << ", " << m.m02 << ", " << m.m03 << ",  "
		<< m.m10 << ", " << m.m11 << ", " << m.m12 << ", " << m.m13 << ",  "
		<< m.m20 << ", " << m.m21 << ", " << m.m22 << ", " << m.m23 << ",  "
		<< m.m30 << ", " << m.m31 << ", " << m.m32 << ", " << m.m33 << ")";

	return out;
}


float det(const mat4& m) noexcept
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

mat3 inverse(const mat3& m)
{
	// inverse is found by Cramer’s rule.

	// Check whether m is a singular matix
	float d = det(m);
	assert(!approx_equal(d, 0.f));

	// construct the adjugate matrix.
	// cofactor00 cofactor10 cofactor20
	// cofactor01 cofactor11 cofactor21
	// cofactor02 cofactor12 cofactor22
	mat3 adj;
	adj.m00 = m.m11*m.m22 - m.m12*m.m21;
	adj.m01 = -(m.m01*m.m22 - m.m02*m.m21);
	adj.m02 = m.m01*m.m12 - m.m02*m.m11;

	adj.m10 = -(m.m10*m.m22 - m.m12*m.m20);
	adj.m11 = m.m00*m.m22 - m.m02*m.m20;
	adj.m12 = -(m.m00*m.m12 - m.m02*m.m10);

	adj.m20 = m.m10*m.m21 - m.m11*m.m20;
	adj.m21 = -(m.m00*m.m21 - m.m01*m.m20);
	adj.m22 = m.m00*m.m11 - m.m01*m.m10;

	float inv_d = 1.f / d;
	return adj * inv_d;
}

mat4 inverse(const mat4& m) noexcept
{
	// inverse is found by Cramer’s rule.

	// Check whether m is a singular matix
	float d = det(m);
	assert(!approx_equal(d, 0.f));


	// construct the adjugate matrix.
	// cofactor00 cofactor10 cofactor20 cofactor30
	// cofactor01 cofactor11 cofactor21 cofactor31
	// cofactor02 cofactor12 cofactor22 cofactor32
	// cofactor03 cofactor13 cofactor23 cofactor33
	mat4 adj;
	adj.m00 = m.m11*m.m22*m.m33 + m.m12*m.m23*m.m31 + m.m13*m.m21*m.m32 - m.m13*m.m22*m.m31 - m.m12*m.m21*m.m33 - m.m11*m.m23*m.m32;
	adj.m01 = -(m.m01*m.m22*m.m33 + m.m02*m.m23*m.m31 + m.m03*m.m21*m.m32 - m.m03*m.m22*m.m31 - m.m02*m.m21*m.m33 - m.m01*m.m23*m.m32);
	adj.m02 = m.m01*m.m12*m.m33 + m.m02*m.m13*m.m31 + m.m03*m.m11*m.m32 - m.m03*m.m12*m.m31 - m.m02*m.m11*m.m33 - m.m01*m.m13*m.m32;
	adj.m03 = -(m.m01*m.m12*m.m23 + m.m02*m.m13*m.m21 + m.m03*m.m11*m.m22 - m.m03*m.m12*m.m21 - m.m02*m.m11*m.m23 - m.m01*m.m13*m.m22);

	adj.m10 = -(m.m10*m.m22*m.m33 + m.m12*m.m23*m.m30 + m.m13*m.m20*m.m32 - m.m13*m.m22*m.m30 - m.m12*m.m20*m.m33 - m.m10*m.m23*m.m32);
	adj.m11 = m.m00*m.m22*m.m33 + m.m02*m.m23*m.m30 + m.m03*m.m20*m.m32 - m.m03*m.m22*m.m30 - m.m02*m.m20*m.m33 - m.m00*m.m23*m.m32;
	adj.m12 = -(m.m00*m.m12*m.m33 + m.m02*m.m13*m.m30 + m.m03*m.m10*m.m32 - m.m03*m.m12*m.m30 - m.m02*m.m10*m.m33 - m.m00*m.m13*m.m32);
	adj.m13 = m.m00*m.m12*m.m23 + m.m02*m.m13*m.m20 + m.m03*m.m10*m.m22 - m.m03*m.m12*m.m20 - m.m02*m.m10*m.m23 - m.m00*m.m13*m.m22;

	adj.m20 = m.m10*m.m21*m.m33 + m.m11*m.m23*m.m30 + m.m13*m.m20*m.m31 - m.m13*m.m21*m.m30 - m.m11*m.m20*m.m33 - m.m10*m.m23*m.m31;
	adj.m21 = -(m.m00*m.m21*m.m33 + m.m01*m.m23*m.m30 + m.m03*m.m20*m.m31 - m.m03*m.m21*m.m30 - m.m01*m.m20*m.m33 - m.m00*m.m23*m.m31);
	adj.m22 = m.m00*m.m11*m.m33 + m.m01*m.m13*m.m30 + m.m03*m.m10*m.m31 - m.m03*m.m11*m.m30 - m.m01*m.m10*m.m33 - m.m00*m.m13*m.m31;
	adj.m23 = -(m.m00*m.m11*m.m23 + m.m01*m.m13*m.m20 + m.m03*m.m10*m.m21 - m.m03*m.m11*m.m20 - m.m01*m.m10*m.m23 - m.m00*m.m13*m.m21);

	adj.m30 = -(m.m10*m.m21*m.m32 + m.m11*m.m22*m.m30 + m.m12*m.m20*m.m31 - m.m12*m.m21*m.m30 - m.m11*m.m20*m.m32 - m.m10*m.m22*m.m31);
	adj.m31 = m.m00*m.m21*m.m32 + m.m01*m.m22*m.m30 + m.m02*m.m20*m.m31 - m.m02*m.m21*m.m30 - m.m01*m.m20*m.m32 - m.m00*m.m22*m.m31;
	adj.m32 = -(m.m00*m.m11*m.m32 + m.m01*m.m12*m.m30 + m.m02*m.m10*m.m31 - m.m02*m.m11*m.m30 - m.m01*m.m10*m.m32 - m.m00*m.m12*m.m31);
	adj.m33 = m.m00*m.m11*m.m22 + m.m01*m.m12*m.m20 + m.m02*m.m10*m.m21 - m.m02*m.m11*m.m20 - m.m01*m.m10*m.m22 - m.m00*m.m12*m.m21;

	float inv_d = 1.f / d;
	return adj * inv_d;
}


} // namespace cg
