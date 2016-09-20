#include "cg/base/base.h"

#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace unittest {

TEST_CLASS(cg_base_Base) {

	TEST_METHOD(enforce)
	{
		using cg::enforce;

		enforce(true, "it doesn't throw.");
		Assert::ExpectException<std::exception>([] { enforce<std::exception>(false, "error"); });


		std::string msg("error");
		enforce(1, msg);
		Assert::ExpectException<std::runtime_error>([&msg] { enforce<std::runtime_error>(false, msg); });
	}

};

} // namespace unittest
