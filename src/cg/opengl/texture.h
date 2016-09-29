#ifndef CG_OPENGL_TEXTURE_H_
#define CG_OPENGL_TEXTURE_H_

#include <ostream>
#include "cg/math/math.h"
#include "cg/opengl/opengl_def.h"
#include "cg/opengl/utility.h"


namespace cg {
namespace opengl {

struct Texture_2d_sub_image final {
	Texture_2d_sub_image(size_t mip_level, cg::uint2 offset, cg::uint2 size,
		GLenum pixel_format, GLenum pixel_type, void* pixels) noexcept :
		offset(offset), size(size), mip_level(mip_level), pixel_type(pixel_type),
		pixel_format(pixel_format), pixels(pixels)
	{}


	// Texel offset in x & y directions.
	cg::uint2 offset;

	// Texel width & height of subimage.
	cg::uint2 size;

	// The level-of-detail number (mipmap level).
	size_t mip_level;

	// The data type of a pixel.
	GLenum pixel_type;

	// Specifies the format of pixels pointed by pixels.
	GLenum pixel_format;

	// Pointer to the image in memory. 
	void* pixels;

};

enum class Texture_format : unsigned char {
	none = 0,

	red_8,
	red_32f,

	rg_8,
	rg_32f,

	rgb_8,
	rgb_32f,

	rgba_8,
	rgba_32f,

	depth_24,
	depth_24_stencil_8,
	depth_32,
	depth_32f,
	depth_32f_stencil_8
};

// ---- funcs -----

inline bool operator==(const Texture_2d_sub_image& lhs, const Texture_2d_sub_image& rhs) noexcept
{
	return (lhs.offset == rhs.offset)
		&& (lhs.size == rhs.size)
		&& (lhs.mip_level == rhs.mip_level)
		&& (lhs.pixel_type == rhs.pixel_type)
		&& (lhs.pixel_format == rhs.pixel_format)
		&& (lhs.pixels == rhs.pixels);
}

inline bool operator!=(const Texture_2d_sub_image& lhs, const Texture_2d_sub_image& rhs) noexcept
{
	return !(lhs == rhs);
}

inline std::ostream& operator<<(std::ostream& out, const Texture_2d_sub_image& subimg)
{
	out << "Texture_2d_sub_image(" << subimg.offset << ", " << subimg.size << ", "
		<< subimg.mip_level << ", " << subimg.pixel_type << ", " 
		<< subimg.pixel_format << ", " << subimg.pixels << ')';
	return out;
}

inline std::wostream& operator<<(std::wostream& out, const Texture_2d_sub_image& subimg)
{
	out << "Texture_2d_sub_image(" << subimg.offset << ", " << subimg.size << ", "
		<< subimg.mip_level << ", " << subimg.pixel_type << ", "
		<< subimg.pixel_format << ", " << subimg.pixels << ')';
	return out;
}


} // namespace opengl
} // namespace cg

#endif // CG_OPENGL_TEXTURE_H_