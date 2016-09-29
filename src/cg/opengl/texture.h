#ifndef CG_OPENGL_TEXTURE_H_
#define CG_OPENGL_TEXTURE_H_

#include <ostream>
#include "cg/data/image.h"
#include "cg/math/math.h"
#include "cg/opengl/opengl_def.h"
#include "cg/opengl/utility.h"


namespace cg {
namespace opengl {

struct Texture_2d_sub_image final {

	Texture_2d_sub_image(size_t mipmap_index, cg::uint2 offset, const cg::data::Image_2d& image) noexcept;

	Texture_2d_sub_image(size_t mipmap_index, cg::uint2 offset, cg::uint2 size,
		GLenum pixel_format, GLenum pixel_type, const void* pixels) noexcept;


	// Texel offset in x & y directions.
	cg::uint2 offset = cg::uint2::zero;

	// Texel width & height of subimage.
	cg::uint2 size = cg::uint2::zero;

	// The level-of-detail index (mipmap level).
	size_t mipmap_index = 0;

	// Specifies the format of pixels pointed by pixels.
	GLenum pixel_format;

	// The data type of a pixel.
	GLenum pixel_type;

	// Pointer to the image in memory. 
	const void* pixels;

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

class Texture_2d final {
public:

	Texture_2d() noexcept = default;

	// Creates a texture object using the specified image as data source.
	// The resulting texture will have 1 mipmap level, and size of the texture will be equal to size of the image.
	Texture_2d(Texture_format format, const cg::data::Image_2d& image) noexcept;

private:
	GLuint _id = Invalid::texture_id;
	Texture_format _format = Texture_format::none;
};

// ---- funcs -----

inline bool operator==(const Texture_2d_sub_image& lhs, const Texture_2d_sub_image& rhs) noexcept
{
	return (lhs.offset == rhs.offset)
		&& (lhs.size == rhs.size)
		&& (lhs.mipmap_index == rhs.mipmap_index)
		&& (lhs.pixel_type == rhs.pixel_type)
		&& (lhs.pixel_format == rhs.pixel_format)
		&& (lhs.pixels == rhs.pixels);
}

inline bool operator!=(const Texture_2d_sub_image& lhs, const Texture_2d_sub_image& rhs) noexcept
{
	return !(lhs == rhs);
}

std::ostream& operator<<(std::ostream& out, const Texture_2d_sub_image& subimg);

std::wostream& operator<<(std::wostream& out, const Texture_2d_sub_image& subimg);

std::ostream& operator<<(std::ostream& out, const Texture_format& fmt);

std::wostream& operator<<(std::wostream& out, const Texture_format& fmt);

// Inferes an appropriate texture format based on the specified image format.
Texture_format texture_format(cg::data::Image_format fmt) noexcept;

// Converts the specified texture format into OpenGL internal texture format.
GLenum texture_internal_format(Texture_format fmt) noexcept;

// Inferes an appropriate format value for the glTextureSubImage call based on the specified image format.
// Returns GL_NONE if fmt value eqauls to Image_format::none.
GLenum texture_sub_image_format(cg::data::Image_format fmt) noexcept;

// Inferes an appropriate type value for the glTextureSubImage call based on the specified image format.
// Returns GL_NONE if fmt value eqauls to Image_format::none.
GLenum texture_sub_image_type(cg::data::Image_format fmt) noexcept;

} // namespace opengl
} // namespace cg

#endif // CG_OPENGL_TEXTURE_H_