#include "cg/data/image.h"

#include <cassert>
#include <cstring>


namespace cg {
namespace data {

Image_2d::Image_2d(uint2 size, Image_format format) :
	_ptr(new unsigned char[size.width * size.height * cg::data::byte_count(format)]),
	_size(size),
	_format(format)
{
	assert(greater_than(_size, 0));
	assert(format != Image_format::none);
}

Image_2d::Image_2d(const Image_2d& img) :
	_ptr(new unsigned char[img.byte_count()]),
	_size(img._size),
	_format(img._format)
{
	std::memcpy(_ptr, img._ptr, img.byte_count());
}

Image_2d::Image_2d(Image_2d&& img) noexcept :
	_ptr(img._ptr),
	_size(img._size),
	_format(img._format)
{
	img._ptr = nullptr;
	img._size = uint2::zero;
	img._format = Image_format::none;
}

Image_2d::~Image_2d() noexcept
{
	dispose();
}

Image_2d& Image_2d::operator=(const Image_2d& img)
{
	if (this == &img) return *this;

	if (_size != img._size || _format != img._format) {
		dispose();
		_ptr = new unsigned char[img.byte_count()];
	}

	std::memcpy(_ptr, img._ptr, img.byte_count());
	_size = img._size;
	_format = img._format;
	return *this;
}

Image_2d& Image_2d::operator=(Image_2d&& img) noexcept
{
	if (this == &img) return *this;

	dispose();
	_ptr = img._ptr;
	_size = img._size;
	_format = img._format;
	
	img._ptr = nullptr;
	img._size = uint2::zero;
	img._format = Image_format::none;
	return *this;
}

void Image_2d::dispose() noexcept
{
	if (!_ptr) return;

	delete[] _ptr;
	_ptr = nullptr;
}

size_t Image_2d::write(size_t offset, unsigned char* ptr, size_t count) noexcept
{
	if (count == 0) return offset;

	size_t bytes_to_write = sizeof(unsigned char) * count;
	size_t ret_offset = offset + bytes_to_write;
	assert(ret_offset <= byte_count());

	std::memcpy(_ptr + offset, ptr, bytes_to_write);
	return ret_offset;
}

// ----- funcs -----

std::ostream& operator<<(std::ostream& out, const Image_format& fmt)
{
	out << "Image_format::";

	switch (fmt) {
		case Image_format::none:
			out << "none";
			break;

		case Image_format::red_8:
			out << "red_8";
			break;

		case Image_format::rgb_8:
			out << "rgb_8";
			break;

		case Image_format::rgba_8:
			out << "rgba_8";
			break;

		case Image_format::bgr_8:
			out << "bgr_8";
			break;

		case Image_format::bgra_8:
			out << "bgra_8";
			break;
	}

	return out;
}

std::wostream& operator<<(std::wostream& out, const Image_format& fmt)
{
	out << "Image_format::";

	switch (fmt) {
		case Image_format::none:
			out << "none";
			break;

		case Image_format::red_8:
			out << "red_8";
			break;

		case Image_format::rgb_8:
			out << "rgb_8";
			break;

		case Image_format::rgba_8:
			out << "rgba_8";
			break;

		case Image_format::bgr_8:
			out << "bgr_8";
			break;

		case Image_format::bgra_8:
			out << "bgra_8";
			break;
	}

	return out;
}

size_t byte_count(const Image_format& fmt) noexcept
{
	switch (fmt) {
		default:
		case Image_format::none: return 0;
		case Image_format::red_8: return 1;
		case Image_format::rgb_8: return 3;
		case Image_format::rgba_8: return 4;
		case Image_format::bgr_8: return 3;
		case Image_format::bgra_8: return 4;
	}
}

} // namespace data
} // namespace cg