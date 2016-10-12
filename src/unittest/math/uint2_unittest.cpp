#include "cg/math/math.h"

#include <utility>
#include "unittest/math/common_math.h"

using cg::uint2;


namespace unittest {

TEST_CLASS(cg_math_Uint2) {
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

		uint2 v1(24);
		Assert::IsTrue((v1.x == 24) && (v1.y == 24));
		Assert::IsTrue((v1.width == 24) && (v1.height == 24));

		uint2 v2(1, 2);
		Assert::IsTrue((v2.x == 1) && (v2.y == 2));
		Assert::IsTrue((v2.width == 1) && (v2.height == 2));

		// copy ctor
		uint2 vc = v2;
		Assert::IsTrue((vc.x == v2.x) && (vc.y == v2.y));
		Assert::IsTrue((vc.width == v2.width) && (vc.height == v2.height));

		// move ctor
		uint2 vm = std::move(v2);
		Assert::IsTrue((vm.x == vc.x) && (vm.y == vc.y));
		Assert::IsTrue((vm.width == vc.width) && (vm.height == vc.height));
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

} // namespace unittest