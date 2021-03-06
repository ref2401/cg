#include "cg/data/image.h"

#include <cstdint>
#include <algorithm>
#include <array>
#include <iterator>
#include <type_traits>
#include <utility>
#include "cg/base/math.h"
#include "unittest/data/common_file.h"
#include "CppUnitTest.h"

using cg::data::pixel_format;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace Microsoft { namespace VisualStudio { namespace CppUnitTestFramework {

template<> inline std::wstring ToString<uint2>(const uint2& t) { RETURN_WIDE_STRING(t); }
template<> inline std::wstring ToString<cg::data::pixel_format>(const cg::data::pixel_format& t) { RETURN_WIDE_STRING(t); }

}}} // namespace Microsoft::VisualStudio::CppUnitTestFramewor


namespace unittest {

TEST_CLASS(cg_data_image_Image_2d) {
public:

};

TEST_CLASS(cg_data_image_pixel_format) {
public:

	TEST_METHOD(byte_count)
	{
		using cg::data::byte_count;

		Assert::AreEqual<size_t>(0, byte_count(pixel_format::none));
		Assert::AreEqual(3 * sizeof(float), byte_count(pixel_format::rgb_32f));
		Assert::AreEqual(4 * sizeof(float), byte_count(pixel_format::rgba_32f));
		Assert::AreEqual<size_t>(1, byte_count(pixel_format::red_8));
		Assert::AreEqual<size_t>(2, byte_count(pixel_format::rg_8));
		Assert::AreEqual<size_t>(3, byte_count(pixel_format::rgb_8));
		Assert::AreEqual<size_t>(4, byte_count(pixel_format::rgba_8));
	}

	TEST_METHOD(channel_count)
	{
		using cg::data::channel_count;

		Assert::AreEqual<size_t>(0, channel_count(pixel_format::none));
		Assert::AreEqual<size_t>(3, channel_count(pixel_format::rgb_32f));
		Assert::AreEqual<size_t>(4, channel_count(pixel_format::rgba_32f));
		Assert::AreEqual<size_t>(1, channel_count(pixel_format::red_8));
		Assert::AreEqual<size_t>(2, channel_count(pixel_format::rg_8));
		Assert::AreEqual<size_t>(3, channel_count(pixel_format::rgb_8));
		Assert::AreEqual<size_t>(4, channel_count(pixel_format::rgba_8));
	}
};

} // namespace unittest
