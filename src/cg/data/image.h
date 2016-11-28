#ifndef CG_DATA_IMAGE_H_
#define CG_DATA_IMAGE_H_

#include <cstdint>
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

// Returns the number of bytes occupied by one pixel of the specified format.
size_t byte_count(const Image_format& fmt) noexcept;

// Returns the number of color channels for the given image format.
size_t channel_count(const Image_format& fmt) noexcept;

class Image_2d final {
public:

	Image_2d() noexcept = default;

	Image_2d(Image_format format, uint2 size);

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
	uint8_t* data() noexcept 
	{
		return _ptr;
	}

	// Returns pointer to the underlying buffer serving as pixel storage.
	const unsigned char* data() const noexcept
	{
		return _ptr;
	}

	void flip_vertical();

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

	void write(const Image_2d& src_image) noexcept;

	// Writes a sequence of bytes into the image's underlying buffer.
	// -	offset: Byte offset from the beginig of the underlying buffer.
	// -	ptr: Pointer to the first byte in source the sequence.
	// -	count: How many bytes are in the sequence pointed by ptr.
	//		If count is 0 the method returns immediately.
	// 
	// Returns:	offset + sizeof(unsigned char) * count.
	//			Byte offset into the buffer that can be used for subsequent writes.	
	size_t write(size_t offset, uint8_t* ptr, size_t count) noexcept;

private:
	void dispose() noexcept;

	void write_converted(const Image_2d& src_image) noexcept;

	void write_converted_swap_rb(const Image_2d& src_image) noexcept;

	uint8_t* _ptr = nullptr;
	uint2 _size = uint2::zero;
	Image_format _format = Image_format::none;
};

// Load_image_params struct specifies which image file to load, 
// whether the pixel conversion & vertical flip must be performed.
struct Load_image_params final {
	Load_image_params() = default;

	Load_image_params(const std::string& filename, Image_format format, bool flip_vertical) :
		filename(filename), format(format), flip_vertical(flip_vertical)
	{}

	Load_image_params(const char* filename, Image_format format, bool flip_vertical) : 
		filename(filename), format(format), flip_vertical(flip_vertical)
	{}

	// Full image's file name.
	std::string filename;

	// format describes the desirted format of the image. It may differ from the
	// orginal image fromat stored in file.
	// If format == Image_format::none then no pixel conversion will be performed.
	Image_format format = Image_format::none;

	// Specifies whether the image must be flipped before after loading.
	bool flip_vertical = false;
};

// ----- funcs -----

std::ostream& operator<<(std::ostream& out, const Image_format& fmt);

std::wostream& operator<<(std::wostream& out, const Image_format& fmt);

Image_2d load_image_tga(const std::string& filename);

Image_2d load_image_tga(const char* filename);

Image_2d load_image_tga(const Load_image_params& params);

} // namespace data
} // namespace cg

#endif // CG_DATA_IMAGE_H_
