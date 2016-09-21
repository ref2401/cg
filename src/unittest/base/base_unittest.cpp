#include "cg/base/base.h"

#include <exception>
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

	TEST_METHOD(get_exception_message)
	{
		using cg::get_exception_message;

		std::string topmost_msg = "topmost_msg";
		std::string middle_msg = "middle_exception";
		std::string inner_msg = "inner_exception";

		std::string expected_msg;
		expected_msg.append("- "); expected_msg.append(topmost_msg); expected_msg.push_back('\n');
		expected_msg.append("- "); expected_msg.append(middle_msg); expected_msg.push_back('\n');
		expected_msg.append("- "); expected_msg.append(inner_msg); expected_msg.push_back('\n');

		std::string actual_msg;
		try {
			try {
				try {
					throw std::range_error(inner_msg);
				}
				catch (...) {
					std::throw_with_nested(std::invalid_argument(middle_msg));
				}
			}
			catch (...) {
				std::throw_with_nested(std::runtime_error(topmost_msg));
			}
		}
		catch (std::exception& exc) {
			actual_msg = get_exception_message(exc);
		}
			
		Assert::AreEqual(expected_msg, actual_msg);
	}

};

} // namespace unittest
