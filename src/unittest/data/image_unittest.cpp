#include "cg/data/image.h"

#include <algorithm>
#include <utility>
#include "cg/math/math.h"
#include "unittest/math/common_math.h"
#include "CppUnitTest.h"

using cg::uint2;
using cg::data::Image_2d;
using cg::data::Image_format;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace Microsoft { namespace VisualStudio { namespace CppUnitTestFramework {

template<> inline std::wstring ToString<cg::data::Image_format>(const cg::data::Image_format& t) { RETURN_WIDE_STRING(t); }

}}} // namespace Microsoft::VisualStudio::CppUnitTestFramewor


namespace unittest {

TEST_CLASS(cg_data_Image_2d) {
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
};

TEST_CLASS(cg_data_Image_format) {
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

} // namespace unittest