#include "cg/math/math.h"

#include <cmath>
#include <utility>
#include "unittest/common.h"
#include "CppUnitTest.h"

using cg::float2;

namespace unittest {

TEST_CLASS(cg_math_Float2) {
public:

	TEST_METHOD(static_members)
	{
		Assert::AreEqual(float2(0, 0), float2::zero);
		Assert::AreEqual(float2(1, 0), float2::unit_x);
		Assert::AreEqual(float2(0, 1), float2::unit_y);
		Assert::AreEqual(float2(1, 1), float2::unit_xy);
	}

	TEST_METHOD(ctors)
	{
		float2 v0;
		Assert::IsTrue((v0.x == 0) && (v0.y == 0));
		Assert::IsTrue((v0.u == 0) && (v0.v == 0));

		float2 v1(24);
		Assert::IsTrue((v1.x == 24) && (v1.y == 24));
		Assert::IsTrue((v1.u == 24) && (v1.v == 24));

		float2 v2(1, 2);
		Assert::IsTrue((v2.x == 1) && (v2.y == 2));
		Assert::IsTrue((v2.u == 1) && (v2.v == 2));

		// copy ctor
		float2 vc = v2;
		Assert::IsTrue((vc.x == v2.x) && (vc.y == v2.y));
		Assert::IsTrue((vc.u == v2.u) && (vc.v == v2.v));

		// move ctor
		float2 vm = std::move(v2);
		Assert::IsTrue((vm.x == vc.x) && (vm.y == vc.y));
		Assert::IsTrue((vm.u == vc.u) && (vm.v == vc.v));
	}

	TEST_METHOD(assignments)
	{
		float2 v(5, 6);

		// copy assignment
		float2 vc;
		vc = v;
		Assert::IsTrue((vc.x == v.x) && (vc.y == v.y));
		Assert::IsTrue((vc.u == v.u) && (vc.v == v.v));

		// move assignment
		float2 vm;
		vm = std::move(v);
		Assert::IsTrue((vm.x == v.x) && (vm.y == v.y));
		Assert::IsTrue((vm.u == v.u) && (vm.v == v.v));
	}

	TEST_METHOD(compound_assignment_operators)
	{
		float2 v(1, 2);
		(v += 5) += 5;
		Assert::AreEqual(float2(11, 12), v);

		(v -= 7) -= 3;
		Assert::AreEqual(float2(1, 2), v);

		(v *= 2) *= 3;
		Assert::AreEqual(float2(6, 12), v);

		(v /= 3) /= 2;
		Assert::AreEqual(float2(1, 2), v);

		(v += v) += v;
		Assert::AreEqual(float2(4, 8), v);

		v -= v;
		Assert::AreEqual(float2::zero, v);
	}


	TEST_METHOD(binary_operators)
	{
		float2 v(1, 2);

		// operator+
		Assert::AreEqual(float2(11, 12), v + 10);
		Assert::AreEqual(float2(11, 12), 10 + v);
		Assert::AreEqual(float2(1.11f, 2.22f), v + float2(0.11f, 0.22f));
		Assert::AreEqual(float2(1.11f, 2.22f), float2(0.11f, 0.22f) + v);

		// operator-
		Assert::AreEqual(float2(0, 1), v - 1);
		Assert::AreEqual(float2(0, -1), 1 - v);
		Assert::AreEqual(float2::zero, v - float2(1, 2));
		Assert::AreEqual(float2::zero, float2(1, 2) - v);

		// operator*s
		Assert::AreEqual(float2(10, 20), v * 10);
		Assert::AreEqual(float2(10, 20), 10 * v);

		// operator/
		Assert::AreEqual(float2(0.5, 1), v / 2);
		Assert::AreEqual(float2::zero, 0.f / v);
	}

	TEST_METHOD(clamp)
	{
		using cg::clamp;

		float2 lo(-5, -7);
		float2 hi(5, 7);
		float2 v(2, 3);

		Assert::AreEqual(lo,clamp(float2(-9, -10), lo, hi));
		Assert::AreEqual(v, clamp(v, lo, hi));
		Assert::AreEqual(hi, clamp(float2(9, 10), lo, hi));

		// default lo and hi
		Assert::AreEqual(float2::zero, clamp(float2(-5)));
		Assert::AreEqual(float2(0.5), clamp(float2(0.5)));
		Assert::AreEqual(float2::unit_xy, clamp(float2(5)));
	}

	TEST_METHOD(dot_product)
	{
		using cg::dot;
		using cg::len_squared;

		float2 u(2, 3);
		float2 v(4, 5);
		float2 w(7, 8);

		Assert::AreEqual(2.f * 4 + 3 * 5, dot(u, v));
		Assert::AreEqual(0.f, dot(u, float2::zero));
		Assert::AreEqual(len_squared(u), dot(u, u), L"U * U = |U| * |U|");
		Assert::AreEqual(dot(u, v), dot(v, u), L"U * V = V * U");
		Assert::AreEqual(dot(2 * u, v), 2 * dot(u, v), L"(aU) * V = a(U * V)");
		Assert::AreEqual(dot(u + v, w), dot(u, w) + dot(v, w), L"(U + V)*W = U*W + V*W");
	}

	TEST_METHOD(equal_operator)
	{
		float2 v(1, 2);

		Assert::AreNotEqual(v, float2(100, 2));
		Assert::AreNotEqual(v, float2(1, 100));

		Assert::AreEqual(v, v);
		Assert::AreEqual(v, float2(1, 2));
	}

	TEST_METHOD(greater_than)
	{
		using cg::greater_than;

		Assert::IsFalse(greater_than(float2(0, 5), 1));
		Assert::IsFalse(greater_than(float2(1, 5), 1));
		Assert::IsFalse(greater_than(float2(4, 0), 1));
		Assert::IsFalse(greater_than(float2(4, 1), 1));

		Assert::IsTrue(greater_than(float2(4, 5), 1));
	}

	TEST_METHOD(is_normalized)
	{
		using cg::is_normalized;
		using cg::len;

		Assert::IsTrue(is_normalized(float2::unit_x));
		Assert::IsTrue(is_normalized(float2::unit_y));

		float2 v(1, 2);
		Assert::IsTrue(len(v) > 1 && (!is_normalized(v)));
	}

	TEST_METHOD(len_and_len_squared)
	{
		using cg::approx_equal;
		using cg::len;
		using cg::len_squared;

		float2 u(2, 3);
		float2 v(4, 5);

		Assert::AreEqual(2.f*2 + 3*3, len_squared(u));
		Assert::AreEqual(std::sqrt(2.f*2 + 3*3), len(u));

		Assert::AreEqual(2 * len(u), len(2 * u), L"|aU| = |a| * |U|");
		Assert::IsTrue(approx_equal(len(u + v), len(u) + len(v), 0.1f), L"|U + V| <= |U| + |V|");
	}

	TEST_METHOD(lerp)
	{
		using cg::lerp;

		Assert::AreEqual(float2(0.0f), lerp(float2::zero, float2::unit_xy, 0.0f));
		Assert::AreEqual(float2(0.6f), lerp(float2::zero, float2::unit_xy, 0.6f));
		Assert::AreEqual(float2(1.0f), lerp(float2::zero, float2::unit_xy, 1.0f));

		float2 v(24);
		Assert::AreEqual(v, lerp(v, v, 0.4f));
		Assert::AreEqual(v, lerp(v, v, 0.7f));
	}

	TEST_METHOD(normalize)
	{
		using cg::approx_equal;
		using cg::len;
		using cg::normalize;

		Assert::AreEqual(float2::zero, normalize(float2::zero));
		Assert::AreEqual(float2::unit_x, normalize(float2::unit_x));
		Assert::AreEqual(float2::unit_y, normalize(float2::unit_y));

		Assert::AreEqual(float2::unit_x, normalize(float2(24, 0)));
		Assert::AreEqual(float2::unit_y, normalize(float2(0, 24)));

		float2 u(-8, 6);
		Assert::IsTrue(approx_equal(1.f, len(normalize(u))));
	}

	TEST_METHOD(unary_operators)
	{
		float2 v(1, 2);

		Assert::AreEqual(float2(-1, -2), -v);
		Assert::AreEqual(float2(1, 2), -(-v));
	}
};

} // namespace unittest
