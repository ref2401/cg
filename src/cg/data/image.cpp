#include "cg/data/image.h"

#include <cassert>
#include <cstring>


namespace cg {
namespace data {

Image_2d::Image_2d(uint2 size, Image_format format) :
	_ptr(new unsigned char[size.width * size.height]),
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

	dispose();

	_ptr = new unsigned char[img.byte_count()];
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

} // namespace data
} // namespace cg