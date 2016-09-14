#include "cg/file/file.h"

#include "unittest/file/file_unittest_common.h"

using cg::file::By_line_iterator;
using cg::file::File_exception;


namespace unittest {

TEST_CLASS(By_line_iterator_unitetst) {
public:

	TEST_METHOD(ctors_and_end_iterator)
	{
		// no file iterator (the end iterator)
		By_line_iterator it0;
		Assert::IsTrue(it0.filename().empty());
		Assert::IsFalse(it0.is_open());
		Assert::IsTrue(it0 == By_line_iterator::end);

		// empty file iterator
		By_line_iterator it1(Filenames::empty_file);
		Assert::AreEqual(Filenames::empty_file, it1.filename());
		Assert::IsTrue(it1.is_open());
		Assert::IsTrue((*it1).empty()); // empty file has no content
		Assert::IsTrue(it1->empty());  // empty file has no content
		Assert::IsTrue(it1 != By_line_iterator::end);
		
		{ // single line file iterator
			std::string expected_first_line = "abc123";
			By_line_iterator it(Filenames::single_line_ascii_file);
			Assert::AreEqual(Filenames::single_line_ascii_file, it.filename());
			Assert::IsTrue(it.is_open());
			Assert::AreEqual(expected_first_line, *it);
			Assert::AreEqual(expected_first_line.size(), it->size());
			Assert::IsTrue(it != By_line_iterator::end);
		}

		{ // multiline file iterator
			std::string expected_first_line = "123";
			By_line_iterator it(Filenames::multiline_ascii_file);
			Assert::AreEqual(Filenames::multiline_ascii_file, it.filename());
			Assert::IsTrue(it.is_open());
			Assert::AreEqual(expected_first_line, *it);
			Assert::AreEqual(expected_first_line.size(), it->size());
			Assert::IsTrue(it != By_line_iterator::end);
		}
	}

	TEST_METHOD(iterate_no_line_feeds)
	{
		By_line_iterator it0;
		Assert::ExpectException<File_exception>([&] { ++it0; });

		// empty file iterator
		By_line_iterator it_empty(Filenames::empty_file, false);
		Assert::IsTrue((*it_empty).empty()); // empty file has no content
		++it_empty;
		Assert::IsTrue(it_empty == By_line_iterator::end);

		// single line file iterator
		std::string expected_line = "abc123";
		By_line_iterator it_single(Filenames::single_line_ascii_file, false);
		Assert::AreEqual(expected_line, *it_single);
		++it_single;
		Assert::IsTrue(it_single == By_line_iterator::end);

		// multiline file iterator
		std::string expected_lines[] = { "123", "abc", "", "last_line" };
		By_line_iterator it_multi(Filenames::multiline_ascii_file, false);
		for (size_t i = 0; it_multi != By_line_iterator::end; ++it_multi, ++i) {
			Assert::AreEqual(expected_lines[i], *it_multi);
		}
		Assert::IsTrue(it_multi == By_line_iterator::end);
	}

	TEST_METHOD(iterate_with_line_feeds)
	{
		By_line_iterator it0;
		Assert::ExpectException<File_exception>([&] { ++it0; });

		{ // empty file iterator
			std::string expected_line = "\n";
			By_line_iterator it_empty(Filenames::empty_file, true);
			Assert::AreEqual(expected_line, *it_empty);
			++it_empty;
			Assert::IsTrue(it_empty == By_line_iterator::end);
		}

		{ // single line file iterator
			std::string expected_line = "abc123\n";
			By_line_iterator it_single(Filenames::single_line_ascii_file, true);
			Assert::AreEqual(expected_line, *it_single);
			++it_single;
			Assert::IsTrue(it_single == By_line_iterator::end);
		}

		// multiline file iterator
		std::string expected_lines[] = { "123\n", "abc\n", "\n", "last_line\n" };
		By_line_iterator it_multi(Filenames::multiline_ascii_file, true);
		for (size_t i = 0; it_multi != By_line_iterator::end; ++it_multi, ++i) {
			Assert::AreEqual(expected_lines[i], *it_multi);
		}
		Assert::IsTrue(it_multi == By_line_iterator::end);
	}

	TEST_METHOD(post_increment)
	{
		std::string expected_lines[] = { "123", "abc", "", "last_line" };
		By_line_iterator it(Filenames::multiline_ascii_file, false);
		for (size_t i = 0; it != By_line_iterator::end; ++i) {
			Assert::AreEqual(expected_lines[i], *it++);
		}
	}
};

} // namespace unittest