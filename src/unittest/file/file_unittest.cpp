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
		File fe;
		Assert::IsTrue(fe.filename().empty());
		Assert::IsFalse(fe.is_open());

		Assert::ExpectException<File_exception>([] { File f("unknown-file"); });

		File f(Filenames::empty_file);
		Assert::AreEqual(Filenames::empty_file, f.filename());
		Assert::IsTrue(f.is_open());

		File fm = std::move(f);
		Assert::AreEqual(Filenames::empty_file, fm.filename());
		Assert::IsTrue(fm.is_open());
		// f is empty
		Assert::IsTrue(f.filename().empty());
		Assert::IsFalse(f.is_open());
	}

	TEST_METHOD(move_assignment)
	{
		File f0(Filenames::empty_file);
		File f1(Filenames::ascii_single_line);

		f1 = std::move(f0);
		Assert::AreEqual(Filenames::empty_file, f1.filename());
		Assert::IsTrue(f1.is_open());
		// f is empty
		Assert::IsTrue(f0.filename().empty());
		Assert::IsFalse(f0.is_open());
	}

	TEST_METHOD(open_close)
	{
		File f;
		f.close(); // does not throw

		Assert::ExpectException<File_exception>([&] { f.open("unknown-file"); });
		Assert::IsTrue(f.filename().empty());
		Assert::IsFalse(f.is_open());

		f.open(Filenames::empty_file);
		Assert::IsFalse(f.eof());
		Assert::AreEqual(Filenames::empty_file, f.filename());
		Assert::IsTrue(f.is_open());

		f.close();
		Assert::IsTrue(f.filename().empty());
		Assert::IsFalse(f.is_open());
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
			File f(Filenames::ascii_single_line);
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

		{ // attempt to read from moved File object
			File f0(Filenames::ascii_single_line);
			Assert::IsFalse(f0.eof());

			File f = std::move(f0);
			Assert::ExpectException<File_exception>([&]
			{
				char ch;
				f0.read_byte(&ch);
			});
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
			File f(Filenames::ascii_single_line);
			Assert::IsFalse(f.eof());

			unsigned char buffer[8];
			size_t count = f.read_bytes(buffer, std::extent<decltype(buffer)>::value);
			buffer[count] = '\0';
			Assert::AreEqual(6u, count);
			Assert::IsTrue(f.eof());
			Assert::AreEqual("abc123", reinterpret_cast<char*>(buffer));
		}

		{ // attempt to read from moved File object
			File f0(Filenames::ascii_single_line);
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
