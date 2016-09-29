#ifndef CG_OPENGL_TEXTURE_H_
#define CG_OPENGL_TEXTURE_H_

#include <cassert>
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

	Texture_2d(const Texture_2d& tex) = delete;

	Texture_2d(Texture_2d&& tex) noexcept;

	~Texture_2d() noexcept;


	Texture_2d& operator=(Texture_2d&& tex) noexcept;


	// Texel format of this texture.
	Texture_format format() const noexcept
	{
		return _format;
	}

	GLuint id() const noexcept 
	{
		return _id;
	}

private:
	void dispose() noexcept;

	GLuint _id = Invalid::texture_id;
	Texture_format _format = Texture_format::none;
};

// Wrap_mode describes what texture sampler will do if texture coordinates fall outside the range [0, 1].
enum class Wrap_mode : unsigned char {
	// repeat the texture in the direction in which the texture coordinate has exceeded 1.0.
	repeat,

	// if the texture coordinate has exceeded 1.0 sample the texel from the constant border color.
	clamp_to_border,

	// forces texture coordinates out of range to be sampled along the last row or column.
	clamp_to_edge,

	// similar to repeat, but as the texture coordinate passes 1.0 it starts moving back 
	// toward the origin of the texture until it reaches 2.0, at which point the pattern repeats.
	mirror_repeat,

	// The texture coordinate is handled normaly between 0.0 and 1.0.
	// In ranges [-1.0, 0.0] and [1.0, 2.0] the texture coordinate is handled as mirror_repeat.
	// Outside the range [-1.0, 2.0] it si clamped to the edge od the texture (clamp_to_edge).
	mirror_clamp_to_edge
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

std::ostream& operator<<(std::ostream& out, const Wrap_mode& wrap_mode);

std::wostream& operator<<(std::wostream& out, const Wrap_mode& wrap_mode);

// Inferes an appropriate texture format based on the specified image format.
Texture_format get_texture_format(cg::data::Image_format fmt) noexcept;

// Converts the specified texture format into OpenGL internal texture format.
GLenum get_texture_internal_format(Texture_format fmt) noexcept;

// Inferes an appropriate format value for the glTextureSubImage call based on the specified image format.
// Returns GL_NONE if fmt value eqauls to Image_format::none.
GLenum get_texture_sub_image_format(cg::data::Image_format fmt) noexcept;

// Inferes an appropriate type value for the glTextureSubImage call based on the specified image format.
// Returns GL_NONE if fmt value eqauls to Image_format::none.
GLenum get_texture_sub_image_type(cg::data::Image_format fmt) noexcept;

// Converts the specified wrap_mode into OpenGL wrap value.
GLenum get_wrap(Wrap_mode wrap_mode) noexcept;

// Convenient function to call the glTextureSubImage2D func.
inline void texture_2d_sub_image(GLuint texture_id, const Texture_2d_sub_image sub_img) noexcept
{
	assert(texture_id != Invalid::texture_id);
	glTextureSubImage2D(texture_id, sub_img.mipmap_index,
		sub_img.offset.x, sub_img.offset.y,
		sub_img.size.width, sub_img.size.height,
		sub_img.pixel_format, sub_img.pixel_type, sub_img.pixels);
}

} // namespace opengl
} // namespace cg

#endif // CG_OPENGL_TEXTURE_H_