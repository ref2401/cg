#ifndef UNITTEST_FILE_COMMON_FILE_H_
#define UNITTEST_FILE_COMMON_FILE_H_

#include "cg/file/file.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace unittest {

class Filenames final {
public:

	// 123\nabc\n\nlast_line
	static const std::string ascii_multiline;
	// abc123
	static const std::string ascii_single_line;
	static const std::string empty_file;
	static const std::string not_real_shader_program_name;
	static const std::string not_real_vertex_glsl;
	static const std::string not_real_pixel_glsl;
	static const std::string tga_gayscale_r_compressed_rect_3x2;
	static const std::string tga_grayscale_r_square_2x2;
	static const std::string tga_true_color_rgb_compressed_rect_3x2;
	static const std::string tga_true_color_rgb_square_2x2;
	static const std::string wavefront_rect_positive_indices_p;
	static const std::string wavefront_rect_positive_indices_pn;
	static const std::string wavefront_rect_positive_indices_pntc;
	static const std::string wavefront_rect_positive_indices_ptc;
};

} // namespace unittest

#endif // UNITTEST_FILE_COMMON_FILE_H_
