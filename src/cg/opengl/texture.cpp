#include "cg/opengl/texture.h"

#include <cassert>

using cg::data::Image_2d;
using cg::data::Image_format;


namespace cg {
namespace opengl {

Texture_2d_sub_image::Texture_2d_sub_image(const Image_2d& image) noexcept
{
}

Texture_2d_sub_image::Texture_2d_sub_image(size_t mip_level, cg::uint2 offset, cg::uint2 size,
	GLenum pixel_format, GLenum pixel_type, void* pixels) noexcept :
	offset(offset), size(size), mip_level(mip_level), pixel_type(pixel_type),
	pixel_format(pixel_format), pixels(pixels)
{}

// ---- funcs ----

std::ostream& operator<<(std::ostream& out, const Texture_2d_sub_image& subimg)
{
	out << "Texture_2d_sub_image(" << subimg.offset << ", " << subimg.size << ", "
		<< subimg.mip_level << ", " << subimg.pixel_type << ", "
		<< subimg.pixel_format << ", " << subimg.pixels << ')';
	return out;
}

std::wostream& operator<<(std::wostream& out, const Texture_2d_sub_image& subimg)
{
	out << "Texture_2d_sub_image(" << subimg.offset << ", " << subimg.size << ", "
		<< subimg.mip_level << ", " << subimg.pixel_type << ", "
		<< subimg.pixel_format << ", " << subimg.pixels << ')';
	return out;
}

std::ostream& operator<<(std::ostream& out, const Texture_format& fmt)
{
	out << "Texture_format::";

	switch (fmt) {
		default:
		case Texture_format::none:
			out << "none";
			break;

		case Texture_format::red_8:
			out << "red_8";
			break;

		case Texture_format::red_32f:
			out << "red_32f";
			break;

		case Texture_format::rg_8:
			out << "rg_8";
			break;

		case Texture_format::rg_32f:
			out << "rg_32f";
			break;

		case Texture_format::rgb_8:
			out << "rgb_8";
			break;

		case Texture_format::rgb_32f:
			out << "rgb_32f";
			break;

		case Texture_format::rgba_8:
			out << "rgba_8";
			break;

		case Texture_format::rgba_32f:
			out << "rgba_32f";
			break;

		case Texture_format::depth_24:
			out << "depth_24";
			break;

		case Texture_format::depth_24_stencil_8:
			out << "depth_24_stencil_8";
			break;

		case Texture_format::depth_32:
			out << "depth_32";
			break;

		case Texture_format::depth_32f:
			out << "depth_32f";
			break;

		case Texture_format::depth_32f_stencil_8:
			out << "depth_32f_stencil_8";
			break;
	}

	return out;
}

std::wostream& operator<<(std::wostream& out, const Texture_format& fmt)
{
	out << "Texture_format::";

	switch (fmt) {
		default:
		case Texture_format::none:
			out << "none";
			break;

		case Texture_format::red_8:
			out << "red_8";
			break;

		case Texture_format::red_32f:
			out << "red_32f";
			break;

		case Texture_format::rg_8:
			out << "rg_8";
			break;

		case Texture_format::rg_32f:
			out << "rg_32f";
			break;

		case Texture_format::rgb_8:
			out << "rgb_8";
			break;

		case Texture_format::rgb_32f:
			out << "rgb_32f";
			break;

		case Texture_format::rgba_8:
			out << "rgba_8";
			break;

		case Texture_format::rgba_32f:
			out << "rgba_32f";
			break;

		case Texture_format::depth_24:
			out << "depth_24";
			break;

		case Texture_format::depth_24_stencil_8:
			out << "depth_24_stencil_8";
			break;

		case Texture_format::depth_32:
			out << "depth_32";
			break;

		case Texture_format::depth_32f:
			out << "depth_32f";
			break;

		case Texture_format::depth_32f_stencil_8:
			out << "depth_32f_stencil_8";
			break;
	}

	return out;
}

Texture_format texture_format(cg::data::Image_format fmt) noexcept
{
	switch (fmt) {
		default:
		case Image_format::none: return Texture_format::none;

		case Image_format::red_8: return Texture_format::red_8;
		case Image_format::rgb_8: return Texture_format::rgb_8;
		case Image_format::rgba_8: return Texture_format::rgba_8;
		case Image_format::bgr_8: return Texture_format::rgb_8;
		case Image_format::bgra_8: return Texture_format::rgba_8;
	}
}

} // namespace opengl
} // namespace cg
