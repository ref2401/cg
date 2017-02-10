#include "cg/base/base.h"

#include <exception>
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace unittest {

TEST_CLASS(cg_base_base_Funcs) {
	
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
			std::string s = concat(123);
			Assert::AreEqual(char_123, s.c_str());
			Assert::IsTrue(char_123 != s.c_str());
		}

		{ // abc!123!abc!123!!
			std::string s = concat(str_abc, ch, char_123, ch, str_abc, ch, char_123, ch, ch);
			Assert::AreEqual("abc!123!abc!123!!", s.c_str());
		}

	}

	TEST_METHOD(ENFORCE_macro)
	{
		ENFORCE(true, "it doesn't throw.");
		Assert::ExpectException<std::runtime_error>([] { ENFORCE(false, "error"); });
	}

	TEST_METHOD(get_exception_message)
	{
		using cg::exception_message;

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
			actual_msg = exception_message(exc);
		}
			
		Assert::AreEqual(expected_msg, actual_msg);
	}

	TEST_METHOD(kilobytes_megabytes)
	{
		using cg::kilobytes;
		using cg::megabytes;

		Assert::AreEqual<size_t>(0, kilobytes(0));
		Assert::AreEqual<size_t>(1024, kilobytes(1));
		Assert::AreEqual<size_t>(128 * 1024, kilobytes(128));

		Assert::AreEqual<size_t>(0, megabytes(0));
		Assert::AreEqual<size_t>(1024 * 1024, megabytes(1));
		Assert::AreEqual<size_t>(128 * 1024 * 1024, megabytes(128));

		Assert::AreEqual<size_t>(kilobytes(1024), megabytes(1));
	}
};

} // namespace unittest
