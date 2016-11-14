#include "cg/file/file_image.h"

#include <algorithm>
#include <array>
#include <iterator>
#include "unittest/file/common_file.h"

namespace unittest {

TEST_CLASS(cg_file_image_Funcs) {
public:

	TEST_METHOD(load_image_tga)
	{
		using cg::uint2;
		using cg::data::Image_2d;
		using cg::data::Image_format;
		using cg::file::load_image_tga;

		{ // grayscale, red_8, no compression
		  // image data:
		  // black (r: 0x00), dark gray (r: 0x3f)
		  // white (r: 0xff), light gray (r: 0x7f)
		  // image's pixel sequence: white, light-gray, black, dark-gray
			std::array<unsigned char, 4> expected_data = {
				0xff,	// white
				0x7f,	// light gray
				0x00,	// black
				0x3f	// dark gray
			};

			Image_2d img = load_image_tga(Filenames::tga_grayscale_r_square_2x2);
			Assert::IsTrue(uint2(2) == img.size());
			Assert::IsTrue(Image_format::red_8 == img.format());
			Assert::IsTrue(std::equal(expected_data.cbegin(), expected_data.cend(), img.data()));
		}

		{ // true color, bgr_8, no compression
		  // image data:
		  // white (rgb/bgr: 0xffffff), red (rgb: 0xff0000 -> bgr: 0x0000ff)
		  // green (rgb/bgr: 0x00ff00), blue(rgb: 0x0000ff -> bgr: 0xff0000)
		  // image's pixel sequence: green, blue, white, red
			std::array<unsigned char, 4 * 3> expected_data = {
				0x00, 0xff, 0x00,	// green
				0xff, 0x00, 0x00,	// blue
				0xff, 0xff, 0xff,	// white
				0x00, 0x00, 0xff	// red
			};

			Image_2d img = load_image_tga(Filenames::tga_true_color_rgb_square_2x2);
			Assert::IsTrue(uint2(2) == img.size());
			Assert::IsTrue(Image_format::bgr_8 == img.format());
			Assert::IsTrue(std::equal(expected_data.cbegin(), expected_data.cend(), img.data()));
		}

		{ // grayscale, red_8, rle compression
		  // image data:
		  // black (r: 0x00), black (r: 0x00), dark gray (r: 0x3f)
		  // white (r: 0xff), white (r: 0xff), light gray (r: 0x7f)
		  // image's pixel sequence: white, white, light-gray, black, black, dark-gray
			std::array<unsigned char, 6> expected_data = {
				0xff,	// white
				0xff,	// white
				0x7f,	// light gray
				0x00,	// black
				0x00,	// black
				0x3f	// dark gray
			};

			Image_2d img = load_image_tga(Filenames::tga_gayscale_r_compressed_rect_3x2);
			Assert::IsTrue(uint2(3, 2) == img.size());
			Assert::IsTrue(Image_format::red_8 == img.format());
			Assert::IsTrue(std::equal(expected_data.cbegin(), expected_data.cend(), img.data()));
		}

		{ // true color, bgr_8, rle compression
		  // image data:
		  // white (rgb/bgr: 0xffffff), white (rgb/bgr: 0xffffff), red (rgb: 0xff0000 -> bgr: 0x0000ff)
		  // green (rgb/bgr: 0x00ff00), green (rgb/bgr: 0x00ff00), blue(rgb: 0x0000ff -> bgr: 0xff0000)
		  // image's pixel sequence: green, green, blue, white, white, red
			std::array<unsigned char, 3 * 2 * 3> expected_data = {
				0x00, 0xff, 0x00,	// green
				0x00, 0xff, 0x00,	// green
				0xff, 0x00, 0x00,	// blue
				0xff, 0xff, 0xff,	// white
				0xff, 0xff, 0xff,	// white
				0x00, 0x00, 0xff	// red
			};

			Image_2d img = load_image_tga(Filenames::tga_true_color_rgb_compressed_rect_3x2);
			Assert::IsTrue(uint2(3, 2) == img.size());
			Assert::IsTrue(Image_format::bgr_8 == img.format());
			Assert::IsTrue(std::equal(expected_data.cbegin(), expected_data.cend(), img.data()));
		}
	}
};

} // namespace unittest
