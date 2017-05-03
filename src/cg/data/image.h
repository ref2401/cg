#ifndef CG_DATA_IMAGE_H_
#define CG_DATA_IMAGE_H_

#include <cstdint>
#include <iostream>
#include "cg/base/math.h"


namespace cg {
namespace data {

// Describes pixel's channels and their size. 
enum class pixel_format :unsigned char {
	none = 0,

	red_8,
	rg_8,
	rgb_8,
	rgba_8
};

// NOTE(ref2401): If you ever need any image processing then you should implement
// processing algorithms as func(Image_data): write, flip, rg_swap, convert, etc...
// struct Image_view { pointer, size, pixel_format };
struct image_2d final {

	image_2d() noexcept = default;

	image_2d(const char* filename, uint8_t channel_count = 0, bool flip_vertically = false);

	image_2d(const std::string& filename, uint8_t channel_count = 0, bool flip_vertically = false);

	image_2d(image_2d&& image) noexcept;
	
	~image_2d() noexcept;


	image_2d& operator=(image_2d&& image) noexcept;


	void dispose() noexcept;


	// Pointer to the underlying buffer serving as pixel storage.
	uint8_t* data = nullptr;

	// Size of the image in pixels.
	uint2 size;

	// Pixel format of this image.
	pixel_format pixel_format = pixel_format::none;
};

// ----- funcs -----

std::ostream& operator<<(std::ostream& out, const pixel_format& fmt);

std::wostream& operator<<(std::wostream& out, const pixel_format& fmt);


// Returns the number of bytes occupied by one pixel of the specified format.
size_t byte_count(const pixel_format& fmt) noexcept;

// Total number of bytes occupied by this image.
inline size_t byte_count(const image_2d& image) noexcept
{
	return square(image.size);
}

// Returns the number of color channels for the given image format.
size_t channel_count(const pixel_format& fmt) noexcept;

} // namespace data
} // namespace cg

#endif // CG_DATA_IMAGE_H_
