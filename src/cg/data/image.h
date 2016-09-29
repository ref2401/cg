#ifndef CG_DATA_IMAGE_H_
#define CG_DATA_IMAGE_H_

#include <iostream>
#include "cg/math/math.h"


namespace cg {
namespace data {

// Describes pixel's channels and their size. 
enum class Image_format :unsigned char {
	none = 0,

	red_8,
	rgb_8,
	rgba_8,

	bgr_8,
	bgra_8
};

// Returns a number of bytes occupied by one pixel of the specified format.
size_t byte_count(const Image_format& fmt) noexcept;

class Image_2d final {
public:

	Image_2d() noexcept = default;

	Image_2d(uint2 size, Image_format format);

	Image_2d(const Image_2d& img);

	Image_2d(Image_2d&& img) noexcept;

	~Image_2d() noexcept;


	Image_2d& operator=(const Image_2d& img);

	Image_2d& operator=(Image_2d&& img) noexcept;


	// Total number of bytes occupied by this image.
	size_t byte_count() const noexcept
	{
		return (_size.width * _size.height) * cg::data::byte_count(_format);
	}

	// Returns pointer to the underlying buffer serving as pixel storage.
	unsigned char* data() noexcept 
	{
		return _ptr;
	}

	// Returns pointer to the underlying buffer serving as pixel storage.
	const unsigned char* data() const noexcept
	{
		return _ptr;
	}

	// Pixel format of this image.
	Image_format format() const noexcept
	{
		return _format;
	}

	// Size of the image in pixels.
	uint2 size() const noexcept
	{
		return _size;
	}

	// Writes a sequence of bytes into the image's underlying buffer.
	// -	offset: Byte offset from the beginig of the underlying buffer.
	// -	ptr: Pointer to the first byte in the sequence.
	// -	count: How many bytes are in the sequence pointed by ptr.
	//		If count is 0 the method returns immediately.
	// Returns:	offset + sizeof(unsigned char) * count.
	//			Byte offset into the buffer that can be used for subsequent writes.	
	size_t write(size_t offset, unsigned char* ptr, size_t count) noexcept;

private:
	void dispose() noexcept;

	unsigned char* _ptr = nullptr;
	uint2 _size = uint2::zero;
	Image_format _format = Image_format::none;
};

// ----- funcs -----

std::ostream& operator<<(std::ostream& out, const Image_format& fmt);

std::wostream& operator<<(std::wostream& out, const Image_format& fmt);


} // namespace data
} // namespace cg

#endif // CG_DATA_IMAGE_H_
