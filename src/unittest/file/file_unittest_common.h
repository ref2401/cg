#ifndef UNITTEST_FILE_FILE_UNITTEST_COMMON_H_
#define UNITTEST_FILE_FILE_UNITTEST_COMMON_H_

#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace unittest {

class Filenames final {
public:

	static const std::string empty_file;
	static const std::string multiline_ascii_file;
	static const std::string single_line_ascii_file;
};

} // namespace unittest

#endif // UNITTEST_FILE_FILE_UNITTEST_COMMON_H_