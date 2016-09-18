#ifndef UNITTEST_FILE_COMMON_FILE_H_
#define UNITTEST_FILE_COMMON_FILE_H_

#include "cg/file/file.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace unittest {

class Filenames final {
public:

	static const std::string ascii_multiline;
	static const std::string ascii_single_line;
	static const std::string empty_file;
	static const std::string wavefront_triangle_p;
	static const std::string wavefront_triangle_pn;
	static const std::string wavefront_triangle_ptc;
	static const std::string wavefront_triangle_pntc;
};

} // namespace unittest

#endif // UNITTEST_FILE_COMMON_FILE_H_
