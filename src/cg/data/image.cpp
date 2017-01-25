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

Image_2d::Image_2d(const char* filename, uint8_t channel_count, bool flip_vertically)
{
	assert(filename && std::strlen(filename));
	assert(channel_count >= 0);

	stbi_set_flip_vertically_on_load(flip_vertically);

	int width = 0;
	int height = 0;
	int actual_channel_count = 0;
	_data = stbi_load(filename, &width, &height, &actual_channel_count, channel_count);
	
	if (!_data) {
		const char* stb_error = stbi_failure_reason();
		throw std::runtime_error(EXCEPTION_MSG("Loading ", filename, " image error. ", stb_error));
	}

	_size.width = width;
	_size.height = height;
	uint8_t cc = (channel_count) ? channel_count : uint8_t(actual_channel_count);
	switch (cc) {
		case 1: _pixel_format = Pixel_format::red_8; break;
		//case 2: _pixel_format = Pixel_format::rg_8; break;
		case 3: _pixel_format = Pixel_format::rgb_8; break;
		case 4: _pixel_format = Pixel_format::rgba_8; break;
	}
}

Image_2d::Image_2d(const std::string& filename, uint8_t channel_count, bool flip_vertically)
	: Image_2d(filename.c_str(), channel_count, flip_vertically)
{}

Image_2d::Image_2d(Image_2d&& image) noexcept
	: _data(image._data),
	_size(image._size),
	_pixel_format(image._pixel_format)
{
	image._data = nullptr;
	image._size = uint2::zero;
	image._pixel_format = Pixel_format::none;
}

Image_2d::~Image_2d() noexcept
{
	dispose();
}

Image_2d& Image_2d::operator=(Image_2d&& image) noexcept
{
	if (this == &image) return *this;

	dispose();

	_data = image._data;
	_size = image._size;
	_pixel_format = image._pixel_format;

	image._data = nullptr;
	image._size = uint2::zero;
	image._pixel_format = Pixel_format::none;

	return *this;
}

void Image_2d::dispose() noexcept
{
	if (!_data) return;

	stbi_image_free(_data);
	_data = nullptr;
	_size = uint2::zero;
	_pixel_format = Pixel_format::none;
}
// ----- funcs -----

std::ostream& operator<<(std::ostream& out, const Pixel_format& fmt)
{
	out << "Image_format::";

	switch (fmt) {
		case Pixel_format::none:
			out << "none";
			break;

		case Pixel_format::red_8:
			out << "red_8";
			break;

		case Pixel_format::rgb_8:
			out << "rgb_8";
			break;

		case Pixel_format::rgba_8:
			out << "rgba_8";
			break;

		case Pixel_format::bgr_8:
			out << "bgr_8";
			break;

		case Pixel_format::bgra_8:
			out << "bgra_8";
			break;
	}

	return out;
}

std::wostream& operator<<(std::wostream& out, const Pixel_format& fmt)
{
	out << "Image_format::";

	switch (fmt) {
		case Pixel_format::none:
			out << "none";
			break;

		case Pixel_format::red_8:
			out << "red_8";
			break;

		case Pixel_format::rgb_8:
			out << "rgb_8";
			break;

		case Pixel_format::rgba_8:
			out << "rgba_8";
			break;

		case Pixel_format::bgr_8:
			out << "bgr_8";
			break;

		case Pixel_format::bgra_8:
			out << "bgra_8";
			break;
	}

	return out;
}

size_t byte_count(const Pixel_format& fmt) noexcept
{
	switch (fmt) {
		default:
		case Pixel_format::none: return 0;
		case Pixel_format::red_8: return 1;
		case Pixel_format::rgb_8: return 3;
		case Pixel_format::rgba_8: return 4;
		case Pixel_format::bgr_8: return 3;
		case Pixel_format::bgra_8: return 4;
	}
}

size_t channel_count(const Pixel_format& fmt) noexcept
{
	switch (fmt) {
		default:
		case Pixel_format::none: return 0;
		
		case Pixel_format::red_8: 
			return 1;
		
		case Pixel_format::rgb_8:
		case Pixel_format::bgr_8: 
			return 3;

		case Pixel_format::rgba_8:
		case Pixel_format::bgra_8:
			return 4;
	}
}

} // namespace data
} // namespace cg