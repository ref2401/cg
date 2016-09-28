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

private:
	void dispose() noexcept;

	unsigned char* _ptr = nullptr;
	uint2 _size = uint2::zero;
	Image_format _format = Image_format::none;
};

// ----- funcs -----

inline std::ostream& operator<<(std::ostream& out, const Image_format& fmt)
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

inline std::wostream& operator<<(std::wostream& out, const Image_format& fmt)
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

inline size_t byte_count(const Image_format& fmt) noexcept
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

#endif // CG_DATA_IMAGE_H_
