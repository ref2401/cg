#include "cg/math/math.h"

#include <cmath>
#include <utility>
#include "unittest/math/math_unittest_utils.h"
#include "CppUnitTest.h"

using cg::float4;


namespace unittest {

TEST_CLASS(Float4_unittest) {
public:

	TEST_METHOD(static_members)
	{
		Assert::AreEqual(float4(0, 0, 0, 0), float4::zero);
		Assert::AreEqual(float4(1, 0, 0, 0), float4::unit_x);
		Assert::AreEqual(float4(0, 1, 0, 0), float4::unit_y);
		Assert::AreEqual(float4(0, 0, 1, 0), float4::unit_z);
		Assert::AreEqual(float4(0, 0, 0, 1), float4::unit_w);
		Assert::AreEqual(float4(1, 1, 1, 1), float4::unit_xyzw);
	}

	TEST_METHOD(ctors) 
	{
		float4 v0;
		Assert::IsTrue((v0.x == 0) && (v0.y == 0) && (v0.z == 0) && (v0.w == 0));
		Assert::IsTrue((v0.r == 0) && (v0.g == 0) && (v0.b == 0) && (v0.a == 0));

		float4 v1(24);
		Assert::IsTrue((v1.x == 24) && (v1.y == 24) && (v1.z == 24) && (v1.w == 24));
		Assert::IsTrue((v1.r == 24) && (v1.g == 24) && (v1.b == 24) && (v1.w == 24));

		float4 v4(1, 2, 3, 4);
		Assert::IsTrue((v4.x == 1) && (v4.y == 2) && (v4.z == 3) && (v4.w == 4));
		Assert::IsTrue((v4.r == 1) && (v4.g == 2) && (v4.b == 3) && (v4.a == 4));

		// copy ctor
		float4 vc = v4;
		Assert::IsTrue((vc.x == v4.x) && (vc.y == v4.y) && (vc.z == v4.z) && (vc.w == v4.w));
		Assert::IsTrue((vc.r == v4.r) && (vc.g == v4.g) && (vc.b == v4.b) && (vc.a == v4.a));

		// move ctor
		float4 vm = std::move(v4);
		Assert::IsTrue((vm.x == vc.x) && (vm.y == vc.y) && (vm.z == vc.z) && (vm.w == vc.w));
		Assert::IsTrue((vm.r == vc.r) && (vm.g == vc.g) && (vm.b == vc.b) && (vm.a == vc.a));
	}

	TEST_METHOD(assignments)
	{
		float4 v (5, 6, 7, 8);

		// copy assignment
		float4 vc;
		vc = v;
		Assert::IsTrue((vc.x == v.x) && (vc.y == v.y) && (vc.z == v.z) && (vc.w == v.w));
		Assert::IsTrue((vc.r == v.r) && (vc.g == v.g) && (vc.b == v.b) && (vc.a == v.a));

		// move assignment
		float4 vm;
		vm = std::move(v);
		Assert::IsTrue((vm.x == v.x) && (vm.y == v.y) && (vm.z == v.z) && (vm.w == v.w));
		Assert::IsTrue((vm.r == v.r) && (vm.g == v.g) && (vm.b == v.b) && (vm.a == v.a));
	}

	TEST_METHOD(compound_assignment_operators)
	{
		float4 v(1, 2, 3, 4);
		(v += 5) += 5;
		Assert::AreEqual(float4(11, 12, 13, 14), v);

		(v -= 7) -= 3;
		Assert::AreEqual(float4(1, 2, 3, 4), v);

		(v *= 2) *= 3;
		Assert::AreEqual(float4(6, 12, 18, 24), v);

		(v /= 3) /= 2;
		Assert::AreEqual(float4(1, 2, 3, 4), v);

		(v += v) += v;
		Assert::AreEqual(float4(4, 8, 12, 16), v);

		v -= v;
		Assert::AreEqual(float4::zero, v);
	}

	TEST_METHOD(xy_uv_xyz_and_rgb)
	{
		using cg::float2;
		using cg::float3;

		float4 v(1, 2, 3, 4);

		Assert::AreEqual(float2(1, 2), v.xy());
		Assert::AreEqual(float2(1, 2), v.uv());
		Assert::AreEqual(float3(1, 2, 3), v.xyz());
		Assert::AreEqual(float3(1, 2, 3), v.rgb());
	}

	
	TEST_METHOD(binary_operators)
	{
		float4 v(1, 2, 3, 4);

		// operator+
		Assert::AreEqual(float4(11, 12, 13, 14), v + 10);
		Assert::AreEqual(float4(11, 12, 13, 14), 10 + v);
		Assert::AreEqual(float4(1.11f, 2.22f, 3.33f, 4.44f), v + float4(0.11f, 0.22f, 0.33f, 0.44f));
		Assert::AreEqual(float4(1.11f, 2.22f, 3.33f, 4.44f), float4(0.11f, 0.22f, 0.33f, 0.44f) + v);

		// operator-
		Assert::AreEqual(float4(0, 1, 2, 3), v - 1);
		Assert::AreEqual(float4(0, -1, -2, -3), 1 - v);
		Assert::AreEqual(float4::zero, v - float4(1, 2, 3, 4));
		Assert::AreEqual(float4::zero, float4(1, 2, 3, 4) - v);

		// operator*
		Assert::AreEqual(float4(10, 20, 30, 40), v * 10);
		Assert::AreEqual(float4(10, 20, 30, 40), 10 * v);

		// operator/
		Assert::AreEqual(float4(0.5, 1, 1.5, 2), v / 2);
		Assert::AreEqual(float4::zero, 0 / v);
	}

	TEST_METHOD(clamp)
	{
		using cg::clamp;

		float4 lo(-5, -7, -9, -11);
		float4 hi(5, 7, 9, 11);
		float4 v(2, 3, 4, 5);

		Assert::AreEqual(lo, clamp(float4(-9, -10, -11, -12), lo, hi));
		Assert::AreEqual(v, clamp(v, lo, hi));
		Assert::AreEqual(hi, clamp(float4(9, 10, 11, 12), lo, hi));

		// default lo and hi
		Assert::AreEqual(float4::zero, clamp(float4(-5)));
		Assert::AreEqual(float4(0.5), clamp(float4(0.5)));
		Assert::AreEqual(float4::unit_xyzw, clamp(float4(5)));
	}

	TEST_METHOD(equal_operator)
	{
		float4 v(1, 2, 3, 4);

		Assert::AreNotEqual(v, float4(100, 2, 3, 4));
		Assert::AreNotEqual(v, float4(1, 100, 3, 4));
		Assert::AreNotEqual(v, float4(1, 2, 100, 4));
		Assert::AreNotEqual(v, float4(1, 2, 3, 100));

		Assert::AreEqual(v, v);
		Assert::AreEqual(v, float4(1, 2, 3, 4));
	}

	TEST_METHOD(dot_product)
	{
		using cg::dot;
		using cg::len_squared;

		float4 u(2, 3, 4, 5);
		float4 v(5, 6, 7, 8);
		float4 w(9, 10, 11, 12);

		Assert::AreEqual(2.f*5 + 3*6 + 4*7 + 5*8, dot(u, v));
		Assert::AreEqual(0.f, cg::dot(u, float4::zero));
		Assert::AreEqual(len_squared(u), dot(u, u), L"U * U = |U| * |U|");
		Assert::AreEqual(dot(u, v), dot(v, u), L"U * V = V * U");
		Assert::AreEqual(dot(2 * u, v), 2 * dot(u, v), L"(aU) * V = a(U * V)");
		Assert::AreEqual(dot(u + v, w), dot(u, w) + dot(v, w), L"(U + V)*W = U*W + V*W");
	}

	TEST_METHOD(is_normalized)
	{
		using cg::is_normalized;
		using cg::len;

		Assert::IsTrue(is_normalized(float4::unit_x));
		Assert::IsTrue(is_normalized(float4::unit_y));
		Assert::IsTrue(is_normalized(float4::unit_z));
		Assert::IsTrue(is_normalized(float4::unit_w));

		float4 v(1, 2, 3, 4);
		Assert::IsTrue(len(v) > 1 && (!is_normalized(v)));
	}

	TEST_METHOD(len_and_len_squared)
	{
		using cg::approx_equal;
		using cg::len;
		using cg::len_squared;

		float4 u(2, 3, 4, 5);
		float4 v(4, 5, 6, 7);

		Assert::AreEqual(2.f*2 + 3*3 + 4*4 + 5*5, len_squared(u));
		Assert::AreEqual(std::sqrt(2.f*2 + 3*3 + 4*4 + 5*5), len(u));

		Assert::AreEqual(2 * len(u), len(2 * u), L"|aU| = |a| * |U|");
		Assert::IsTrue(approx_equal(cg::len(u + v), len(u) + len(v), 0.1f), L"|U + V| <= |U| + |V|");
	}

	TEST_METHOD(lerp)
	{
		using cg::lerp;

		Assert::AreEqual(float4(0.0f), lerp(float4::zero, float4::unit_xyzw, 0.0f));
		Assert::AreEqual(float4(0.6f), lerp(float4::zero, float4::unit_xyzw, 0.6f));
		Assert::AreEqual(float4(1.0f), lerp(float4::zero, float4::unit_xyzw, 1.0f));

		float4 v(24);
		Assert::AreEqual(v, lerp(v, v, 0.4f));
		Assert::AreEqual(v, lerp(v, v, 0.7f));
	}

	TEST_METHOD(normalize)
	{
		using cg::approx_equal;
		using cg::len;
		using cg::normalize;

		Assert::AreEqual(float4::zero, normalize(float4::zero));
		Assert::AreEqual(float4::unit_x, normalize(float4::unit_x));
		Assert::AreEqual(float4::unit_y, normalize(float4::unit_y));
		Assert::AreEqual(float4::unit_z, normalize(float4::unit_z));
		Assert::AreEqual(float4::unit_w, normalize(float4::unit_w));

		Assert::AreEqual(float4::unit_x, normalize(float4(24, 0, 0, 0)));
		Assert::AreEqual(float4::unit_y, normalize(float4(0, 24, 0, 0)));
		Assert::AreEqual(float4::unit_z, normalize(float4(0, 0, 24, 0)));
		Assert::AreEqual(float4::unit_w, normalize(float4(0, 0, 0, 24)));

		float4 u(-8, 6, 24, -0.1f);
		Assert::IsTrue(approx_equal(1.f, len(normalize(u))));
	}

	TEST_METHOD(rgba)
	{
		Assert::AreEqual(float4::zero, cg::rgba(0));
		Assert::AreEqual(float4::unit_x, cg::rgba(0xff'00'00'00));
		Assert::AreEqual(float4::unit_y, cg::rgba(0x00'ff'00'00));
		Assert::AreEqual(float4::unit_z, cg::rgba(0x00'00'ff'00));
		Assert::AreEqual(float4::unit_w, cg::rgba(0x00'00'00'ff));
		Assert::AreEqual(float4::unit_xyzw, cg::rgba(0xffffffff));

		Assert::AreEqual(float4(0xA1 / 255.f, 0xB2 / 255.f, 0xE3 / 255.f, 0x18 / 255.f), cg::rgba(0xa1'b2'e3'18));
	}

	TEST_METHOD(unary_operators)
	{
		float4 v(1, 2, 3, 4);

		Assert::AreEqual(float4(-1, -2, -3, -4), -v);
		Assert::AreEqual(float4(1, 2, 3, 4), -(-v));
	}
};

} // namespace unittest