#include "cg/math/utils.h"

#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace unittest {

TEST_CLASS(Utils_unittest) {
public:

	TEST_METHOD(approx_equal)
	{
		using cg::approx_equal;

		Assert::IsTrue(approx_equal(1.f, 1.f));
		Assert::IsTrue(approx_equal(2., 2.));
		Assert::IsTrue(approx_equal(3.l, 3.l));

		// max_abs param
		Assert::IsTrue(approx_equal(1., 1., 0.));
		Assert::IsFalse(approx_equal(1., 1.00001, 0.));

		Assert::IsTrue(approx_equal(1., 1.001, 1.));
		Assert::IsTrue(approx_equal(1., 1.001, 0.1));
		Assert::IsTrue(approx_equal(1., 1.001, 0.01));
		Assert::IsTrue(approx_equal(1., 1.001, 0.001));

		Assert::IsFalse(approx_equal(1., 1.001, 0.0009));
		Assert::IsFalse(approx_equal(1., 1.001, 0.0001));
	}

	TEST_METHOD(clamp)
	{
		using cg::clamp;

		Assert::AreEqual(-1, clamp(-24, -1, 1));
		Assert::AreEqual(-1, clamp(-1, -1, 1));
		Assert::AreEqual(0, clamp(0, -1, 1));
		Assert::AreEqual(1, clamp(1, -1, 1));
		Assert::AreEqual(1, clamp(24, -1, 1));
	}
};

} // unittest