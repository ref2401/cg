#include "cg/math/utils.h"

#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace unittest {

TEST_CLASS(Utils_unittest) {
public:

	TEST_METHOD(approx_equal)
	{
		Assert::IsTrue(cg::approx_equal(1.f, 1.f));
		Assert::IsTrue(cg::approx_equal(2., 2.));
		Assert::IsTrue(cg::approx_equal(3.l, 3.l));

		// max_abs param
		Assert::IsTrue(cg::approx_equal(1., 1., 0.));
		Assert::IsFalse(cg::approx_equal(1., 1.00001, 0.));

		Assert::IsTrue(cg::approx_equal(1., 1.001, 1.));
		Assert::IsTrue(cg::approx_equal(1., 1.001, 0.1));
		Assert::IsTrue(cg::approx_equal(1., 1.001, 0.01));
		Assert::IsTrue(cg::approx_equal(1., 1.001, 0.001));

		Assert::IsFalse(cg::approx_equal(1., 1.001, 0.0009));
		Assert::IsFalse(cg::approx_equal(1., 1.001, 0.0001));
	}
};

} // unittest