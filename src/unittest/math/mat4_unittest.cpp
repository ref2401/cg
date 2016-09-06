#include "cg/math/mat4.h"

#include <utility>
#include "unittest/math/math_unittest_utils.h"

using cg::mat4;


namespace unittest {

TEST_CLASS(Mat4_unittest) {
public:

	TEST_METHOD(static_members)
	{
		Assert::AreEqual(mat4(1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1), mat4::identity);
		Assert::AreEqual(mat4(), mat4::zero);
	}

	TEST_METHOD(ctors)
	{
		mat4 m;
		Assert::IsTrue(m.m00 == 0 && m.m01 == 0 && m.m02 == 0 && m.m03 == 0);
		Assert::IsTrue(m.m10 == 0 && m.m11 == 0 && m.m12 == 0 && m.m13 == 0);
		Assert::IsTrue(m.m20 == 0 && m.m21 == 0 && m.m22 == 0 && m.m23 == 0);
		Assert::IsTrue(m.m30 == 0 && m.m31 == 0 && m.m32 == 0 && m.m33 == 0);

		mat4 m1(1, 2, 3, 4,  5, 6, 7, 8,  9, 10, 11, 12,  13, 14, 15, 16);
		Assert::IsTrue(m1.m00 == 1 && m1.m01 == 2 && m1.m02 == 3 && m1.m03 == 4);
		Assert::IsTrue(m1.m10 == 5 && m1.m11 == 6 && m1.m12 == 7 && m1.m13 == 8);
		Assert::IsTrue(m1.m20 == 9 && m1.m21 == 10 && m1.m22 == 11 && m1.m23 == 12);
		Assert::IsTrue(m1.m30 == 13 && m1.m31 == 14 && m1.m32 == 15 && m1.m33 == 16);

		// copy ctor
		mat4 m2 = m1;
		Assert::IsTrue(m2.m00 == m1.m00 && m2.m01 == m1.m01 && m2.m02 == m1.m02 && m2.m03 == m1.m03);
		Assert::IsTrue(m2.m10 == m1.m10 && m2.m11 == m1.m11 && m2.m12 == m1.m12 && m2.m13 == m1.m13);
		Assert::IsTrue(m2.m20 == m1.m20 && m2.m21 == m1.m21 && m2.m22 == m1.m22 && m2.m23 == m1.m23);
		Assert::IsTrue(m2.m30 == m1.m30 && m2.m31 == m1.m31 && m2.m32 == m1.m32 && m2.m33 == m1.m33);

		// move ctor
		mat4 mm = std::move(m2);
		Assert::IsTrue(mm.m00 == m1.m00 && mm.m01 == m1.m01 && mm.m02 == m1.m02 && mm.m03 == m1.m03);
		Assert::IsTrue(mm.m10 == m1.m10 && mm.m11 == m1.m11 && mm.m12 == m1.m12 && mm.m13 == m1.m13);
		Assert::IsTrue(mm.m20 == m1.m20 && mm.m21 == m1.m21 && mm.m22 == m1.m22 && mm.m23 == m1.m23);
		Assert::IsTrue(mm.m30 == m1.m30 && mm.m31 == m1.m31 && mm.m32 == m1.m32 && mm.m33 == m1.m33);
	}

	TEST_METHOD(assignments)
	{
		mat4 m(0, 1, 2, 3,  4, 5, 6, 7,  8, 9, 10, 11,  12, 13, 14, 15);

		// copy assignment
		mat4 mc = m;
		Assert::IsTrue(mc.m00 == m.m00 && mc.m01 == m.m01 && mc.m02 == m.m02 && mc.m03 == m.m03);
		Assert::IsTrue(mc.m10 == m.m10 && mc.m11 == m.m11 && mc.m12 == m.m12 && mc.m13 == m.m13);
		Assert::IsTrue(mc.m20 == m.m20 && mc.m21 == m.m21 && mc.m22 == m.m22 && mc.m23 == m.m23);
		Assert::IsTrue(mc.m30 == m.m30 && mc.m31 == m.m31 && mc.m32 == m.m32 && mc.m33 == m.m33);

		// move assignment

		mat4 mv;
		mv = std::move(mc);
		Assert::IsTrue(mv.m00 == m.m00 && mv.m01 == m.m01 && mv.m02 == m.m02 && mv.m03 == m.m03);
		Assert::IsTrue(mv.m10 == m.m10 && mv.m11 == m.m11 && mv.m12 == m.m12 && mv.m13 == m.m13);
		Assert::IsTrue(mv.m20 == m.m20 && mv.m21 == m.m21 && mv.m22 == m.m22 && mv.m23 == m.m23);
		Assert::IsTrue(mv.m30 == m.m30 && mv.m31 == m.m31 && mv.m32 == m.m32 && mv.m33 == m.m33);
	}

	TEST_METHOD(compound_assignment_operators)
	{
		mat4 m(0, 1, 2, 3,  4, 5, 6, 7,  8, 9, 10, 11,  12, 13, 14, 15);
		mat4 r(5, 6, 7, 8,  9, 10, 11, 12,  13, 14, 15, 16,  17, 18, 19, 20);

		(m += r) += r;
		Assert::AreEqual(m, mat4(
			0 + 5 + 5, 1 + 6 + 6, 2 + 7 + 7, 3 + 8 + 8,
			4 + 9 + 9, 5 + 10 + 10, 6 + 11 + 11, 7 + 12 + 12,
			8 + 13 + 13, 9 + 14 + 14, 10 + 15 + 15, 11 + 16 + 16,
			12 + 17 + 17, 13 + 18 + 18, 14 + 19 + 19, 15 + 20 + 20));

		(m -= r) -= r;
		Assert::AreEqual(m, mat4(0, 1, 2, 3,  4, 5, 6, 7,  8, 9, 10, 11,  12, 13, 14, 15));

		(m *= 2) *= 3;
		Assert::AreEqual(m, mat4(
			0, 1*6, 2*6, 3*6,
			4*6, 5*6, 6*6, 7*6,
			8*6, 9*6, 10*6, 11*6,
			12*6, 13*6, 14*6, 15*6));

		(m /= 3) /= 2;
		Assert::AreEqual(m, mat4(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15));

		// matrix product
		mat4 mA(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
		mat4 mB(80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95);
		mat4 expectedAB(880, 890, 900, 910, 2256, 2282, 2308, 2334, 3632, 3674, 3716, 3758, 5008, 5066, 5124, 5182);
		Assert::AreEqual(expectedAB, (mA *= mB));
	}


	TEST_METHOD(binary_operators)
	{
		mat4 m(0, 1, 2, 3,  4, 5, 6, 7,  8, 9, 10, 11,  12, 13, 14, 15);
		mat4 l(5, 7, -9, 0,  3, 4, 4, 3,  9, 8, 7, 6,  1, 2, 1, 2);
		mat4 n(4, 5, 6, 7, 9, 8, -7, 6, 1, 2, 3, 4, 0, 0, -3, -4);

		Assert::AreEqual(m + l, mat4(5, 8, -7, 3,  7, 9, 10, 10,  17, 17, 17, 17,  13, 15, 15, 17));
		Assert::AreEqual(m - l, mat4(-5, -6, 11, 3, 1, 1, 2, 4, -1, 1, 3, 5,  11, 11, 13, 13));

		Assert::AreEqual(m + l, l + m);
		Assert::AreEqual((m + l) + n, m + (l + n));
		Assert::AreEqual(m, m + mat4::zero);
		Assert::AreEqual(mat4::zero, m - m);

		Assert::AreEqual(m * 2, mat4(0, 2, 4, 6,  8, 10, 12, 14,  16, 18, 20, 22,  24, 26, 28, 30));
		Assert::AreEqual(2 * m, mat4(0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30));
		Assert::AreEqual(m / 2, mat4(0, 0.5f, 1, 1.5f,  2, 2.5f, 3, 3.5f,  4, 4.5f, 5, 5.5f,  6, 6.5f, 7, 7.5f));

		// matrix product
		Assert::AreEqual(m * mat4::identity, mat4::identity * m);
		Assert::AreEqual(mat4::zero, m * mat4::zero);
		Assert::AreEqual((l * m) * n, l * (m * n));
		Assert::AreEqual((l + m) * n, (l * n) + (m * n));
		Assert::AreEqual(l * (m + n), (l * m) + (l * n));

		mat4 mA(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
		mat4 mB(80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95);
		mat4 expectedAB(880, 890, 900, 910, 2256, 2282, 2308, 2334, 3632, 3674, 3716, 3758, 5008, 5066, 5124, 5182);
		Assert::AreEqual(expectedAB, mA * mB);
	}

	TEST_METHOD(equal_operator)
	{
		mat4 m(0, 1, 2, 3,  4, 5, 6, 7,  8, 9, 10, 11,  12, 13, 14, 15);

		Assert::IsTrue(m != mat4(100, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15));
		Assert::IsTrue(m != mat4(0, 100, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15));
		Assert::IsTrue(m != mat4(0, 1, 100, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15));
		Assert::IsTrue(m != mat4(0, 1, 2, 100, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15));
		Assert::IsTrue(m != mat4(0, 1, 2, 3, 100, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15));
		Assert::IsTrue(m != mat4(0, 1, 2, 3, 4, 100, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15));
		Assert::IsTrue(m != mat4(0, 1, 2, 3, 4, 5, 100, 7, 8, 9, 10, 11, 12, 13, 14, 15));
		Assert::IsTrue(m != mat4(0, 1, 2, 3, 4, 5, 6, 100, 8, 9, 10, 11, 12, 13, 14, 15));
		Assert::IsTrue(m != mat4(0, 1, 2, 3, 4, 5, 6, 7, 100, 9, 10, 11, 12, 13, 14, 15));
		Assert::IsTrue(m != mat4(0, 1, 2, 3, 4, 5, 6, 7, 8, 100, 10, 11, 12, 13, 14, 15));
		Assert::IsTrue(m != mat4(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 100, 11, 12, 13, 14, 15));
		Assert::IsTrue(m != mat4(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 100, 12, 13, 14, 15));
		Assert::IsTrue(m != mat4(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 100, 13, 14, 15));
		Assert::IsTrue(m != mat4(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 100, 14, 15));
		Assert::IsTrue(m != mat4(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 100, 15));
		Assert::IsTrue(m != mat4(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 100));
		
		Assert::IsTrue(m == mat4(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15));
	}

	TEST_METHOD(ox_oy_oz)
	{
		using cg::float3;

		mat4 m(
			0, 1, 2, 3,  
			4, 5, 6, 7,  
			8, 9, 10, 11,  
			12, 13, 14, 15);
		
		Assert::AreEqual(float3(0, 4, 8), m.ox());
		Assert::AreEqual(float3(1, 5, 9), m.oy());
		Assert::AreEqual(float3(2, 6, 10), m.oz());

		m.set_ox(float3(21, 22, 23));
		m.set_oy(float3(24, 25, 26));
		m.set_oz(float3(27, 28, 29));
		Assert::AreEqual(mat4(21, 24, 27, 3,  22, 25, 28, 7,  23, 26, 29, 11,  12, 13, 14, 15), m);
	}

	TEST_METHOD(trace)
	{
		using cg::trace;
		using cg::transpose;

		Assert::AreEqual(0.f, trace(mat4::zero));
		Assert::AreEqual(4.f, trace(mat4::identity));
		
		mat4 m(0, 1, 2, 3,  4, 5, 6, 7,  8, 9, 10, 11,  12, 13, 14, 15);
		mat4 l(5, 7, -9, 0, 3, 4, 4, 3, 9, 8, 7, 6, 1, 2, 1, 2);

		Assert::AreEqual(0.f + 5 + 10 + 15, trace(m));

		Assert::AreEqual(trace(m * l), trace(l * m));
		Assert::AreEqual(trace(m), trace(transpose(m)));
	}

	TEST_METHOD(transpose)
	{
		using cg::transpose;

		Assert::AreEqual(mat4::zero, transpose(mat4::zero));
		Assert::AreEqual(mat4::zero, transpose(mat4::zero));

		mat4 m = mat4(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
		mat4 n = mat4(4, 5, 6, 7, 9, 8, -7, 6, 1, 2, 3, 4, 0, 0, -3, -4);
		mat4 mt = mat4(1, 5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 15, 4, 8, 12, 16);
		
		Assert::AreEqual(mt, transpose(m));
		Assert::AreEqual(m, transpose(transpose(m)));

		// transpose properties
		Assert::AreEqual(transpose(5.f * m), 5.f * transpose(m));
		Assert::AreEqual(transpose(m + n), transpose(m) + transpose(n));
		Assert::AreEqual(transpose(m * n), transpose(n) * transpose(m), L"(MN)^T == (N)^T * (M)^T");
	}
};

} // namespace unittest