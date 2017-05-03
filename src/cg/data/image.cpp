#include "cg/data/image.h"

#include <cassert>
#include <cstring>
#include <algorithm>
#include <limits>
#include <vector>
#include "cg/base/base.h"
#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG 
#include "stb/stb_image.h"



namespace cg {
namespace data {

image_2d::image_2d(const char* filename, uint8_t channel_count, bool flip_vertically)
{
	assert(filename && std::strlen(filename));

	stbi_set_flip_vertically_on_load(flip_vertically);

	int width = 0;
	int height = 0;
	int actual_channel_count = 0;
	data = stbi_load(filename, &width, &height, &actual_channel_count, channel_count);
	
	if (!data) {
		const char* stb_error = stbi_failure_reason();
		throw std::runtime_error(EXCEPTION_MSG("Loading ", filename, " image error. ", stb_error));
	}

	size.x = width;
	size.y = height;
	uint8_t cc = (channel_count) ? channel_count : uint8_t(actual_channel_count);
	switch (cc) {
		case 1: pixel_format = pixel_format::red_8; break;
		case 2: pixel_format = pixel_format::rg_8; break;
		case 3: pixel_format = pixel_format::rgb_8; break;
		case 4: pixel_format = pixel_format::rgba_8; break;
	}
}

image_2d::image_2d(const std::string& filename, uint8_t channel_count, bool flip_vertically)
	: image_2d(filename.c_str(), channel_count, flip_vertically)
{}

image_2d::image_2d(image_2d&& image) noexcept
	: data(image.data),
	size(image.size),
	pixel_format(image.pixel_format)
{
	image.data = nullptr;
	image.size = uint2::zero;
	image.pixel_format = pixel_format::none;
}

image_2d::~image_2d() noexcept
{
	dispose();
}

image_2d& image_2d::operator=(image_2d&& image) noexcept
{
	if (this == &image) return *this;

	dispose();

	data = image.data;
	size = image.size;
	pixel_format = image.pixel_format;

	image.data = nullptr;
	image.size = uint2::zero;
	image.pixel_format = pixel_format::none;

	return *this;
}

void image_2d::dispose() noexcept
{
	if (!data) return;

	stbi_image_free(data);
	data = nullptr;
	size = uint2::zero;
	pixel_format = pixel_format::none;
}

// ----- funcs -----

std::ostream& operator<<(std::ostream& out, const pixel_format& fmt)
{
	out << "Image_format::";

	switch (fmt) {
		case pixel_format::none:
			out << "none";
			break;

		case pixel_format::red_8:
			out << "red_8";
			break;

		case pixel_format::rg_8:
			out << "rg_8";
			break;

		case pixel_format::rgb_8:
			out << "rgb_8";
			break;

		case pixel_format::rgba_8:
			out << "rgba_8";
			break;
	}

	return out;
}

std::wostream& operator<<(std::wostream& out, const pixel_format& fmt)
{
	out << "Image_format::";

	switch (fmt) {
		case pixel_format::none:
			out << "none";
			break;

		case pixel_format::red_8:
			out << "red_8";
			break;

		case pixel_format::rg_8:
			out << "rg_8";
			break;

		case pixel_format::rgb_8:
			out << "rgb_8";
			break;

		case pixel_format::rgba_8:
			out << "rgba_8";
			break;
	}

	return out;
}

size_t byte_count(const pixel_format& fmt) noexcept
{
	switch (fmt) {
		default:
		case pixel_format::none: return 0;
		case pixel_format::red_8: return 1;
		case pixel_format::rg_8: return 2;
		case pixel_format::rgb_8: return 3;
		case pixel_format::rgba_8: return 4;
	}
}

size_t channel_count(const pixel_format& fmt) noexcept
{
	switch (fmt) {
		default:
		case pixel_format::none: return 0;
		
		case pixel_format::red_8: 
			return 1;

		case pixel_format::rg_8:
			return 2;
		
		case pixel_format::rgb_8:
			return 3;

		case pixel_format::rgba_8:
			return 4;
	}
}

} // namespace data
} // namespace cg