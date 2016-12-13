#include "cg/data/image.h"

#include <cassert>
#include <cstring>
#include <algorithm>
#include <limits>
#include <vector>


namespace {

using cg::data::Image_format;

bool is_swap_rb_required(const Image_format& fmt_a, const Image_format& fmt_b) noexcept
{
	if (fmt_a == fmt_b) return false;

	switch (fmt_a) {
		default:
		case Image_format::none: return false;

		case Image_format::red_8:
		case Image_format::rgb_8:
		case Image_format::rgba_8:
			return fmt_b == Image_format::bgr_8 || fmt_b == Image_format::bgra_8;

		case Image_format::bgr_8:
		case Image_format::bgra_8:
			return fmt_b == Image_format::red_8 
				|| fmt_b == Image_format::rgb_8
				|| fmt_b == Image_format::rgba_8;
	}
}

} // namespace


namespace cg {
namespace data {

Image_2d::Image_2d(Image_format format, uint2 size) :
	_size(size),
	_format(format)
{
	assert(greater_than(_size, 0));
	assert(format != Image_format::none);

	_ptr = new unsigned char[size.width * size.height * cg::data::byte_count(format)];
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

void Image_2d::clear(const ubyte4& color) noexcept
{
	assert(_format == Image_format::rgba_8);
	if (_size == uint2::zero) return;

	size_t byte_count = this->byte_count();
	for (size_t i = 0; i <byte_count; i += 4) {
		memcpy(_ptr + i, color.data, 4);
	}
}

void Image_2d::dispose() noexcept
{
	if (!_ptr) return;

	delete[] _ptr;
	_ptr = nullptr;
}

void Image_2d::flip_vertical()
{
	if (_size.height <= 1) return;

	const size_t row_byte_count = _size.width * cg::data::byte_count(_format);
	std::vector<uint8_t> temp_buffer(row_byte_count);

	uint8_t* up_ptr = _ptr;
	uint8_t* bottom_ptr = _ptr + row_byte_count * (_size.height - 1);

	while (up_ptr < bottom_ptr) {
		memcpy(temp_buffer.data(), up_ptr, row_byte_count);
		memcpy(up_ptr, bottom_ptr, row_byte_count);
		memcpy(bottom_ptr, temp_buffer.data(), row_byte_count);

		up_ptr += row_byte_count;
		bottom_ptr -= row_byte_count;
	}
}

void Image_2d::write(const Image_2d& src_image) noexcept
{
	if (this == &src_image) return;

	assert(src_image._size != uint2::zero);
	assert(src_image._format != Image_format::none);
	assert(_size == src_image._size); // other options are not implemented

	if (_format == src_image._format) {
		memcpy(_ptr, src_image._ptr, src_image.byte_count());
	} 
	else {
		bool swap_rg = is_swap_rb_required(_format, src_image._format);
		if (swap_rg) {
			write_converted_swap_rb(src_image);
		}
		else {
			write_converted(src_image);
		}
	}
}

size_t Image_2d::write(size_t offset, uint8_t* ptr, size_t count) noexcept
{
	if (count == 0) return offset;

	size_t bytes_to_write = sizeof(unsigned char) * count;
	size_t ret_offset = offset + bytes_to_write;
	assert(ret_offset <= byte_count());

	std::memcpy(_ptr + offset, ptr, bytes_to_write);
	return ret_offset;
}

void Image_2d::write_converted(const Image_2d& src_image) noexcept
{
	const size_t src_byte_count = cg::data::byte_count(src_image._format);
	const size_t dest_byte_count = cg::data::byte_count(_format);

	uint8_t* src_ptr = src_image._ptr;
	uint8_t* dest_ptr = _ptr;


	for (size_t i = 0; i < src_image.byte_count(); i += src_byte_count) {
		ubyte4 pixel(0, 0, 0, std::numeric_limits<uint8_t>::max());
		memcpy(pixel.data, src_ptr, src_byte_count);
		memcpy(dest_ptr, pixel.data, dest_byte_count);

		src_ptr += src_byte_count;
		dest_ptr += dest_byte_count;
	}
}

void Image_2d::write_converted_swap_rb(const Image_2d& src_image) noexcept
{
	const size_t src_byte_count = cg::data::byte_count(src_image._format);
	const size_t dest_byte_count = cg::data::byte_count(_format);

	uint8_t* src_ptr = src_image._ptr;
	uint8_t* dest_ptr = _ptr;


	for (size_t i = 0; i < src_image.byte_count(); i += src_byte_count) {
		ubyte4 pixel(0, 0, 0, std::numeric_limits<uint8_t>::max());
		memcpy(pixel.data, src_ptr, src_byte_count);
		
		std::swap(pixel.r, pixel.b);
		memcpy(dest_ptr, pixel.data, dest_byte_count);

		src_ptr += src_byte_count;
		dest_ptr += dest_byte_count;
	}
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

size_t channel_count(const Image_format& fmt) noexcept
{
	switch (fmt) {
		default:
		case Image_format::none: return 0;
		
		case Image_format::red_8: 
			return 1;
		
		case Image_format::rgb_8:
		case Image_format::bgr_8: 
			return 3;

		case Image_format::rgba_8:
		case Image_format::bgra_8:
			return 4;
	}
}

} // namespace data
} // namespace cg