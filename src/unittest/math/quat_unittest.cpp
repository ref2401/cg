#include "cg/math/math.h"

#include "unittest/math/math_unittest_utils.h"

using cg::float3;
using cg::quat;


namespace unittest {

TEST_CLASS(Quat_unittest) {
public:

	TEST_METHOD(static_members)
	{
		Assert::AreEqual(quat(1, 0, 0, 0), quat::i);
		Assert::AreEqual(quat(0, 1, 0, 0), quat::j);
		Assert::AreEqual(quat(0, 0, 1, 0), quat::k);
		Assert::AreEqual(quat(0, 0, 0, 1), quat::identity);
		Assert::AreEqual(quat(0, 0, 0, 0), quat::zero);
	}

	TEST_METHOD(ctors)
	{
		quat q0(1, 2, 3, 4);
		Assert::IsTrue((q0.x == 1) && (q0.y == 2) && (q0.z == 3) && (q0.a == 4));

		quat q1(float3(1, 2, 3), 4);
		Assert::IsTrue((q1.x == 1) && (q1.y == 2) && (q1.z == 3) && (q1.a == 4));
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

		// NOTE(ref2401): Hamilton product is tested in the binary_operators method
	}


	TEST_METHOD(binary_operator)
	{

	}

	TEST_METHOD(equal_operator)
	{
		quat v(1, 2, 3, 4);

		Assert::AreNotEqual(v, quat(100, 2, 3, 4));
		Assert::AreNotEqual(v, quat(1, 100, 3, 4));
		Assert::AreNotEqual(v, quat(1, 2, 100, 4));
		Assert::AreNotEqual(v, quat(1, 2, 3, 100));

		Assert::AreEqual(v, v);
		Assert::AreEqual(v, quat(1, 2, 3, 4));
	}
};

} // namespace unittest