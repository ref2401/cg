#ifndef CG_DATA_IMAGE_H_
#define CG_DATA_IMAGE_H_

#include <cstdint>
#include <iostream>
#include "cg/math/math.h"


namespace cg {
namespace data {

// Describes pixel's channels and their size. 
enum class Pixel_format :unsigned char {
	none = 0,

	red_8,
	rg_8,
	rgb_8,
	rgba_8
};

// Returns the number of bytes occupied by one pixel of the specified format.
size_t byte_count(const Pixel_format& fmt) noexcept;

// Returns the number of color channels for the given image format.
size_t channel_count(const Pixel_format& fmt) noexcept;


class Image_2d final {
public:

	Image_2d() noexcept = default;

	Image_2d(const char* filename, uint8_t channel_count = 0, bool flip_vertically = false);

	Image_2d(const std::string& filename, uint8_t channel_count = 0, bool flip_vertically = false);

	Image_2d(const Image_2d&) = delete;

	Image_2d(Image_2d&& image) noexcept;
	
	~Image_2d() noexcept;


	Image_2d& operator=(const Image_2d&) = delete;

	Image_2d& operator=(Image_2d&& image) noexcept;


	// Total number of bytes occupied by this image.
	size_t byte_count() const noexcept
	{
		return _size.square<size_t>();
	}

	// Returns pointer to the underlying buffer serving as pixel storage.
	uint8_t* data() noexcept
	{
		return _data;
	}

	// Returns pointer to the underlying buffer serving as pixel storage.
	const uint8_t* data() const noexcept
	{
		return _data;
	}

	// Pixel format of this image.
	Pixel_format pixel_format() const noexcept
	{
		return _pixel_format;
	}

	// Size of the image in pixels.
	uint2 size() const noexcept
	{
		return _size;
	}

private:

	// NOTE(ref2401): If you ever need any image processing then you should implement
	// processing algorithms as func(Image_data): write, flip, rg_swap, convert, etc...
	// struct Image_view { pointer, size, pixel_format };

	void dispose() noexcept;

	uint8_t* _data = nullptr;
	uint2 _size;
	Pixel_format _pixel_format = Pixel_format::none;
};

// ----- funcs -----

std::ostream& operator<<(std::ostream& out, const Pixel_format& fmt);

std::wostream& operator<<(std::wostream& out, const Pixel_format& fmt);

} // namespace data
} // namespace cg

#endif // CG_DATA_IMAGE_H_
