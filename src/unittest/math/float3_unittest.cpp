#include "cg/math/math.h"

#include <cmath>
#include <utility>
#include "unittest/math/math_unittest_utils.h"
#include "CppUnitTest.h"

using cg::float3;


namespace unittest {

TEST_CLASS(Float3_unittest) {
public:

	TEST_METHOD(static_members)
	{
		Assert::AreEqual(float3(0, 0, 0), float3::zero);
		Assert::AreEqual(float3(1, 0, 0), float3::unit_x);
		Assert::AreEqual(float3(0, 1, 0), float3::unit_y);
		Assert::AreEqual(float3(0, 0, 1), float3::unit_z);
		Assert::AreEqual(float3(1, 1, 1), float3::unit_xyz);
	}

	TEST_METHOD(ctors)
	{
		float3 v0;
		Assert::IsTrue((v0.x == 0) && (v0.y == 0) && (v0.z == 0));
		Assert::IsTrue((v0.r == 0) && (v0.g == 0) && (v0.b == 0));
		Assert::IsTrue((v0.width == 0) && (v0.height == 0) && (v0.depth == 0));

		float3 v1(24);
		Assert::IsTrue((v1.x == 24) && (v1.y == 24) && (v1.z == 24));
		Assert::IsTrue((v1.r == 24) && (v1.g == 24) && (v1.b == 24));
		Assert::IsTrue((v1.width == 24) && (v1.height == 24) && (v1.depth == 24));

		float3 v3(1, 2, 3);
		Assert::IsTrue((v3.x == 1) && (v3.y == 2) && (v3.z == 3));
		Assert::IsTrue((v3.r == 1) && (v3.g == 2) && (v3.b == 3));
		Assert::IsTrue((v3.width == 1) && (v3.height == 2) && (v3.depth == 3));

		// copy ctor
		float3 vc = v3;
		Assert::IsTrue((vc.x == v3.x) && (vc.y == v3.y) && (vc.z == v3.z));
		Assert::IsTrue((vc.r == v3.r) && (vc.g == v3.g) && (vc.b == v3.b));
		Assert::IsTrue((vc.width == v3.width) && (vc.height == v3.height) && (vc.depth == v3.depth));

		// move ctor
		float3 vm = std::move(v3);
		Assert::IsTrue((vm.x == vc.x) && (vm.y == vc.y) && (vm.z == vc.z));
		Assert::IsTrue((vm.r == vc.r) && (vm.g == vc.g) && (vm.b == vc.b));
		Assert::IsTrue((vm.width == vc.width) && (vm.height == vc.height) && (vm.depth == vc.depth));
	}

	TEST_METHOD(assignments)
	{
		float3 v(5, 6, 7);

		// copy assignment
		float3 vc;
		vc = v;
		Assert::IsTrue((vc.x == v.x) && (vc.y == v.y) && (vc.z == v.z));
		Assert::IsTrue((vc.r == v.r) && (vc.g == v.g) && (vc.b == v.b));
		Assert::IsTrue((vc.width == v.width) && (vc.height == v.height) && (vc.depth == v.depth));

		// move assignment
		float3 vm;
		vm = std::move(v);
		Assert::IsTrue((vm.x == v.x) && (vm.y == v.y) && (vm.z == v.z));
		Assert::IsTrue((vm.r == v.r) && (vm.g == v.g) && (vm.b == v.b));
		Assert::IsTrue((vm.width == v.width) && (vm.height == v.height) && (vm.depth == v.depth));
	}

	TEST_METHOD(compound_assignment_operators)
	{
		float3 v(1, 2, 3);
		(v += 5) += 5;
		Assert::AreEqual(float3(11, 12, 13), v);

		(v -= 7) -= 3;
		Assert::AreEqual(float3(1, 2, 3), v);

		(v *= 2) *= 3;
		Assert::AreEqual(float3(6, 12, 18), v);

		(v /= 3) /= 2;
		Assert::AreEqual(float3(1, 2, 3), v);

		(v += v) += v;
		Assert::AreEqual(float3(4, 8, 12), v);

		v -= v;
		Assert::AreEqual(float3::zero, v);
	}

	TEST_METHOD(xy_uv)
	{
		using cg::float2;

		float3 v(1, 2, 3);

		Assert::AreEqual(float2(1, 2), v.xy());
		Assert::AreEqual(float2(1, 2), v.uv());
	}


	TEST_METHOD(binary_operators)
	{
		float3 v(1, 2, 3);

		// operator+
		Assert::AreEqual(float3(11, 12, 13), v + 10);
		Assert::AreEqual(float3(11, 12, 13), 10 + v);
		Assert::AreEqual(float3(1.11f, 2.22f, 3.33f), v + float3(0.11f, 0.22f, 0.33f));
		Assert::AreEqual(float3(1.11f, 2.22f, 3.33f), float3(0.11f, 0.22f, 0.33f) + v);

		// operator-
		Assert::AreEqual(float3(0, 1, 2), v - 1);
		Assert::AreEqual(float3(0, -1, -2), 1 - v);
		Assert::AreEqual(float3::zero, v - float3(1, 2, 3));
		Assert::AreEqual(float3::zero, float3(1, 2, 3) - v);

		// operator*
		Assert::AreEqual(float3(10, 20, 30), v * 10);
		Assert::AreEqual(float3(10, 20, 30), 10 * v);
		
		// operator/
		Assert::AreEqual(float3(0.5, 1, 1.5), v / 2);
		Assert::AreEqual(float3::zero, 0 / v);
	}

	TEST_METHOD(clamp)
	{
		using cg::clamp;

		float3 lo(-5, -7, -9);
		float3 hi(5, 7, 9);
		float3 v(2, 3, 4);

		Assert::AreEqual(lo, clamp(float3(-9, -10, -11), lo, hi));
		Assert::AreEqual(v, clamp(v, lo, hi));
		Assert::AreEqual(hi, clamp(float3(9, 10, 11), lo, hi));

		// default lo and hi
		Assert::AreEqual(float3::zero, clamp(float3(-5)));
		Assert::AreEqual(float3(0.5), clamp(float3(0.5)));
		Assert::AreEqual(float3::unit_xyz, clamp(float3(5)));
	}

	TEST_METHOD(equal_operator)
	{
		float3 v(1, 2, 3);

		Assert::AreNotEqual(v, float3(100, 2, 3));
		Assert::AreNotEqual(v, float3(1, 100, 3));
		Assert::AreNotEqual(v, float3(1, 2, 100));

		Assert::AreEqual(v, v);
		Assert::AreEqual(v, float3(1, 2, 3));
	}

	TEST_METHOD(cross_product)
	{
		float3 u(1, 2, 3);
		float3 v(4, 5, -6);
		float3 w(7, -8, 9);

		Assert::AreEqual(cg::cross(u, v), cg::cross(-v, u), L"U x V = -V x U");
		
		Assert::AreEqual(
			cg::cross((5 * u) + (7 * v), w), 
			5 * cross(u, w) + 7 * cross(v, w), 
			L"(aU + bV) x W = a(U x W) + b(V x W)");
		
		Assert::AreEqual(
			cg::cross(u + v, w), 
			cg::cross(u, w) + cg::cross(v, w), 
			L"(U + V) x W = U x W + V x W");

		Assert::AreEqual(
			cg::cross(w, u + v), 
			cg::cross(w, u) + cg::cross(w, v), 
			L"W x (U + V) = W x U + W x V");
		
		Assert::AreEqual(
			cg::cross(u, cg::cross(v, w)), 
			(cg::dot(u, w) * v) - (cg::dot(u, v) * w), 
			L"U x (V x W) = (U * W) * V - (U * V) * W");

		auto r1 = 5 * cg::cross(u, v);
		auto r2 = cg::cross(5 * u, v);
		auto r3 = cg::cross(u, 5 * v);
		Assert::IsTrue(r1 == r2 && r2 == r3, L"(aU) x V = U x (aV) = a(U x V)");

		// scalar triple product
		float s1 = cg::dot(cg::cross(u, v), w);		// (U x V)*W
		float s2 = cg::dot(cg::cross(v, w), u);		// (V x W)*U
		float s3 = cg::dot(cg::cross(w, u), v);		// (W x U)*V
		Assert::IsTrue(s1 == s2 && s2 == s3);
		Assert::AreEqual(s1, cg::dot(-cg::cross(v, u), w), L"(U x V)*W = -(V x U)*W");
		Assert::AreEqual(s2, cg::dot(-cg::cross(w, v), u), L"(V x W)*U = -(W x V)*U");
		Assert::AreEqual(s3, cg::dot(-cg::cross(u, w), v), L"(W x U)*V = -(U x W)*V");

		auto uv = cg::cross(u, v);
		Assert::AreEqual(0.f, cg::dot(uv, u));
		Assert::AreEqual(0.f,  cg::dot(uv, v));
		Assert::AreEqual(float3::zero, cg::cross(u, u));
	}

	TEST_METHOD(dot_product)
	{
		using cg::dot;
		using cg::len_squared;

		float3 u(2, 3, 4);
		float3 v(4, 5, 6);
		float3 w(7, 8 ,9);

		Assert::AreEqual(2.f*4 + 3*5 + 4*6, dot(u, v));
		Assert::AreEqual(0.f, dot(u, float3::zero));
		Assert::AreEqual(len_squared(u), dot(u, u), L"U * U = |U| * |U|");
		Assert::AreEqual(dot(u, v), dot(v, u), L"U * V = V * U");
		Assert::AreEqual(dot(2*u, v), 2 * dot(u, v), L"(aU) * V = a(U * V)");
		Assert::AreEqual(dot(u + v, w), dot(u, w) + dot(v, w), L"(U + V)*W = U*W + V*W");
	}

	TEST_METHOD(greater_than)
	{
		using cg::greater_than;

		Assert::IsFalse(greater_than(float3(0, 5, 7), 1));
		Assert::IsFalse(greater_than(float3(1, 5, 7), 1));
		Assert::IsFalse(greater_than(float3(4, 0, 7), 1));
		Assert::IsFalse(greater_than(float3(4, 1, 7), 1));
		Assert::IsFalse(greater_than(float3(4, 5, 0), 1));
		Assert::IsFalse(greater_than(float3(4, 5, 1), 1));

		Assert::IsTrue(greater_than(float3(4, 5, 7), 1));
	}

	TEST_METHOD(is_normalized)
	{
		using cg::is_normalized;
		using cg::len;

		Assert::IsTrue(is_normalized(float3::unit_x));
		Assert::IsTrue(is_normalized(float3::unit_y));
		Assert::IsTrue(is_normalized(float3::unit_z));

		float3 v(1, 2, 3);
		Assert::IsTrue(len(v) > 1 && (!is_normalized(v)));
	}

	TEST_METHOD(len_and_len_squared)
	{
		using cg::approx_equal;
		using cg::len;
		using cg::len_squared;

		float3 u(2, 3, 4);
		float3 v(4, 5, 6);

		Assert::AreEqual(2.f*2 + 3*3 + 4*4, len_squared(u));
		Assert::AreEqual(std::sqrt(2.f*2 + 3*3 + 4*4), len(u));

		Assert::AreEqual(2 * len(u), len(2 * u), L"|aU| = |a| * |U|");
		Assert::IsTrue(approx_equal(len(u + v), len(u) + len(v), 0.1f), L"|U + V| <= |U| + |V|");
	}

	TEST_METHOD(lerp)
	{
		using cg::lerp;

		Assert::AreEqual(float3(0.0f), lerp(float3::zero, float3::unit_xyz, 0.0f));
		Assert::AreEqual(float3(0.6f), lerp(float3::zero, float3::unit_xyz, 0.6f));
		Assert::AreEqual(float3(1.0f), lerp(float3::zero, float3::unit_xyz, 1.0f));

		float3 v(24);
		Assert::AreEqual(v, lerp(v, v, 0.4f));
		Assert::AreEqual(v, lerp(v, v, 0.7f));
	}

	TEST_METHOD(normalize)
	{
		using cg::approx_equal;
		using cg::len;
		using cg::normalize;

		Assert::AreEqual(float3::zero, normalize(float3::zero));
		Assert::AreEqual(float3::unit_x, normalize(float3::unit_x));
		Assert::AreEqual(float3::unit_y, normalize(float3::unit_y));
		Assert::AreEqual(float3::unit_z, normalize(float3::unit_z));

		Assert::AreEqual(float3::unit_x, normalize(float3(24, 0, 0)));
		Assert::AreEqual(float3::unit_y, normalize(float3(0, 24, 0)));
		Assert::AreEqual(float3::unit_z, normalize(float3(0, 0, 24)));

		float3 u(-8, 6, 24);
		Assert::IsTrue(approx_equal(1.f, len(normalize(u))));
	}

	TEST_METHOD(rgb)
	{
		Assert::AreEqual(float3::zero, cg::rgb(0));
		Assert::AreEqual(float3::unit_x, cg::rgb(0xff'00'00));
		Assert::AreEqual(float3::unit_y, cg::rgb(0x00'ff'00));
		Assert::AreEqual(float3::unit_z, cg::rgb(0x00'00'ff));
		Assert::AreEqual(float3::unit_xyz, cg::rgb(0xffffff));

		Assert::AreEqual(float3(0xa1 / 255.f, 0xb2 / 255.f, 0xe3 / 255.f), cg::rgb(0xa1'b2'e3));
	}

	TEST_METHOD(unary_operators)
	{
		float3 v(1, 2, 3);

		Assert::AreEqual(float3(-1, -2, -3), -v);
		Assert::AreEqual(float3(1, 2, 3), -(-v));
	}
};

} // unittest
