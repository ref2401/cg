#include "cg/math/math.h"

#include <cmath>
#include <algorithm>
#include <type_traits>
#include <utility>
#include "unittest/math/common_math.h"

using cg::mat3;


namespace unittest {

TEST_CLASS(cg_math_Mat3) {
	TEST_METHOD(static_members)
	{
		Assert::AreEqual(mat3(1, 0, 0, 0, 1, 0, 0, 0, 1), mat3::identity);
		Assert::AreEqual(mat3(), mat3::zero);
	}

	TEST_METHOD(ctors)
	{
		mat3 m;
		Assert::IsTrue(m.m00 == 0 && m.m01 == 0 && m.m02 == 0);
		Assert::IsTrue(m.m10 == 0 && m.m11 == 0 && m.m12 == 0);
		Assert::IsTrue(m.m20 == 0 && m.m21 == 0 && m.m22 == 0);

		mat3 m1(1, 2, 3, 4, 5, 6, 7, 8, 9);
		Assert::IsTrue(m1.m00 == 1 && m1.m01 == 2 && m1.m02 == 3);
		Assert::IsTrue(m1.m10 == 4 && m1.m11 == 5 && m1.m12 == 6);
		Assert::IsTrue(m1.m20 == 7 && m1.m21 == 8 && m1.m22 == 9);

		// copy ctor
		mat3 m2 = m1;
		Assert::IsTrue(m2.m00 == m1.m00 && m2.m01 == m1.m01 && m2.m02 == m1.m02);
		Assert::IsTrue(m2.m10 == m1.m10 && m2.m11 == m1.m11 && m2.m12 == m1.m12);
		Assert::IsTrue(m2.m20 == m1.m20 && m2.m21 == m1.m21 && m2.m22 == m1.m22);

		// move ctor
		mat3 mm = std::move(m2);
		Assert::IsTrue(mm.m00 == m1.m00 && mm.m01 == m1.m01 && mm.m02 == m1.m02);
		Assert::IsTrue(mm.m10 == m1.m10 && mm.m11 == m1.m11 && mm.m12 == m1.m12);
		Assert::IsTrue(mm.m20 == m1.m20 && mm.m21 == m1.m21 && mm.m22 == m1.m22);
	}

	TEST_METHOD(assignments)
	{
		mat3 m(0, 1, 2, 3, 4, 5, 6, 7, 8);

		// copy assignment
		mat3 mc = m;
		Assert::IsTrue(mc.m00 == m.m00 && mc.m01 == m.m01 && mc.m02 == m.m02);
		Assert::IsTrue(mc.m10 == m.m10 && mc.m11 == m.m11 && mc.m12 == m.m12);
		Assert::IsTrue(mc.m20 == m.m20 && mc.m21 == m.m21 && mc.m22 == m.m22);

		// move assignment

		mat3 mv;
		mv = std::move(mc);
		Assert::IsTrue(mv.m00 == m.m00 && mv.m01 == m.m01 && mv.m02 == m.m02);
		Assert::IsTrue(mv.m10 == m.m10 && mv.m11 == m.m11 && mv.m12 == m.m12);
		Assert::IsTrue(mv.m20 == m.m20 && mv.m21 == m.m21 && mv.m22 == m.m22);
	}

	TEST_METHOD(compound_assignment_operators)
	{
		mat3 m(0, 1, 2, 3, 4, 5, 6, 7, 8);
		mat3 r(5, 6, 7, 8, 9, 10, 11, 12, 13);

		(m += r) += r;
		Assert::AreEqual(m, mat3(
			0 + 5 + 5, 1 + 6 + 6, 2 + 7 + 7, 
			3 + 8 + 8, 4 + 9 + 9, 5 + 10 + 10, 
			6 + 11 + 11, 7 + 12 + 12, 8 + 13 + 13));

		(m -= r) -= r;
		Assert::AreEqual(m, mat3(0, 1, 2, 3, 4, 5, 6, 7, 8));

		(m *= 2) *= 3;
		Assert::AreEqual(m, mat3(
			0, 1 * 6, 2 * 6, 
			3 * 6, 4 * 6, 5 * 6, 
			6 * 6, 7 * 6, 8 * 6));

		(m /= 3) /= 2;
		Assert::AreEqual(m, mat3(0, 1, 2, 3, 4, 5, 6, 7, 8));

		// matrix product
		mat3 mA(1, 2, 3, 4, 5, 6, 7, 8, 9);
		mat3 mB(80, 81, 82, 83, 84, 85, 86, 87, 88);
		mat3 expectedAB(504, 510, 516, 1251, 1266, 1281, 1998, 2022, 2046);
		Assert::AreEqual(expectedAB, (mA *= mB));
	}

	TEST_METHOD(ox_oy_oz)
	{
		using cg::float3;

		mat3 m(
			0, 1, 2, 
			3, 4, 5, 
			6, 7, 8);

		Assert::AreEqual(float3(0, 3, 6), m.ox());
		Assert::AreEqual(float3(1, 4, 7), m.oy());
		Assert::AreEqual(float3(2, 5, 8), m.oz());

		m.set_ox(float3(21, 22, 23));
		m.set_oy(float3(24, 25, 26));
		m.set_oz(float3(27, 28, 29));
		Assert::AreEqual(mat3(21, 24, 27, 22, 25, 28, 23, 26, 29), m);
	}


	TEST_METHOD(binary_operators)
	{
		mat3 m(0, 1, 2, 3, 4, 5, 6, 7, 8);
		mat3 l(5, 7, -9, 0, 3, 4, 4, 3, 9);
		mat3 n(4, 5, 6, 7, 9, 8, -7, 6, 1);

		Assert::AreEqual(m + l, mat3(5, 8, -7, 3, 7, 9, 10, 10, 17));
		Assert::AreEqual(m - l, mat3(-5, -6, 11, 3, 1, 1, 2, 4, -1));

		Assert::AreEqual(m + l, l + m);
		Assert::AreEqual((m + l) + n, m + (l + n));
		Assert::AreEqual(m, m + mat3::zero);
		Assert::AreEqual(mat3::zero, m - m);

		Assert::AreEqual(m * 2, mat3(0, 2, 4, 6, 8, 10, 12, 14, 16));
		Assert::AreEqual(2 * m, mat3(0, 2, 4, 6, 8, 10, 12, 14, 16));
		Assert::AreEqual(m / 2, mat3(0, 0.5f, 1, 1.5f, 2, 2.5f, 3, 3.5f, 4));

		// matrix product
		Assert::AreEqual(m * mat3::identity, mat3::identity * m);
		Assert::AreEqual(mat3::zero, m * mat3::zero);
		Assert::AreEqual((l * m) * n, l * (m * n));
		Assert::AreEqual((l + m) * n, (l * n) + (m * n));
		Assert::AreEqual(l * (m + n), (l * m) + (l * n));

		mat3 mA(1, 2, 3, 4, 5, 6, 7, 8, 9);
		mat3 mB(80, 81, 82, 83, 84, 85, 86, 87, 88);
		mat3 expectedAB(504, 510, 516, 1251, 1266, 1281, 1998, 2022, 2046);
		Assert::AreEqual(expectedAB, mA * mB);
	}

	TEST_METHOD(det)
	{
		using cg::det;
		using cg::transpose;

		Assert::AreEqual(0.f, det(mat3::zero));
		Assert::AreEqual(1.f, det(mat3::identity));
		Assert::AreEqual(0.f, det(mat3(1, 2, 3, 4, 5, 6, 7, 8, 9)));
		Assert::AreEqual(170.f, det(mat3(8, 9, 10, 2, 13, 4, -1, 8, 2)));

		mat3 m(1, 2, 3, 9, -3, 1, 0, 8, -2);
		mat3 n(0, -3, 4, 5, 6, 7, -1, 9, 2);

		Assert::AreEqual(det(m * n), det(m) * det(n));
		Assert::AreEqual(det(5.f * m), std::pow(5.f, 3)* det(m), L"|aM| = a^3 * |M|");
		Assert::AreEqual(det(m), det(transpose(m)));
	}

	TEST_METHOD(equal_operator)
	{
		mat3 m(0, 1, 2, 3, 4, 5, 6, 7, 8);

		Assert::IsTrue(m != mat3(100, 1, 2, 3, 4, 5, 6, 7, 8));
		Assert::IsTrue(m != mat3(0, 100, 2, 3, 4, 5, 6, 7, 8));
		Assert::IsTrue(m != mat3(0, 1, 100, 3, 4, 5, 6, 7, 8));
		Assert::IsTrue(m != mat3(0, 1, 2, 100, 4, 5, 6, 7, 8));
		Assert::IsTrue(m != mat3(0, 1, 2, 3, 100, 5, 6, 7, 8));
		Assert::IsTrue(m != mat3(0, 1, 2, 3, 4, 100, 6, 7, 8));
		Assert::IsTrue(m != mat3(0, 1, 2, 3, 4, 5, 100, 7, 8));
		Assert::IsTrue(m != mat3(0, 1, 2, 3, 4, 5, 6, 100, 8));
		Assert::IsTrue(m != mat3(0, 1, 2, 3, 4, 5, 6, 7, 100));

		Assert::IsTrue(m == mat3(0, 1, 2, 3, 4, 5, 6, 7, 8));
	}

	TEST_METHOD(inverse)
	{
		using cg::inverse;
		using cg::transpose;

		Assert::AreEqual(mat3::identity, inverse(mat3::identity));

		mat3 m(5, 7, -9, 0, 3, 4, 4, 3, 9);
		mat3 n(4, 5, 6, 7, 9, 8, -7, 6, 1);

		Assert::AreEqual(mat3::identity, m * inverse(m));
		Assert::AreEqual(m, inverse(inverse(m)));
		Assert::AreEqual(transpose(inverse(m)), inverse(transpose(m)));
		Assert::AreEqual(inverse(m * n), inverse(n) * inverse(m));
	}

	TEST_METHOD(is_orthogonal)
	{
		using cg::is_orthogonal;

		Assert::IsTrue(is_orthogonal(mat3::identity));
		Assert::IsFalse(is_orthogonal(mat3::zero));

		// NOTE(ref2401): The tests are not convincing at all. Try harder...
	}

	TEST_METHOD(mul)
	{
		using cg::float2;
		using cg::float3;
		using cg::mul;


		mat3 m(4, 5, 6, 7, 9, 8, -7, 6, 1);
		mat3 n(5, 7, -9, 0, 3, 4, 4, 3, 9);
		float3 v(1, 2, 3);

		Assert::AreEqual(float3::zero, mul(mat3::zero, v));
		Assert::AreEqual(v, mul(mat3::identity, v));
		Assert::AreEqual(mul(m * n, v), mul(m, mul(n, v)), L"(MN)u == M(Nu)");
		Assert::AreEqual(mul(m + n, v), mul(m, v) + mul(n, v), L"(M + N)u == Mu + Nu");

		float3 v1 = -5.f * mul(m, v);
		float3 v2 = mul(-5.f * m, v);
		float3 v3 = mul(m, -5.f * v);
		Assert::IsTrue(v1 == v2 && v2 == v3);

		float2 u = v.xy();
		Assert::AreEqual(mul(m, v), mul(m, u, v.z));
	}

	TEST_METHOD(put_in_column_major_row_major_order)
	{
		using cg::put_in_column_major_order;
		using cg::put_in_row_major_order;

		mat3 m(0, 1, 2, 
			3, 4, 5, 
			6, 7, 8);

		float *p, arr[9];
		constexpr size_t count = std::extent<decltype(arr)>::value;

		// column-major order
		float col[count] = { 0, 3, 6,  1, 4, 7,  2, 5, 8 };

		p = put_in_column_major_order(m, arr);
		Assert::AreEqual<float*>(p, arr);
		Assert::IsTrue(std::equal(col, col + count - 1, arr));

		// row-major order
		float row[count] = { 0, 1, 2,  3, 4, 5,  6, 7, 8 };

		p = put_in_row_major_order(m, arr);
		Assert::AreEqual<float*>(p, arr);
		Assert::IsTrue(std::equal(row, row + count - 1, arr));
	}

	TEST_METHOD(trace)
	{
		using cg::trace;
		using cg::transpose;

		Assert::AreEqual(0.f, trace(mat3::zero));
		Assert::AreEqual(3.f, trace(mat3::identity));

		mat3 m(0, 1, 2, 3, 4, 5, 6, 7, 8);
		mat3 l(5, 7, -9, 0, 3, 4, 4, 3, 9);

		Assert::AreEqual(0.f + 4 + 8, trace(m));

		Assert::AreEqual(trace(m * l), trace(l * m));
		Assert::AreEqual(trace(m), trace(transpose(m)));
	}

	TEST_METHOD(transpose)
	{
		using cg::transpose;

		Assert::AreEqual(mat3::zero, transpose(mat3::zero));
		Assert::AreEqual(mat3::zero, transpose(mat3::zero));

		mat3 m(1, 2, 3, 4, 5, 6, 7, 8, 9);
		mat3 n(4, 5, 6, 7, 9, 8, -7, 6, 1);
		mat3 mt(1, 4, 7, 2, 5, 8, 3, 6, 9);

		Assert::AreEqual(mt, transpose(m));
		Assert::AreEqual(m, transpose(transpose(m)));

		// transpose properties
		Assert::AreEqual(transpose(5.f * m), 5.f * transpose(m));
		Assert::AreEqual(transpose(m + n), transpose(m) + transpose(n));
		Assert::AreEqual(transpose(m * n), transpose(n) * transpose(m), L"(MN)^T == (N)^T * (M)^T");
	}
};

} // namespace unittest