#include "cg/file/file.h"

#include <type_traits>
#include <utility>
#include "unittest/file/file_unittest_common.h"

using cg::file::File;
using cg::file::File_exception;


namespace unittest {

TEST_CLASS(File_unittest) {

	TEST_METHOD(ctor_open_file)
	{
		Assert::ExpectException<File_exception>([] { File f("unknown-file"); });

		File f(Filenames::empty_file);
		Assert::AreEqual(Filenames::empty_file, f.filename());
		Assert::IsNotNull(f.handle());

		auto expected_handle = f.handle();
		File fm = std::move(f);
		Assert::IsTrue(f.filename().empty());
		Assert::IsNull(f.handle());

		Assert::AreEqual(Filenames::empty_file, fm.filename());
		Assert::IsTrue(expected_handle == fm.handle());
	}

	TEST_METHOD(move_assignment)
	{
		File f0(Filenames::empty_file);
		File f1(Filenames::single_ascii_line_file);
		Assert::IsTrue(f0.handle() != f1.handle());

		auto expected_handle = f0.handle();
		f1 = std::move(f0);
		Assert::IsTrue(f0.filename().empty());
		Assert::IsNull(f0.handle());

		Assert::AreEqual(Filenames::empty_file, f1.filename());
		Assert::IsTrue(expected_handle == f1.handle());
	}

	TEST_METHOD(read_byte_and_eof)
	{
		{ // empty file
			File f(Filenames::empty_file);
			Assert::IsFalse(f.eof());

			char ch;
			bool res = f.read_byte(&ch);
			Assert::IsFalse(res);
			Assert::IsTrue(f.eof());
		}

		{ // file content: abc123
			File f(Filenames::single_ascii_line_file);
			Assert::IsFalse(f.eof());

			char ch;
			const char* expected_chars = "abc123";
			for (size_t i = 0; i < 6; ++i) {
				bool res = f.read_byte(&ch);
				Assert::IsTrue(res);
				Assert::AreEqual(expected_chars[i], ch);
			}

			bool res = f.read_byte(&ch);
			Assert::IsFalse(res);
			Assert::IsTrue(f.eof());
		}
	}

	TEST_METHOD(read_bytes_and_eof)
	{
		{ // empty file
			File f(Filenames::empty_file);
			Assert::IsFalse(f.eof());

			char ch;
			size_t count = f.read_bytes(&ch, 1);
			Assert::AreEqual(0u, count);
			Assert::IsTrue(f.eof());
		}

		
		{ // file content: abc123
			File f(Filenames::single_ascii_line_file);
			Assert::IsFalse(f.eof());

			unsigned char buffer[8];
			size_t count = f.read_bytes(buffer, std::extent<decltype(buffer)>::value);
			buffer[count] = '\0';
			Assert::AreEqual(6u, count);
			Assert::IsTrue(f.eof());
			Assert::AreEqual("abc123", reinterpret_cast<char*>(buffer));
		}

		{ // attempt to read from moved File object
			File f0(Filenames::single_ascii_line_file);
			Assert::IsFalse(f0.eof());

			File f = std::move(f0);
			Assert::ExpectException<File_exception>([&]
			{ 
				char ch;
				f0.read_bytes(&ch, 1);  
			});
		}
	}

};

} // namespace unittest
