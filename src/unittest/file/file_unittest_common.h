#ifndef UNITTEST_FILE_FILE_UNITTEST_COMMON_H_
#define UNITTEST_FILE_FILE_UNITTEST_COMMON_H_

#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace unittest {

class Filenames final {
public:

	static const std::string ascii_multiline;
	static const std::string ascii_single_line;
	static const std::string empty_file;
	static const std::string wavefront_triangle_p;
};

} // namespace unittest

#endif // UNITTEST_FILE_FILE_UNITTEST_COMMON_H_