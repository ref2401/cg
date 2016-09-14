#include "cg/file/file.h"

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

};

} // namespace unittest
