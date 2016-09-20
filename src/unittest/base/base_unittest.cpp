#include "cg/base/base.h"

#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace unittest {

TEST_CLASS(cg_base_Base) {

	TEST_METHOD(concat)
	{
		using cg::concat;

		std::string s0 = concat();
		Assert::IsTrue(s0.empty());

		char ch = '!';
		std::string str_abc = "abc";
		const char* char_123 = "123";

		{ // !
			std::string s = concat(ch);
			Assert::IsTrue(s.size() == 1);
			Assert::AreEqual(ch, s[0]);
		}

		{ // abc
			std::string s = concat(str_abc);
			Assert::AreEqual(str_abc, s);
			Assert::IsTrue(str_abc.c_str() != s.c_str());
		}

		{ // 123
			std::string s = concat(char_123);
			Assert::AreEqual(char_123, s.c_str());
			Assert::IsTrue(char_123 != s.c_str());
		}

		{ // abc!123!abc!123!!
			std::string s = concat(str_abc, ch, char_123, ch, str_abc, ch, char_123, ch, ch);
			Assert::AreEqual("abc!123!abc!123!!", s.c_str());
		}

	}

	TEST_METHOD(enforce)
	{
		using cg::enforce;

		enforce(true, "it doesn't throw.");
		
		Assert::ExpectException<std::runtime_error>([] { enforce(false, "error"); });
		Assert::ExpectException<std::runtime_error>([] { enforce<std::runtime_error>(false, "error"); });
		Assert::ExpectException<std::range_error>([] { enforce<std::range_error>(false, "error"); });
	}

};

} // namespace unittest
