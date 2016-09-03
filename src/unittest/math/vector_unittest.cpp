#include "cg/math/math.h"

#include <string>
#include <utility>
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace Microsoft { namespace VisualStudio { namespace CppUnitTestFramework {

template<> inline std::wstring ToString<cg::float_3>(const cg::float_3& t) { RETURN_WIDE_STRING(t); }

}}} // namespace Microsoft::VisualStudio::CppUnitTestFramework

namespace unittest {

TEST_CLASS(Vector_unittest)
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
};

} // unittest
