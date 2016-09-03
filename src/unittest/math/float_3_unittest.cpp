#include "cg/math/math.h"

#include <cmath>
#include <string>
#include <utility>
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace Microsoft { namespace VisualStudio { namespace CppUnitTestFramework {

template<> inline std::wstring ToString<cg::float_3>(const cg::float_3& t) { RETURN_WIDE_STRING(t); }

}}} // namespace Microsoft::VisualStudio::CppUnitTestFramework

namespace unittest {

TEST_CLASS(Float_3_unittest)
{
public:

	TEST_METHOD(static_members)
	{
		Assert::AreEqual(float_3(0, 0, 0), float_3::zero);
		Assert::AreEqual(float_3(1, 0, 0), float_3::unit_x);
		Assert::AreEqual(float_3(0, 1, 0), float_3::unit_y);
		Assert::AreEqual(float_3(0, 0, 1), float_3::unit_z);
		Assert::AreEqual(float_3(1, 1, 1), float_3::unit_xyz);
	}

	TEST_METHOD(ctors)
	{
		float_3 v0;
		Assert::IsTrue((v0.x == 0) && (v0.y == 0) && (v0.z == 0));
		Assert::IsTrue((v0.r == 0) && (v0.g == 0) && (v0.b == 0));
		Assert::IsTrue((v0.width == 0) && (v0.height == 0) && (v0.depth == 0));

		float_3 v1 = float_3(24);
		Assert::IsTrue((v1.x == 24) && (v1.y == 24) && (v1.z == 24));
		Assert::IsTrue((v1.r == 24) && (v1.g == 24) && (v1.b == 24));
		Assert::IsTrue((v1.width == 24) && (v1.height == 24) && (v1.depth == 24));

		float_3 v3 = float_3(1, 2, 3);
		Assert::IsTrue((v3.x == 1) && (v3.y == 2) && (v3.z == 3));
		Assert::IsTrue((v3.r == 1) && (v3.g == 2) && (v3.b == 3));
		Assert::IsTrue((v3.width == 1) && (v3.height == 2) && (v3.depth == 3));

		// copy ctor
		float_3 vc = v3;
		Assert::IsTrue((vc.x == v3.x) && (vc.y == v3.y) && (vc.z == v3.z));
		Assert::IsTrue((vc.r == v3.r) && (vc.g == v3.g) && (vc.b == v3.b));
		Assert::IsTrue((vc.width == v3.width) && (vc.height == v3.height) && (vc.depth == v3.depth));

		// move ctor
		float_3 vm = std::move(v3);
		Assert::IsTrue((vm.x == vc.x) && (vm.y == vc.y) && (vm.z == vc.z));
		Assert::IsTrue((vm.r == vc.r) && (vm.g == vc.g) && (vm.b == vc.b));
		Assert::IsTrue((vm.width == vc.width) && (vm.height == vc.height) && (vm.depth == vc.depth));
	}

	TEST_METHOD(assignments)
	{
		float_3 v = float_3(5, 6, 7);

		// copy assignment
		float_3 vc;
		vc = v;
		Assert::IsTrue((vc.x == v.x) && (vc.y == v.y) && (vc.z == v.z));

		// move assignment
		float_3 vm;
		vm = std::move(v);
		Assert::IsTrue((vm.x == v.x) && (vm.y == v.y) && (vm.z == v.z));
	}

	TEST_METHOD(compound_assignment_operators)
	{
		float_3 v(1, 2, 3);
		(v += 5) += 5;
		Assert::AreEqual(float_3(11, 12, 13), v);

		(v -= 7) -= 3;
		Assert::AreEqual(float_3(1, 2, 3), v);

		(v *= 2) *= 3;
		Assert::AreEqual(float_3(6, 12, 18), v);

		(v /= 3) /= 2;
		Assert::AreEqual(float_3(1, 2, 3), v);

		(v += v) += v;
		Assert::AreEqual(float_3(4, 8, 12), v);

		v -= v;
		Assert::AreEqual(float_3::zero, v);
	}

	TEST_METHOD(unary_operators)
	{
		float_3 v(1, 2, 3);
		
		Assert::AreEqual(float_3(-1, -2, -3), -v);
		Assert::AreEqual(float_3(1, 2, 3), -(-v));
	}

	TEST_METHOD(binary_operators)
	{
		float_3 v(1, 2, 3);

		// operator+
		Assert::AreEqual(float_3(11, 12, 13), v + 10);
		Assert::AreEqual(float_3(11, 12, 13), 10 + v);
		Assert::AreEqual(float_3(1.11f, 2.22f, 3.33f), v + float_3(0.11f, 0.22f, 0.33f));

		// operator-
		Assert::AreEqual(float_3(0, 1, 2), v - 1);
		Assert::AreEqual(float_3(0, -1, -2), 1 - v);
		Assert::AreEqual(float_3::zero, v - float_3(1, 2, 3));

		// operator*
		Assert::AreEqual(float_3(10, 20, 30), v * 10);
		Assert::AreEqual(float_3(10, 20, 30), 10 * v);
		
		// operator/
		Assert::AreEqual(float_3(0.5, 1, 1.5), v / 2);
		Assert::AreEqual(float_3::zero, 0 / v);
	}

	TEST_METHOD(equal_operator)
	{
		float_3 v(1, 2, 3);

		Assert::AreNotEqual(v, float_3(100, 2, 3));
		Assert::AreNotEqual(v, float_3(1, 100, 3));
		Assert::AreNotEqual(v, float_3(1, 2, 100));

		Assert::AreEqual(v, v);
		Assert::AreEqual(v, float_3(1, 2, 3));
	}


	TEST_METHOD(clamp)
	{
		float_3 lo(-5, -7, -9);
		float_3 hi(5, 7, 9);
		float_3 v(2, 3, 4);
		
		Assert::AreEqual(lo, cg::clamp(float_3(-9, -10, -11), lo, hi));
		Assert::AreEqual(v, cg::clamp(v, lo, hi));
		Assert::AreEqual(hi, cg::clamp(float_3(9, 10, 11), lo, hi));

		// default lo and hi
		Assert::AreEqual(float_3::zero, cg::clamp(float_3(-5)));
		Assert::AreEqual(float_3(0.5), cg::clamp(float_3(0.5)));
		Assert::AreEqual(float_3::unit_xyz, cg::clamp(float_3(5)));
	}

	TEST_METHOD(cross_product)
	{
		float_3 u(1, 2, 3);
		float_3 v(4, 5, -6);
		float_3 w(7, -8, 9);

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
		Assert::AreEqual(float_3::zero, cg::cross(u, u));
	}

	TEST_METHOD(dot_product)
	{
		float_3 u(2, 3, 4);
		float_3 v(4, 5, 6);
		float_3 w(7, 8 ,9);

		Assert::AreEqual(2.f*4 + 3*5 + 4*6, cg::dot(u, v));
		Assert::AreEqual(0.f, cg::dot(u, float_3::zero));
		Assert::AreEqual(cg::len_square(u), cg::dot(u, u), L"U * U = |U| * |U|");
		Assert::AreEqual(cg::dot(u, v), cg::dot(v, u), L"U * V = V * U");
		Assert::AreEqual(cg::dot(2*u, v), 2 * cg::dot(u, v), L"(aU) * V = a(U * V)");
		Assert::AreEqual(cg::dot(u + v, w), cg::dot(u, w) + cg::dot(v, w), L"(U + V)*W = U*W + V*W");
	}

	TEST_METHOD(len_and_len_square)
	{
		float_3 u(2, 3, 4);
		float_3 v(4, 5, 6);

		Assert::AreEqual(2.f*2 + 3*3 + 4*4, cg::len_square(u));
		Assert::AreEqual(std::sqrt(2.f*2 + 3*3 + 4*4), cg::len(u));

		Assert::AreEqual(2 * cg::len(u), cg::len(2 * u), L"|aU| = |a| * |U|");
		Assert::IsTrue(cg::approx_equal(cg::len(u + v), cg::len(u) + cg::len(v), 0.1f), L"|U + V| <= |U| + |V|");
	}

	TEST_METHOD(lerp)
	{
		Assert::AreEqual(float_3(0.), cg::lerp(float_3::zero, float_3::unit_xyz, 0.));
		Assert::AreEqual(float_3(0.6f), cg::lerp(float_3::zero, float_3::unit_xyz, 0.6f));
		Assert::AreEqual(float_3(1.), cg::lerp(float_3::zero, float_3::unit_xyz, 1.));

		float_3 v(24);
		Assert::AreEqual(v, cg::lerp(v, v, 0.4f));
		Assert::AreEqual(v, cg::lerp(v, v, 0.7f));

	}

	TEST_METHOD(normalize)
	{
		Assert::AreEqual(float_3::zero, cg::normalize(float_3::zero));
		Assert::AreEqual(float_3::unit_x, cg::normalize(float_3::unit_x));
		Assert::AreEqual(float_3::unit_y, cg::normalize(float_3::unit_y));
		Assert::AreEqual(float_3::unit_z, cg::normalize(float_3::unit_z));

		Assert::AreEqual(float_3::unit_x, cg::normalize(float_3(24, 0, 0)));
		Assert::AreEqual(float_3::unit_y, cg::normalize(float_3(0, 24, 0)));
		Assert::AreEqual(float_3::unit_z, cg::normalize(float_3(0, 0, 24)));

		float_3 u(-8, 6, 24);
		Assert::IsTrue(cg::approx_equal(1.f, cg::len(cg::normalize(u))));
	}

	TEST_METHOD(rgb)
	{
		Assert::AreEqual(float_3::zero, cg::rgb(0));
		Assert::AreEqual(float_3::unit_x, cg::rgb(0xff'00'00));
		Assert::AreEqual(float_3::unit_y, cg::rgb(0x00'ff'00));
		Assert::AreEqual(float_3::unit_z, cg::rgb(0x00'00'ff));
		Assert::AreEqual(float_3::unit_xyz, cg::rgb(0xffffff));

		Assert::AreEqual(float_3(0xa1 / 255.f, 0xb2 / 255.f, 0xe3 / 255.f), cg::rgb(0xa1'b2'e3));
	}
};

} // unittest
