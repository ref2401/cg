#include "cg/data/image.h"

#include <algorithm>
#include <array>
#include <utility>
#include "cg/math/math.h"
#include "unittest/data/common_file.h"
#include "CppUnitTest.h"

using cg::uint2;
using cg::data::Image_2d;
using cg::data::Image_format;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace Microsoft { namespace VisualStudio { namespace CppUnitTestFramework {

template<> inline std::wstring ToString<cg::uint2>(const cg::uint2& t) { RETURN_WIDE_STRING(t); }
template<> inline std::wstring ToString<cg::data::Image_format>(const cg::data::Image_format& t) { RETURN_WIDE_STRING(t); }

}}} // namespace Microsoft::VisualStudio::CppUnitTestFramewor


namespace unittest {

TEST_CLASS(cg_data_image_Image_2d) {
public:

	TEST_METHOD(ctors)
	{
		using cg::data::byte_count;

		Image_2d img0;
		Assert::IsNull(img0.data());
		Assert::AreEqual(uint2::zero, img0.size());
		Assert::AreEqual(Image_format::none, img0.format());
		Assert::AreEqual<size_t>(0, img0.byte_count());

		Image_2d img1(uint2(3, 2), Image_format::red_8);
		Assert::IsNotNull(img1.data());
		Assert::AreEqual(uint2(3, 2), img1.size());
		Assert::AreEqual(Image_format::red_8, img1.format());
		Assert::AreEqual<size_t>(3 * 2 * byte_count(Image_format::red_8), img1.byte_count());

		Image_2d img2(uint2(4, 3), Image_format::rgba_8);
		Assert::IsNotNull(img2.data());
		Assert::AreEqual(uint2(4, 3), img2.size());
		Assert::AreEqual(Image_format::rgba_8, img2.format());
		Assert::AreEqual<size_t>(4 * 3 * byte_count(Image_format::rgba_8), img2.byte_count());

		// copy ctor.
		Image_2d img_c = img1;
		Assert::AreNotEqual(img1.data(), img_c.data());
		Assert::IsTrue(std::equal(img1.data(), img1.data() + img1.byte_count(), img_c.data()));
		Assert::AreEqual(img1.size(), img_c.size());
		Assert::AreEqual(img1.format(), img_c.format());

		// move ctor.
		auto expected_data = img_c.data();
		Image_2d img_m = std::move(img_c);
		Assert::AreNotEqual(img1.data(), img_m.data());
		Assert::AreEqual(expected_data, img_m.data());
		Assert::IsTrue(std::equal(img1.data(), img1.data() + img1.byte_count(), img_m.data()));
		Assert::AreEqual(img1.size(), img_m.size());
		Assert::AreEqual(img1.format(), img_m.format());
		Assert::IsNull(img_c.data());
		Assert::AreEqual(uint2::zero, img_c.size());
		Assert::AreEqual(Image_format::none, img_c.format());
		Assert::AreEqual<size_t>(0, img_c.byte_count());

		// move empty
		Image_2d img_me = std::move(img0);
		Assert::IsNull(img_me.data());
		Assert::AreEqual(uint2::zero, img_me.size());
		Assert::AreEqual(Image_format::none, img_me.format());
		Assert::AreEqual<size_t>(0, img_me.byte_count());
		Assert::IsNull(img0.data());
		Assert::AreEqual(uint2::zero, img0.size());
		Assert::AreEqual(Image_format::none, img0.format());
		Assert::AreEqual<size_t>(0, img0.byte_count());
	}

	TEST_METHOD(assignments)
	{
		Image_2d img(uint2(4, 3), Image_format::bgra_8);

		// copy assignment
		Image_2d img_c;
		img_c = img;
		Assert::AreNotEqual(img.data(), img_c.data());
		Assert::IsTrue(std::equal(img.data(), img.data() + img.byte_count(), img_c.data()));
		Assert::AreEqual(img.size(), img_c.size());
		Assert::AreEqual(img.format(), img_c.format());

		// move assignment
		Image_2d img_m;
		img_m  = std::move(img_c);
		Assert::AreNotEqual(img.data(), img_m.data());
		Assert::IsTrue(std::equal(img.data(), img.data() + img.byte_count(), img_m.data()));
		Assert::AreEqual(img.size(), img_m.size());
		Assert::AreEqual(img.format(), img_m.format());
		Assert::IsNull(img_c.data());
		Assert::AreEqual(uint2::zero, img_c.size());
		Assert::AreEqual(Image_format::none, img_c.format());
		Assert::AreEqual<size_t>(0, img_c.byte_count());

		// move self
		auto expected_data = img_m.data();
		auto expected_size = img_m.size();
		auto expected_format = img_m.format();
		img_m = std::move(img_m);
		Assert::AreEqual(expected_data, img_m.data());
		Assert::AreEqual(expected_size, img_m.size());
		Assert::AreEqual(expected_format, img_m.format());

		// move empty
		Image_2d img0;
		Image_2d img_me;
		img_me = std::move(img0);
		Assert::IsNull(img_me.data());
		Assert::AreEqual(uint2::zero, img_me.size());
		Assert::AreEqual(Image_format::none, img_me.format());
		Assert::AreEqual<size_t>(0, img_me.byte_count());
		Assert::IsNull(img0.data());
		Assert::AreEqual(uint2::zero, img0.size());
		Assert::AreEqual(Image_format::none, img0.format());
		Assert::AreEqual<size_t>(0, img0.byte_count());
	}

	TEST_METHOD(write)
	{
		Image_2d img(uint2(2, 2), Image_format::red_8);
		
		std::array<unsigned char, 4> arr = { 0, 1, 2, 3 };
		for (size_t i = 0; i < arr.size(); ++i) {
			size_t offset = img.write(i, &arr[i], 1);
			Assert::AreEqual(i + 1, offset);
		}

		Assert::IsTrue(std::equal(arr.cbegin(), arr.cend(), img.data()));
	}
};

TEST_CLASS(cg_data_image_Image_format) {
public:

	TEST_METHOD(byte_count)
	{
		using cg::data::byte_count;

		Assert::AreEqual<size_t>(0, byte_count(Image_format::none));
		Assert::AreEqual<size_t>(1, byte_count(Image_format::red_8));
		Assert::AreEqual<size_t>(3, byte_count(Image_format::rgb_8));
		Assert::AreEqual<size_t>(4, byte_count(Image_format::rgba_8));
		Assert::AreEqual<size_t>(3, byte_count(Image_format::bgr_8));
		Assert::AreEqual<size_t>(4, byte_count(Image_format::bgra_8));
	}
};

TEST_CLASS(cg_data_image_Funcs) {
public:

	TEST_METHOD(load_image_tga)
	{
		using cg::uint2;
		using cg::data::Image_2d;
		using cg::data::Image_format;
		using cg::data::load_image_tga;

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