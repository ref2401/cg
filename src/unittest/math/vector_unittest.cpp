#include "cg/math/math.h"

#include <cmath>
#include <utility>
#include "unittest/math/common_math.h"

using cg::float2;
using cg::float3;
using cg::float4;
using cg::quat;
using cg::ubyte4;
using cg::uint2;
using cg::uint3;
using cg::uint4;


namespace unittest {

TEST_CLASS(cg_math_float2) {
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
		Assert::IsTrue((v0.data[0] == 0) && (v0.data[1] == 0));

		float2 v1(24);
		Assert::IsTrue((v1.x == 24) && (v1.y == 24));
		Assert::IsTrue((v1.u == 24) && (v1.v == 24));
		Assert::IsTrue((v1.data[0] == 24) && (v1.data[1] == 24));

		float2 v2(1, 2);
		Assert::IsTrue((v2.x == 1) && (v2.y == 2));
		Assert::IsTrue((v2.u == 1) && (v2.v == 2));
		Assert::IsTrue((v2.data[0] == 1) && (v2.data[1] == 2));

		// copy ctor
		float2 vc = v2;
		Assert::IsTrue((vc.x == v2.x) && (vc.y == v2.y));
		Assert::IsTrue((vc.u == v2.u) && (vc.v == v2.v));
		Assert::IsTrue((vc.data[0] == v2.data[0]) && (vc.data[1] == v2.data[1]));

		// move ctor
		float2 vm = std::move(v2);
		Assert::IsTrue((vm.x == vc.x) && (vm.y == vc.y));
		Assert::IsTrue((vm.u == vc.u) && (vm.v == vc.v));
		Assert::IsTrue((vm.data[0] == vc.data[0]) && (vm.data[1] == vc.data[1]));
	}

	TEST_METHOD(assignments)
	{
		float2 v(5, 6);

		// copy assignment
		float2 vc;
		vc = v;
		Assert::IsTrue((vc.x == v.x) && (vc.y == v.y));
		Assert::IsTrue((vc.u == v.u) && (vc.v == v.v));
		Assert::IsTrue((vc.data[0] == v.data[0]) && (vc.data[1] == v.data[1]));

		// move assignment
		float2 vm;
		vm = std::move(v);
		Assert::IsTrue((vm.x == v.x) && (vm.y == v.y));
		Assert::IsTrue((vm.u == v.u) && (vm.v == v.v));
		Assert::IsTrue((vm.data[0] == v.data[0]) && (vm.data[1] == v.data[1]));
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

		Assert::AreEqual(lo, clamp(float2(-9, -10), lo, hi));
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

		Assert::AreEqual(2.f * 2 + 3 * 3, len_squared(u));
		Assert::AreEqual(std::sqrt(2.f * 2 + 3 * 3), len(u));

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

TEST_CLASS(cg_math_float3) {
public:

	TEST_METHOD(static_members)
	{
		Assert::AreEqual(float3(0, 0, 0), float3::zero);
		Assert::AreEqual(float3(1, 0, 0), float3::unit_x);
		Assert::AreEqual(float3(0, 1, 0), float3::unit_y);
		Assert::AreEqual(float3(0, 0, 1), float3::unit_z);
		Assert::AreEqual(float3(1, 1, 0), float3::unit_xy);
		Assert::AreEqual(float3(1, 1, 1), float3::unit_xyz);
	}

	TEST_METHOD(ctors)
	{
		float3 v0;
		Assert::IsTrue((v0.x == 0) && (v0.y == 0) && (v0.z == 0));
		Assert::IsTrue((v0.r == 0) && (v0.g == 0) && (v0.b == 0));
		Assert::IsTrue((v0.width == 0) && (v0.height == 0) && (v0.depth == 0));
		Assert::IsTrue((v0.data[0] == 0) && (v0.data[1] == 0) && (v0.data[2] == 0));

		float3 v1(24);
		Assert::IsTrue((v1.x == 24) && (v1.y == 24) && (v1.z == 24));
		Assert::IsTrue((v1.r == 24) && (v1.g == 24) && (v1.b == 24));
		Assert::IsTrue((v1.width == 24) && (v1.height == 24) && (v1.depth == 24));
		Assert::IsTrue((v1.data[0] == 24) && (v1.data[1] == 24) && (v1.data[2] == 24));

		cg::float2 vec2(3, 4);
		float3 v3(vec2, 5);
		Assert::IsTrue((v3.x == vec2.x) && (v3.y == vec2.y) && (v3.z == 5));
		Assert::IsTrue((v3.r == vec2.x) && (v3.g == vec2.y) && (v3.b == 5));
		Assert::IsTrue((v3.width == vec2.x) && (v3.height == vec2.y) && (v3.depth == 5));
		Assert::IsTrue((v3.data[0] == vec2.x) && (v3.data[1] == vec2.y) && (v3.data[2] == 5));

		float3 v4(1, 2, 3);
		Assert::IsTrue((v4.x == 1) && (v4.y == 2) && (v4.z == 3));
		Assert::IsTrue((v4.r == 1) && (v4.g == 2) && (v4.b == 3));
		Assert::IsTrue((v4.width == 1) && (v4.height == 2) && (v4.depth == 3));
		Assert::IsTrue((v4.data[0] == 1) && (v4.data[1] == 2) && (v4.data[2] == 3));

		// copy ctor
		float3 vc = v4;
		Assert::IsTrue((vc.x == v4.x) && (vc.y == v4.y) && (vc.z == v4.z));
		Assert::IsTrue((vc.r == v4.r) && (vc.g == v4.g) && (vc.b == v4.b));
		Assert::IsTrue((vc.width == v4.width) && (vc.height == v4.height) && (vc.depth == v4.depth));
		Assert::IsTrue((vc.data[0] == v4.data[0]) && (vc.data[1] == v4.data[1]) && (vc.data[2] == v4.data[2]));

		// move ctor
		float3 vm = std::move(v4);
		Assert::IsTrue((vm.x == vc.x) && (vm.y == vc.y) && (vm.z == vc.z));
		Assert::IsTrue((vm.r == vc.r) && (vm.g == vc.g) && (vm.b == vc.b));
		Assert::IsTrue((vm.width == vc.width) && (vm.height == vc.height) && (vm.depth == vc.depth));
		Assert::IsTrue((vm.data[0] == vc.data[0]) && (vm.data[1] == vc.data[1]) && (vm.data[2] == vc.data[2]));
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
		Assert::IsTrue((vc.data[0] == v.data[0]) && (vc.data[1] == v.data[1]) && (vc.data[2] == v.data[2]));

		// move assignment
		float3 vm;
		vm = std::move(v);
		Assert::IsTrue((vm.x == v.x) && (vm.y == v.y) && (vm.z == v.z));
		Assert::IsTrue((vm.r == v.r) && (vm.g == v.g) && (vm.b == v.b));
		Assert::IsTrue((vm.width == v.width) && (vm.height == v.height) && (vm.depth == v.depth));
		Assert::IsTrue((vm.data[0] == v.data[0]) && (vm.data[1] == v.data[1]) && (vm.data[2] == v.data[2]));
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
		Assert::AreEqual(0.f, cg::dot(uv, v));
		Assert::AreEqual(float3::zero, cg::cross(u, u));
	}

	TEST_METHOD(dot_product)
	{
		using cg::dot;
		using cg::len_squared;

		float3 u(2, 3, 4);
		float3 v(4, 5, 6);
		float3 w(7, 8, 9);

		Assert::AreEqual(2.f * 4 + 3 * 5 + 4 * 6, dot(u, v));
		Assert::AreEqual(0.f, dot(u, float3::zero));
		Assert::AreEqual(len_squared(u), dot(u, u), L"U * U = |U| * |U|");
		Assert::AreEqual(dot(u, v), dot(v, u), L"U * V = V * U");
		Assert::AreEqual(dot(2 * u, v), 2 * dot(u, v), L"(aU) * V = a(U * V)");
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

		Assert::AreEqual(2.f * 2 + 3 * 3 + 4 * 4, len_squared(u));
		Assert::AreEqual(std::sqrt(2.f * 2 + 3 * 3 + 4 * 4), len(u));

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

TEST_CLASS(cg_math_float4) {
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
		Assert::IsTrue((v0.data[0] == 0) && (v0.data[1] == 0) && (v0.data[2] == 0) && (v0.data[3] == 0));

		float4 v1(24);
		Assert::IsTrue((v1.x == 24) && (v1.y == 24) && (v1.z == 24) && (v1.w == 24));
		Assert::IsTrue((v1.r == 24) && (v1.g == 24) && (v1.b == 24) && (v1.w == 24));
		Assert::IsTrue((v1.data[0] == 24) && (v1.data[1] == 24) && (v1.data[2] == 24) && (v1.data[3] == 24));

		float4 v4(1, 2, 3, 4);
		Assert::IsTrue((v4.x == 1) && (v4.y == 2) && (v4.z == 3) && (v4.w == 4));
		Assert::IsTrue((v4.r == 1) && (v4.g == 2) && (v4.b == 3) && (v4.a == 4));
		Assert::IsTrue((v4.data[0] == 1) && (v4.data[1] == 2) && (v4.data[2] == 3) && (v4.data[3] == 4));

		// copy ctor
		float4 vc = v4;
		Assert::IsTrue((vc.x == v4.x) && (vc.y == v4.y) && (vc.z == v4.z) && (vc.w == v4.w));
		Assert::IsTrue((vc.r == v4.r) && (vc.g == v4.g) && (vc.b == v4.b) && (vc.a == v4.a));
		Assert::IsTrue((vc.data[0] == v4.data[0]) && (vc.data[1] == v4.data[1]) && (vc.data[2] == v4.data[2]) && (vc.data[3] == v4.data[3]));

		// move ctor
		float4 vm = std::move(v4);
		Assert::IsTrue((vm.x == vc.x) && (vm.y == vc.y) && (vm.z == vc.z) && (vm.w == vc.w));
		Assert::IsTrue((vm.r == vc.r) && (vm.g == vc.g) && (vm.b == vc.b) && (vm.a == vc.a));
		Assert::IsTrue((vm.data[0] == vc.data[0]) && (vm.data[1] == vc.data[1]) && (vm.data[2] == vc.data[2]) && (vm.data[3] == vc.data[3]));
	}

	TEST_METHOD(assignments)
	{
		float4 v(5, 6, 7, 8);

		// copy assignment
		float4 vc;
		vc = v;
		Assert::IsTrue((vc.x == v.x) && (vc.y == v.y) && (vc.z == v.z) && (vc.w == v.w));
		Assert::IsTrue((vc.r == v.r) && (vc.g == v.g) && (vc.b == v.b) && (vc.a == v.a));
		Assert::IsTrue((vc.data[0] == v.data[0]) && (vc.data[1] == v.data[1]) 
			&& (vc.data[2] == v.data[2]) && (vc.data[3] == v.data[3]));

		// move assignment
		float4 vm;
		vm = std::move(v);
		Assert::IsTrue((vm.x == v.x) && (vm.y == v.y) && (vm.z == v.z) && (vm.w == v.w));
		Assert::IsTrue((vm.r == v.r) && (vm.g == v.g) && (vm.b == v.b) && (vm.a == v.a));
		Assert::IsTrue((vm.data[0] == v.data[0]) && (vm.data[1] == v.data[1])
			&& (vm.data[2] == v.data[2]) && (vm.data[3] == v.data[3]));
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

	TEST_METHOD(dot_product)
	{
		using cg::dot;
		using cg::len_squared;

		float4 u(2, 3, 4, 5);
		float4 v(5, 6, 7, 8);
		float4 w(9, 10, 11, 12);

		Assert::AreEqual(2.f * 5 + 3 * 6 + 4 * 7 + 5 * 8, dot(u, v));
		Assert::AreEqual(0.f, cg::dot(u, float4::zero));
		Assert::AreEqual(len_squared(u), dot(u, u), L"U * U = |U| * |U|");
		Assert::AreEqual(dot(u, v), dot(v, u), L"U * V = V * U");
		Assert::AreEqual(dot(2 * u, v), 2 * dot(u, v), L"(aU) * V = a(U * V)");
		Assert::AreEqual(dot(u + v, w), dot(u, w) + dot(v, w), L"(U + V)*W = U*W + V*W");
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

	TEST_METHOD(greater_than)
	{
		using cg::greater_than;

		Assert::IsFalse(greater_than(float4(0, 5, 7, 9), 1));
		Assert::IsFalse(greater_than(float4(1, 5, 7, 9), 1));
		Assert::IsFalse(greater_than(float4(4, 0, 7, 9), 1));
		Assert::IsFalse(greater_than(float4(4, 1, 7, 9), 1));
		Assert::IsFalse(greater_than(float4(4, 5, 0, 9), 1));
		Assert::IsFalse(greater_than(float4(4, 5, 1, 9), 1));
		Assert::IsFalse(greater_than(float4(4, 5, 7, 0), 1));
		Assert::IsFalse(greater_than(float4(4, 5, 7, 1), 1));

		Assert::IsTrue(greater_than(float4(4, 5, 7, 9), 1));
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

		Assert::AreEqual(2.f * 2 + 3 * 3 + 4 * 4 + 5 * 5, len_squared(u));
		Assert::AreEqual(std::sqrt(2.f * 2 + 3 * 3 + 4 * 4 + 5 * 5), len(u));

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

TEST_CLASS(cg_math_quat) {
public:

	TEST_METHOD(static_members)
	{
		Assert::AreEqual(quat(1, 0, 0, 0), quat::i);
		Assert::AreEqual(quat(0, 1, 0, 0), quat::j);
		Assert::AreEqual(quat(0, 0, 1, 0), quat::k);
		Assert::AreEqual(quat(0, 0, 0, 1), quat::identity);
		Assert::AreEqual(quat(0, 0, 0, 0), quat::zero);
	}

	TEST_METHOD(assignments)
	{
		quat q(5, 6, 7, 8);

		// copy assignment
		quat qc;
		qc = q;
		Assert::IsTrue((qc.x == q.x) && (qc.y == q.y) && (qc.z == q.z) && (qc.a == q.a));

		// move assignment
		quat qm;
		qm = std::move(q);
		Assert::IsTrue((qm.x == q.x) && (qm.y == q.y) && (qm.z == q.z) && (qm.a == q.a));
	}

	TEST_METHOD(ctors)
	{
		quat q0;
		Assert::IsTrue((q0.x == 0) && (q0.y == 0) && (q0.z == 0) && (q0.a == 0));

		quat q1(1, 2, 3, 4);
		Assert::IsTrue((q1.x == 1) && (q1.y == 2) && (q1.z == 3) && (q1.a == 4));

		quat q2(float3(1, 2, 3), 4);
		Assert::IsTrue((q2.x == 1) && (q2.y == 2) && (q2.z == 3) && (q2.a == 4));

		// copy ctor.
		quat qc = q1;
		Assert::IsTrue((qc.x == q1.x) && (qc.y == q1.y) && (qc.z == q1.z) && (qc.a == q1.a));

		// move ctor.
		quat qm = std::move(qc);
		Assert::IsTrue((qm.x == q1.x) && (qm.y == q1.y) && (qm.z == q1.z) && (qm.a == q1.a));
	}

	TEST_METHOD(compound_assignment_operators)
	{
		quat q(5, 6, 7, 8);
		quat r(1, 2, 3, 4);

		(q += r) += r;
		Assert::AreEqual(quat(7, 10, 13, 16), q);

		(q -= r) -= r;
		Assert::AreEqual(quat(5, 6, 7, 8), q);

		(q *= 2) *= 3;
		Assert::AreEqual(quat(30, 36, 42, 48), q);

		(q /= 3) /= 2;
		Assert::AreEqual(quat(5, 6, 7, 8), q);

		// NOTE(ref2401): Hamilton product compaund assignment operator 
		// is tested in the binary_operators test method.
	}


	TEST_METHOD(binary_operator)
	{
		quat q(1, 2, 3, 4);

		Assert::AreEqual(quat(1.11f, 2.22f, 3.33f, 4.44f), q + quat(0.11f, 0.22f, 0.33f, 0.44f));
		Assert::AreEqual(quat(1.11f, 2.22f, 3.33f, 4.44f), quat(0.11f, 0.22f, 0.33f, 0.44f) + q);

		Assert::AreEqual(quat(0, 1, 2, 3), q - quat(1, 1, 1, 1));
		Assert::AreEqual(quat(0, -1, -2, -3), quat(1, 1, 1, 1) - q);

		Assert::AreEqual(quat(10, 20, 30, 40), q * 10);
		Assert::AreEqual(quat(10, 20, 30, 40), 10 * q);

		Assert::AreEqual(quat(0.5f, 1, 1.5f, 2), q / 2);
		Assert::AreEqual(quat(2, 1, 2.f / 3, 0.5f), 2 / q);

		// Hamilton product
		quat neg_identity(0, 0, 0, -1);
		Assert::AreEqual(neg_identity, quat::i * quat::i);
		Assert::AreEqual(neg_identity, quat::j * quat::j);
		Assert::AreEqual(neg_identity, quat::k * quat::k);
		Assert::AreEqual(neg_identity, (quat::i * quat::j) * quat::k);

		Assert::AreEqual(quat::k, quat::i * quat::j, L"ij = k");
		Assert::AreEqual(quat::i, quat::j * quat::k, L"jk = i");
		Assert::AreEqual(quat::j, quat::k * quat::i, L"ki = j");
		Assert::AreEqual(-quat::k, quat::j * quat::i, L"ji = -k");
		Assert::AreEqual(-quat::i, quat::k * quat::j, L"kj = -i");
		Assert::AreEqual(-quat::j, quat::i * quat::k, L"ik = -j");

		quat p(2, 3, 4, 1);
		quat r(10, 11, 12, 9);

		Assert::AreEqual(p * (q * r), (p * q) * r);
		Assert::AreEqual(p*(5 * q + 7 * r), 5 * (p*q) + 7 * (p*r), L"linearity: p(5q + 7r) = 5pq + 7pr");
		Assert::AreEqual((5 * p + 7 * q)*r, 5 * (p*r) + 7 * (q*r), L"linearity: (5p + 7q)r = 5pr + 7qr");

		Assert::AreEqual(q * 5, q*quat(0, 0, 0, 5));

		// Hamilton product compaund assignment operator
		quat p1 = p;
		Assert::AreEqual(p * q * r, (p1 *= q) *= r);
	}

	TEST_METHOD(conjugate)
	{
		using cg::approx_equal;
		using cg::conjugate;
		using cg::len;

		quat q(2, 3, 4, 1);
		quat r(6, 7, 8, 5);

		Assert::IsTrue(approx_equal(len(q), len(conjugate(q))), L"|q*| = |q|");
		Assert::AreEqual(q, conjugate(conjugate(q)), L"(q*)* = q");
		Assert::AreEqual(conjugate(q + r), conjugate(q) + conjugate(r), L"(q + r)* = q* + r*");
		Assert::AreEqual(conjugate(q * r), conjugate(r) * conjugate(q), L"(qr)* = r*q*");
	}

	TEST_METHOD(equal_operator)
	{
		quat q(1, 2, 3, 4);

		Assert::AreNotEqual(q, quat(100, 2, 3, 4));
		Assert::AreNotEqual(q, quat(1, 100, 3, 4));
		Assert::AreNotEqual(q, quat(1, 2, 100, 4));
		Assert::AreNotEqual(q, quat(1, 2, 3, 100));

		Assert::AreEqual(q, q);
		Assert::AreEqual(q, quat(1, 2, 3, 4));
	}

	TEST_METHOD(inverse)
	{
		using cg::inverse;

		Assert::AreEqual(quat::identity, inverse(quat::identity));

		quat q = quat(2, 3, 4, 5);

		Assert::AreEqual(quat::identity, q * inverse(q));
		Assert::AreEqual(quat::identity, inverse(q) * q);
	}

	TEST_METHOD(is_normalized)
	{
		using cg::is_normalized;
		using cg::len;

		Assert::IsTrue(is_normalized(quat::i));
		Assert::IsTrue(is_normalized(quat::j));
		Assert::IsTrue(is_normalized(quat::k));
		Assert::IsTrue(is_normalized(quat::identity));

		quat q(1, 2, 3, 4);
		Assert::IsTrue(len(q) > 1 && (!is_normalized(q)));
	}

	TEST_METHOD(len_and_len_squared)
	{
		quat q(2, 3, 4, 5);
		quat r(4, 5, 6, 7);

		Assert::AreEqual(2.f * 2 + 3 * 3 + 4 * 4 + 5 * 5, cg::len_squared(q));
		Assert::AreEqual(std::sqrt(2.f * 2 + 3 * 3 + 4 * 4 + 5 * 5), cg::len(q));

		Assert::AreEqual(2 * cg::len(q), cg::len(2 * q), L"|aQ| = |a| * |Q|");

		Assert::IsTrue(
			cg::approx_equal(cg::len(q + r),
			cg::len(q) + cg::len(r), 0.1f),
			L"|Q + P| <= |Q| + |P|");

		Assert::IsTrue(
			cg::approx_equal(cg::len(q * r),
			cg::len(q) * cg::len(r)),
			L"|QP| = |Q| * |P|");
	}

	TEST_METHOD(normalize)
	{
		using cg::approx_equal;
		using cg::len;
		using cg::normalize;

		Assert::AreEqual(quat::zero, normalize(quat::zero));
		Assert::AreEqual(quat::identity, normalize(quat::identity));

		quat q = quat(6, 7, -8, 0);
		Assert::IsTrue(approx_equal(1.f, len(normalize(q))));


	}

	TEST_METHOD(slerp)
	{
		using cg::is_normalized;
		using cg::normalize;
		using cg::slerp;

		quat q = normalize(quat(1, 2, 3, 4));
		quat r = normalize(quat(5, 6, 7, 8));
		Assert::AreEqual(q, slerp(q, r, 0.f));
		Assert::AreEqual(r, slerp(q, r, 1.f));

		quat qs = slerp(q, r, 0.5f);
		Assert::IsTrue(is_normalized(qs));
	}

	TEST_METHOD(unary_operators)
	{
		quat q(1, 2, 3, 4);

		Assert::AreEqual(quat(-1, -2, -3, -4), -q);
		Assert::AreEqual(quat(1, 2, 3, 4), -(-q));
	}
};

TEST_CLASS(cg_math_ubyte4) {
public:

	TEST_METHOD(static_members)
	{
		Assert::AreEqual(ubyte4(0, 0, 0, 0), ubyte4::zero);
		Assert::AreEqual(ubyte4(1, 0, 0, 0), ubyte4::unit_x);
		Assert::AreEqual(ubyte4(0, 1, 0, 0), ubyte4::unit_y);
		Assert::AreEqual(ubyte4(0, 0, 1, 0), ubyte4::unit_z);
		Assert::AreEqual(ubyte4(0, 0, 0, 1), ubyte4::unit_w);
		Assert::AreEqual(ubyte4(1, 1, 1, 1), ubyte4::unit_xyzw);
	}


	TEST_METHOD(ctors)
	{
		ubyte4 v0;
		Assert::IsTrue((v0.x == 0) && (v0.y == 0) && (v0.z == 0) && (v0.w == 0));
		Assert::IsTrue((v0.r == 0) && (v0.g == 0) && (v0.b == 0) && (v0.a == 0));
		Assert::IsTrue((v0.data[0] == 0) && (v0.data[1] == 0) && (v0.data[2] == 0) && (v0.data[3] == 0));

		ubyte4 v1(24);
		Assert::IsTrue((v1.x == 24) && (v1.y == 24) && (v1.z == 24) && (v1.w == 24));
		Assert::IsTrue((v1.r == 24) && (v1.g == 24) && (v1.b == 24) && (v1.w == 24));
		Assert::IsTrue((v1.data[0] == 24) && (v1.data[1] == 24) && (v1.data[2] == 24) && (v1.data[3] == 24));

		ubyte4 v4(1, 2, 3, 4);
		Assert::IsTrue((v4.x == 1) && (v4.y == 2) && (v4.z == 3) && (v4.w == 4));
		Assert::IsTrue((v4.r == 1) && (v4.g == 2) && (v4.b == 3) && (v4.a == 4));
		Assert::IsTrue((v4.data[0] == 1) && (v4.data[1] == 2) && (v4.data[2] == 3) && (v4.data[3] == 4));

		// copy ctor
		ubyte4 vc = v4;
		Assert::IsTrue((vc.x == v4.x) && (vc.y == v4.y) && (vc.z == v4.z) && (vc.w == v4.w));
		Assert::IsTrue((vc.r == v4.r) && (vc.g == v4.g) && (vc.b == v4.b) && (vc.a == v4.a));
		Assert::IsTrue((vc.data[0] == v4.data[0]) && (vc.data[1] == v4.data[1]) && (vc.data[2] == v4.data[2]) && (vc.data[3] == v4.data[3]));

		// move ctor
		ubyte4 vm = std::move(v4);
		Assert::IsTrue((vm.x == vc.x) && (vm.y == vc.y) && (vm.z == vc.z) && (vm.w == vc.w));
		Assert::IsTrue((vm.r == vc.r) && (vm.g == vc.g) && (vm.b == vc.b) && (vm.a == vc.a));
		Assert::IsTrue((vm.data[0] == vc.data[0]) && (vm.data[1] == vc.data[1]) && (vm.data[2] == vc.data[2]) && (vm.data[3] == vc.data[3]));
	}

	TEST_METHOD(assignment_operators)
	{
		ubyte4 v(5, 6, 7, 8);

		// copy assignment
		ubyte4 vc;
		vc = v;
		Assert::IsTrue((vc.x == v.x) && (vc.y == v.y) && (vc.z == v.z) && (vc.w == v.w));
		Assert::IsTrue((vc.r == v.r) && (vc.g == v.g) && (vc.b == v.b) && (vc.a == v.a));
		Assert::IsTrue((vc.data[0] == v.data[0]) && (vc.data[1] == v.data[1])
			&& (vc.data[2] == v.data[2]) && (vc.data[3] == v.data[3]));

		// move assignment
		ubyte4 vm;
		vm = std::move(v);
		Assert::IsTrue((vm.x == v.x) && (vm.y == v.y) && (vm.z == v.z) && (vm.w == v.w));
		Assert::IsTrue((vm.r == v.r) && (vm.g == v.g) && (vm.b == v.b) && (vm.a == v.a));
		Assert::IsTrue((vm.data[0] == v.data[0]) && (vm.data[1] == v.data[1])
			&& (vm.data[2] == v.data[2]) && (vm.data[3] == v.data[3]));
	}

	TEST_METHOD(binary_operators)
	{
		ubyte4 v(5, 6, 7, 8);

		// operator+
		Assert::AreEqual(ubyte4(15, 16, 17, 18), v + 10);
		Assert::AreEqual(ubyte4(15, 16, 17, 18), 10 + v);
		Assert::AreEqual(ubyte4(15, 26, 37, 48), v + ubyte4(10, 20, 30, 40));
		Assert::AreEqual(ubyte4(15, 26, 37, 48), ubyte4(10, 20, 30, 40) + v);

		// operator-
		Assert::AreEqual(ubyte4(4, 5, 6, 7), v - 1);
		Assert::AreEqual(ubyte4(5, 4, 3, 2), 10 - v);
		Assert::AreEqual(ubyte4::zero, v - ubyte4(5, 6, 7, 8));
		Assert::AreEqual(ubyte4::zero, ubyte4(5, 6, 7, 8) - v);

		// operator*
		Assert::AreEqual(ubyte4(50, 60, 70, 80), v * 10);
		Assert::AreEqual(ubyte4(50, 60, 70, 80), 10 * v);

		// operator/
		Assert::AreEqual(ubyte4(2, 3, 3, 4), v / 2);
		Assert::AreEqual(ubyte4::zero, 0 / v);
	}

	TEST_METHOD(compound_assignment_operators)
	{
		ubyte4 v(1, 2, 3, 4);
		(v += 5) += 5;
		Assert::AreEqual(ubyte4(11, 12, 13, 14), v);

		(v -= 7) -= 3;
		Assert::AreEqual(ubyte4(1, 2, 3, 4), v);

		(v *= 2) *= 3;
		Assert::AreEqual(ubyte4(6, 12, 18, 24), v);

		(v /= 3) /= 2;
		Assert::AreEqual(ubyte4(1, 2, 3, 4), v);

		(v += v) += v;
		Assert::AreEqual(ubyte4(4, 8, 12, 16), v);

		v -= v;
		Assert::AreEqual(ubyte4::zero, v);
	}

	TEST_METHOD(equal_operator)
	{
		ubyte4 v(1, 2, 3, 4);

		Assert::AreNotEqual(v, ubyte4(100, 2, 3, 4));
		Assert::AreNotEqual(v, ubyte4(1, 100, 3, 4));
		Assert::AreNotEqual(v, ubyte4(1, 2, 100, 4));
		Assert::AreNotEqual(v, ubyte4(1, 2, 3, 100));

		Assert::AreEqual(v, v);
		Assert::AreEqual(v, ubyte4(1, 2, 3, 4));
	}

	TEST_METHOD(greater_than)
	{
		using cg::greater_than;

		Assert::IsFalse(greater_than(ubyte4(0, 5, 7, 9), 1));
		Assert::IsFalse(greater_than(ubyte4(1, 5, 7, 9), 1));
		Assert::IsFalse(greater_than(ubyte4(4, 0, 7, 9), 1));
		Assert::IsFalse(greater_than(ubyte4(4, 1, 7, 9), 1));
		Assert::IsFalse(greater_than(ubyte4(4, 5, 0, 9), 1));
		Assert::IsFalse(greater_than(ubyte4(4, 5, 1, 9), 1));
		Assert::IsFalse(greater_than(ubyte4(4, 5, 7, 0), 1));
		Assert::IsFalse(greater_than(ubyte4(4, 5, 7, 1), 1));

		Assert::IsTrue(greater_than(ubyte4(4, 5, 7, 9), 1));
	}
};

TEST_CLASS(cg_math_uint2) {
public:

	TEST_METHOD(static_members)
	{
		Assert::AreEqual(uint2(0, 0), uint2::zero);
		Assert::AreEqual(uint2(1, 0), uint2::unit_x);
		Assert::AreEqual(uint2(0, 1), uint2::unit_y);
		Assert::AreEqual(uint2(1, 1), uint2::unit_xy);
	}

	TEST_METHOD(ctors)
	{
		uint2 v0;
		Assert::IsTrue((v0.x == 0) && (v0.y == 0));
		Assert::IsTrue((v0.width == 0) && (v0.height == 0));
		Assert::IsTrue((v0.data[0] == 0) && (v0.data[1] == 0));

		uint2 v1(24);
		Assert::IsTrue((v1.x == 24) && (v1.y == 24));
		Assert::IsTrue((v1.width == 24) && (v1.height == 24));
		Assert::IsTrue((v1.data[0] == 24) && (v1.data[1] == 24));

		uint2 v2(1, 2);
		Assert::IsTrue((v2.x == 1) && (v2.y == 2));
		Assert::IsTrue((v2.width == 1) && (v2.height == 2));
		Assert::IsTrue((v2.data[0] == 1) && (v2.data[1] == 2));

		// copy ctor
		uint2 vc = v2;
		Assert::IsTrue((vc.x == v2.x) && (vc.y == v2.y));
		Assert::IsTrue((vc.width == v2.width) && (vc.height == v2.height));
		Assert::IsTrue((vc.data[0] == v2.data[0]) && (vc.data[1] == v2.data[1]));

		// move ctor
		uint2 vm = std::move(v2);
		Assert::IsTrue((vm.x == vc.x) && (vm.y == vc.y));
		Assert::IsTrue((vm.width == vc.width) && (vm.height == vc.height));
		Assert::IsTrue((vm.data[0] == vc.data[0]) && (vm.data[1] == vc.data[1]));
	}

	TEST_METHOD(compound_assignment_operators)
	{
		uint2 v(1, 2);
		(v += 5) += 5;
		Assert::AreEqual(uint2(11, 12), v);

		(v -= 7) -= 3;
		Assert::AreEqual(uint2(1, 2), v);

		(v *= 2) *= 3;
		Assert::AreEqual(uint2(6, 12), v);

		(v /= 3) /= 2;
		Assert::AreEqual(uint2(1, 2), v);

		(v += v) += v;
		Assert::AreEqual(uint2(4, 8), v);

		v -= v;
		Assert::AreEqual(uint2::zero, v);
	}


	TEST_METHOD(aspect_ration)
	{
		uint2 r(3, 2);
		Assert::AreEqual(1, r.aspect_ratio<int>());
		Assert::AreEqual(1.5f, r.aspect_ratio());
		Assert::AreEqual(1.5, r.aspect_ratio<double>());
	}

	TEST_METHOD(binary_operators)
	{
		uint2 v(1, 2);

		// operator+
		Assert::AreEqual(uint2(11, 12), v + 10);
		Assert::AreEqual(uint2(11, 12), 10 + v);
		Assert::AreEqual(uint2(11, 22), v + uint2(10, 20));
		Assert::AreEqual(uint2(11, 22), uint2(10, 20) + v);

		// operator-
		Assert::AreEqual(uint2(0, 1), v - 1);
		Assert::AreEqual(uint2(4, 3), 5 - v);
		Assert::AreEqual(uint2::zero, v - uint2(1, 2));
		Assert::AreEqual(uint2::zero, uint2(1, 2) - v);

		// operator*s
		Assert::AreEqual(uint2(10, 20), v * 10);
		Assert::AreEqual(uint2(10, 20), 10 * v);

		// operator/
		Assert::AreEqual(uint2(2, 1), uint2(4, 2) / 2);
		Assert::AreEqual(uint2::zero, 0 / v);
	}

	TEST_METHOD(equal_operator)
	{
		uint2 v(1, 2);

		Assert::AreNotEqual(v, uint2(100, 2));
		Assert::AreNotEqual(v, uint2(1, 100));

		Assert::AreEqual(v, v);
		Assert::AreEqual(v, uint2(1, 2));
	}

	TEST_METHOD(greater_than)
	{
		using cg::greater_than;

		Assert::IsFalse(greater_than(uint2(0, 5), 1));
		Assert::IsFalse(greater_than(uint2(1, 5), 1));
		Assert::IsFalse(greater_than(uint2(4, 0), 1));
		Assert::IsFalse(greater_than(uint2(4, 1), 1));

		Assert::IsTrue(greater_than(uint2(4, 5), 1));
	}

	TEST_METHOD(square)
	{
		Assert::AreEqual(0u, uint2::zero.square());

		uint2 r(4, 5);
		Assert::AreEqual(20, r.square<int>());
		Assert::AreEqual(20.f, r.square<float>());
		Assert::AreEqual(20., r.square<double>());
	}

};

TEST_CLASS(cg_math_uint3) {
public:

	TEST_METHOD(static_members)
	{
		Assert::AreEqual(uint3(1, 0, 0), uint3::unit_x);
		Assert::AreEqual(uint3(0, 1, 0), uint3::unit_y);
		Assert::AreEqual(uint3(0, 0, 1), uint3::unit_z);
		Assert::AreEqual(uint3(1, 1, 1), uint3::unit_xyz);
		Assert::AreEqual(uint3(0, 0, 0), uint3::zero);
	}

	TEST_METHOD(assignment_operators)
	{
		uint3 v(5, 6, 7);

		// copy assignments
		uint3 vc;
		vc = v;
		Assert::IsTrue((vc.x == v.x) && (vc.y == v.y) && (vc.z == v.z));
		Assert::IsTrue((vc.r == v.r) && (vc.g == v.g) && (vc.b == v.b));
		Assert::IsTrue((vc.width == v.width) && (vc.height == v.height) && (vc.depth == v.depth));
		Assert::IsTrue((vc.data[0] == v.data[0]) && (vc.data[1] == v.data[1]) && (vc.data[2] == v.data[2]));

		// move assignment
		uint3 vm;
		vm = std::move(v);
		Assert::IsTrue((vm.x == v.x) && (vm.y == v.y) && (vm.z == v.z));
		Assert::IsTrue((vm.r == v.r) && (vm.g == v.g) && (vm.b == v.b));
		Assert::IsTrue((vm.width == v.width) && (vm.height == v.height) && (vm.depth == v.depth));
		Assert::IsTrue((vm.data[0] == v.data[0]) && (vm.data[1] == v.data[1]) && (vm.data[2] == v.data[2]));
	}

	TEST_METHOD(binary_operators)
	{
		uint3 v(5, 6, 7);

		// operator+
		Assert::AreEqual(uint3(15, 16, 17), v + 10);
		Assert::AreEqual(uint3(15, 16, 17), 10 + v);
		Assert::AreEqual(uint3(15, 26, 37), v + uint3(10, 20, 30));
		Assert::AreEqual(uint3(15, 26, 37), uint3(10, 20, 30) + v);

		// operator-
		Assert::AreEqual(uint3(4, 5, 6), v - 1);
		Assert::AreEqual(uint3(5, 4, 3), 10 - v);
		Assert::AreEqual(uint3::zero, v - uint3(5, 6, 7));
		Assert::AreEqual(uint3::zero, uint3(5, 6, 7) - v);

		// operator*
		Assert::AreEqual(uint3(50, 60, 70), v * 10);
		Assert::AreEqual(uint3(50, 60, 70), 10 * v);

		// operator/
		Assert::AreEqual(uint3(2, 3, 3), v / 2);
		Assert::AreEqual(uint3::zero, 0 / v);
	}

	TEST_METHOD(ctors)
	{
		uint3 v0;
		Assert::IsTrue((v0.x == 0) && (v0.y == 0) && (v0.z == 0));
		Assert::IsTrue((v0.r == 0) && (v0.g == 0) && (v0.b == 0));
		Assert::IsTrue((v0.width == 0) && (v0.height == 0) && (v0.depth == 0));
		Assert::IsTrue((v0.data[0] == 0) && (v0.data[1] == 0) && (v0.data[2] == 0));

		uint3 v1(24);
		Assert::IsTrue((v1.x == 24) && (v1.y == 24) && (v1.z == 24));
		Assert::IsTrue((v1.r == 24) && (v1.g == 24) && (v1.b == 24));
		Assert::IsTrue((v1.width == 24) && (v1.height == 24) && (v1.depth == 24));
		Assert::IsTrue((v1.data[0] == 24) && (v1.data[1] == 24) && (v1.data[2] == 24));

		uint2 v2_origin(7, 8);
		uint3 v2(v2_origin, 9);
		Assert::IsTrue((v2.x == 7) && (v2.y == 8) && (v2.z == 9));
		Assert::IsTrue((v2.r == 7) && (v2.g == 8) && (v2.b == 9));
		Assert::IsTrue((v2.width == 7) && (v2.height == 8) && (v2.depth == 9));
		Assert::IsTrue((v2.data[0] == 7) && (v2.data[1] == 8) && (v2.data[2] == 9));

		uint3 v4(1, 2, 3);
		Assert::IsTrue((v4.x == 1) && (v4.y == 2) && (v4.z == 3));
		Assert::IsTrue((v4.r == 1) && (v4.g == 2) && (v4.b == 3));
		Assert::IsTrue((v4.width == 1) && (v4.height == 2) && (v4.depth == 3));
		Assert::IsTrue((v4.data[0] == 1) && (v4.data[1] == 2) && (v4.data[2] == 3));

		// copy ctor
		uint3 vc = v4;
		Assert::IsTrue((vc.x == v4.x) && (vc.y == v4.y) && (vc.z == v4.z));
		Assert::IsTrue((vc.r == v4.r) && (vc.g == v4.g) && (vc.b == v4.b));
		Assert::IsTrue((vc.width == v4.width) && (vc.height == v4.height) && (vc.depth == v4.depth));
		Assert::IsTrue((vc.data[0] == v4.data[0]) && (vc.data[1] == v4.data[1]) && (vc.data[2] == v4.data[2]));

		// move ctor
		uint3 vm = std::move(vc);
		Assert::IsTrue((vm.x == v4.x) && (vm.y == v4.y) && (vm.z == v4.z));
		Assert::IsTrue((vm.r == v4.r) && (vm.g == v4.g) && (vm.b == v4.b));
		Assert::IsTrue((vm.width == v4.width) && (vm.height == v4.height) && (vm.depth == v4.depth));
		Assert::IsTrue((vm.data[0] == v4.data[0]) && (vm.data[1] == v4.data[1]) && (vm.data[2] == v4.data[2]));
	}

	TEST_METHOD(compound_assignment_operators)
	{
		uint3 v(1, 2, 3);

		(v += 5) += 5;
		Assert::AreEqual(uint3(11, 12, 13), v);

		(v -= 7) -= 3;
		Assert::AreEqual(uint3(1, 2, 3), v);

		(v *= 2) *= 3;
		Assert::AreEqual(uint3(6, 12, 18), v);

		(v /= 3) /= 2;
		Assert::AreEqual(uint3(1, 2, 3), v);

		(v += v) += v;
		Assert::AreEqual(uint3(4, 8, 12), v);

		v -= v;
		Assert::AreEqual(uint3::zero, v);
	}

	TEST_METHOD(equal_operator)
	{
		uint3 v(1, 2, 3);

		Assert::AreNotEqual(v, uint3(100, 2, 3));
		Assert::AreNotEqual(v, uint3(1, 100, 3));
		Assert::AreNotEqual(v, uint3(1, 2, 100));

		Assert::AreEqual(v, v);
		Assert::AreEqual(v, uint3(1, 2, 3));
	}

	TEST_METHOD(greater_than)
	{
		using cg::greater_than;

		Assert::IsFalse(greater_than(uint3(0, 5, 7), 1));
		Assert::IsFalse(greater_than(uint3(1, 5, 7), 1));
		Assert::IsFalse(greater_than(uint3(4, 0, 7), 1));
		Assert::IsFalse(greater_than(uint3(4, 1, 7), 1));
		Assert::IsFalse(greater_than(uint3(4, 5, 0), 1));
		Assert::IsFalse(greater_than(uint3(4, 5, 1), 1));

		Assert::IsTrue(greater_than(uint3(4, 5, 7), 1));
	}
};

TEST_CLASS(cg_math_uint4) {
public:

	TEST_METHOD(static_members)
	{
		Assert::AreEqual(uint4(1, 0, 0, 0), uint4::unit_x);
		Assert::AreEqual(uint4(0, 1, 0, 0), uint4::unit_y);
		Assert::AreEqual(uint4(0, 0, 1, 0), uint4::unit_z);
		Assert::AreEqual(uint4(0, 0, 0, 1), uint4::unit_w);
		Assert::AreEqual(uint4(1, 1, 1, 1), uint4::unit_xyzw);
		Assert::AreEqual(uint4(0, 0, 0, 0), uint4::zero);
	}

	TEST_METHOD(assignment_operators)
	{
		uint4 v(5, 6, 7, 8);

		// copy assignment
		uint4 vc;
		vc = v;
		Assert::IsTrue((vc.x == v.x) && (vc.y == v.y) && (vc.z == v.z) && (vc.w == v.w));
		Assert::IsTrue((vc.r == v.r) && (vc.g == v.g) && (vc.b == v.b) && (vc.a == v.a));
		Assert::IsTrue((vc.data[0] == v.data[0]) && (vc.data[1] == v.data[1])
			&& (vc.data[2] == v.data[2]) && (vc.data[3] == v.data[3]));

		// move assignment
		uint4 vm;
		vm = std::move(v);
		Assert::IsTrue((vm.x == v.x) && (vm.y == v.y) && (vm.z == v.z) && (vm.w == v.w));
		Assert::IsTrue((vm.r == v.r) && (vm.g == v.g) && (vm.b == v.b) && (vm.a == v.a));
		Assert::IsTrue((vm.data[0] == v.data[0]) && (vm.data[1] == v.data[1])
			&& (vm.data[2] == v.data[2]) && (vm.data[3] == v.data[3]));
	}

	TEST_METHOD(binary_operators)
	{
		uint4 v(5, 6, 7, 8);

		// operator+
		Assert::AreEqual(uint4(15, 16, 17, 18), v + 10);
		Assert::AreEqual(uint4(15, 16, 17, 18), 10 + v);
		Assert::AreEqual(uint4(15, 26, 37, 48), v + uint4(10, 20, 30, 40));
		Assert::AreEqual(uint4(15, 26, 37, 48), uint4(10, 20, 30, 40) + v);

		// operator-
		Assert::AreEqual(uint4(4, 5, 6, 7), v - 1);
		Assert::AreEqual(uint4(5, 4, 3, 2), 10 - v);
		Assert::AreEqual(uint4::zero, v - uint4(5, 6, 7, 8));
		Assert::AreEqual(uint4::zero, uint4(5, 6, 7, 8) - v);

		// operator*
		Assert::AreEqual(uint4(50, 60, 70, 80), v * 10);
		Assert::AreEqual(uint4(50, 60, 70, 80), 10 * v);

		// operator/
		Assert::AreEqual(uint4(2, 3, 3, 4), v / 2);
		Assert::AreEqual(uint4::zero, 0 / v);
	}

	TEST_METHOD(ctors)
	{
		uint4 v0;
		Assert::IsTrue((v0.x == 0) && (v0.y == 0) && (v0.z == 0) && (v0.w == 0));
		Assert::IsTrue((v0.r == 0) && (v0.g == 0) && (v0.b == 0) && (v0.a == 0));
		Assert::IsTrue((v0.data[0] == 0) && (v0.data[1] == 0) && (v0.data[2] == 0) && (v0.data[3] == 0));

		uint4 v1(24);
		Assert::IsTrue((v1.x == 24) && (v1.y == 24) && (v1.z == 24) && (v1.w == 24));
		Assert::IsTrue((v1.r == 24) && (v1.g == 24) && (v1.b == 24) && (v1.w == 24));
		Assert::IsTrue((v1.data[0] == 24) && (v1.data[1] == 24) && (v1.data[2] == 24) && (v1.data[3] == 24));

		uint4 v4(1, 2, 3, 4);
		Assert::IsTrue((v4.x == 1) && (v4.y == 2) && (v4.z == 3) && (v4.w == 4));
		Assert::IsTrue((v4.r == 1) && (v4.g == 2) && (v4.b == 3) && (v4.a == 4));
		Assert::IsTrue((v4.data[0] == 1) && (v4.data[1] == 2) && (v4.data[2] == 3) && (v4.data[3] == 4));

		// copy ctor
		uint4 vc = v4;
		Assert::IsTrue((vc.x == v4.x) && (vc.y == v4.y) && (vc.z == v4.z) && (vc.w == v4.w));
		Assert::IsTrue((vc.r == v4.r) && (vc.g == v4.g) && (vc.b == v4.b) && (vc.a == v4.a));
		Assert::IsTrue((vc.data[0] == v4.data[0]) && (vc.data[1] == v4.data[1]) && (vc.data[2] == v4.data[2]) && (vc.data[3] == v4.data[3]));

		// move ctor
		uint4 vm = std::move(v4);
		Assert::IsTrue((vm.x == vc.x) && (vm.y == vc.y) && (vm.z == vc.z) && (vm.w == vc.w));
		Assert::IsTrue((vm.r == vc.r) && (vm.g == vc.g) && (vm.b == vc.b) && (vm.a == vc.a));
		Assert::IsTrue((vm.data[0] == vc.data[0]) && (vm.data[1] == vc.data[1]) && (vm.data[2] == vc.data[2]) && (vm.data[3] == vc.data[3]));
	}

	TEST_METHOD(compound_assignment_operators)
	{
		uint4 v(1, 2, 3, 4);
		(v += 5) += 5;
		Assert::AreEqual(uint4(11, 12, 13, 14), v);

		(v -= 7) -= 3;
		Assert::AreEqual(uint4(1, 2, 3, 4), v);

		(v *= 2) *= 3;
		Assert::AreEqual(uint4(6, 12, 18, 24), v);

		(v /= 3) /= 2;
		Assert::AreEqual(uint4(1, 2, 3, 4), v);

		(v += v) += v;
		Assert::AreEqual(uint4(4, 8, 12, 16), v);

		v -= v;
		Assert::AreEqual(uint4::zero, v);
	}

	TEST_METHOD(equal_operator)
	{
		uint4 v(1, 2, 3, 4);

		Assert::AreNotEqual(v, uint4(100, 2, 3, 4));
		Assert::AreNotEqual(v, uint4(1, 100, 3, 4));
		Assert::AreNotEqual(v, uint4(1, 2, 100, 4));
		Assert::AreNotEqual(v, uint4(1, 2, 3, 100));

		Assert::AreEqual(v, v);
		Assert::AreEqual(v, uint4(1, 2, 3, 4));
	}

	TEST_METHOD(greater_than)
	{
		using cg::greater_than;

		Assert::IsFalse(greater_than(uint4(0, 5, 7, 9), 1));
		Assert::IsFalse(greater_than(uint4(1, 5, 7, 9), 1));
		Assert::IsFalse(greater_than(uint4(4, 0, 7, 9), 1));
		Assert::IsFalse(greater_than(uint4(4, 1, 7, 9), 1));
		Assert::IsFalse(greater_than(uint4(4, 5, 0, 9), 1));
		Assert::IsFalse(greater_than(uint4(4, 5, 1, 9), 1));
		Assert::IsFalse(greater_than(uint4(4, 5, 7, 0), 1));
		Assert::IsFalse(greater_than(uint4(4, 5, 7, 1), 1));

		Assert::IsTrue(greater_than(uint4(4, 5, 7, 9), 1));
	}
};

} // namespace unittest