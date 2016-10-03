#include "cg/opengl/texture.h"


using cg::greater_than;
using cg::data::Image_2d;
using cg::data::Image_format;


namespace cg {
namespace opengl {

// ----- Sampler_config -----

Sampler_config::Sampler_config(Min_filter min_filter, Mag_filter mag_filter, Wrap_mode wrap_mode) noexcept :
	Sampler_config(min_filter, mag_filter, wrap_mode, wrap_mode, wrap_mode)
{}

Sampler_config::Sampler_config(Min_filter min_filter, Mag_filter mag_filter,
	Wrap_mode wrap_u, Wrap_mode wrap_v, Wrap_mode wrap_w) noexcept :
	min_filter(get_texture_min_filter(min_filter)),
	mag_filter(get_texture_mag_filter(mag_filter)),
	wrap_u(get_texture_wrap(wrap_u)),
	wrap_v(get_texture_wrap(wrap_v)),
	wrap_w(get_texture_wrap(wrap_w))
{}

// ----- Sampler -----

Sampler::Sampler(const Sampler_config& config) noexcept
{
	glCreateSamplers(1, &_id);
	glSamplerParameteri(_id, GL_TEXTURE_MIN_FILTER, config.min_filter);
	glSamplerParameteri(_id, GL_TEXTURE_MAG_FILTER, config.mag_filter);
	glSamplerParameteri(_id, GL_TEXTURE_WRAP_S, config.wrap_u);
	glSamplerParameteri(_id, GL_TEXTURE_WRAP_T, config.wrap_v);
	glSamplerParameteri(_id, GL_TEXTURE_WRAP_R, config.wrap_w);
}

Sampler::Sampler(Sampler&& smp) noexcept :
	_id(smp._id)
{
	smp._id = Invalid::sampler_id;
}

Sampler::~Sampler() noexcept
{
	dispose();
}

Sampler& Sampler::operator==(Sampler&& smp) noexcept
{
	if (this == &smp) return *this;

	dispose();
	_id = smp._id;
	smp._id = Invalid::sampler_id;
	return *this;
}

void Sampler::dispose() noexcept
{
	if (_id == Invalid::sampler_id) return;

	glDeleteSamplers(1, &_id);
	_id = Invalid::sampler_id;
}

// ----- Texture_2d_sub_image -----

Texture_2d_sub_image_params::Texture_2d_sub_image_params(size_t mipmap_index, cg::uint2 offset, const Image_2d& image) noexcept :
	mipmap_index(mipmap_index), offset(offset), size(image.size()),
	pixel_format(get_texture_sub_image_format(image.format())),
	pixel_type(get_texture_sub_image_type(image.format())),
	pixels(static_cast<const void*>(image.data()))
{
	assert(greater_than(size, 0));
	assert(is_valid_texture_sub_image_type(pixel_type));
	assert(is_valid_texture_sub_image_format(pixel_format));
	assert(pixels != nullptr);
}

Texture_2d_sub_image_params::Texture_2d_sub_image_params(size_t mipmap_index, cg::uint2 offset, cg::uint2 size,
	GLenum pixel_format, GLenum pixel_type, const void* pixels) noexcept :
	offset(offset), size(size), mipmap_index(mipmap_index), 
	pixel_format(pixel_format), pixel_type(pixel_type), pixels(pixels)
{
	assert(greater_than(size, 0));
	assert(is_valid_texture_sub_image_type(pixel_type));
	assert(is_valid_texture_sub_image_format(pixel_format));
	assert(pixels != nullptr);
}

// ----- Texture_2d -----

Texture_2d::Texture_2d(Texture_format format, uint2 size, size_t mipmap_count) noexcept
{
	assert(greater_than(size, 0));
	assert(mipmap_count > 0);
	glCreateTextures(GL_TEXTURE_2D, 1, &_id);
	glTextureStorage2D(_id, mipmap_count, get_texture_internal_format(format), size.width, size.height);
}

Texture_2d::Texture_2d(Texture_format format, const cg::data::Image_2d& image) noexcept :
	_format(format)
{
	assert(format != Texture_format::none);
	assert(image.format() != Image_format::none);

	glCreateTextures(GL_TEXTURE_2D, 1, &_id);
	glTextureStorage2D(_id, 1, get_texture_internal_format(format), image.size().width, image.size().height);
	Texture_2d_sub_image_params sub_img(0, uint2::zero, image);
	texture_2d_sub_image(_id, sub_img);
}

Texture_2d::Texture_2d(Texture_2d&& tex) noexcept :
	_id(tex._id), _format(tex._format)
{
	tex._id = Invalid::texture_id;
	tex._format = Texture_format::none;
}

Texture_2d::~Texture_2d() noexcept
{
	dispose();
}

Texture_2d& Texture_2d::operator=(Texture_2d&& tex) noexcept
{
	if (this == &tex) return *this;

	dispose();
	_id = tex._id;
	_format = tex._format;

	tex._id = Invalid::texture_id;
	tex._format = Texture_format::none;

	return *this;
}

void Texture_2d::dispose() noexcept
{
	if (_id == Invalid::texture_id) return;

	glDeleteTextures(1, &_id);
	_id = Invalid::texture_id;
	_format = Texture_format::none;
}

// ----- funcs -----

std::ostream& operator<<(std::ostream& out, const Mag_filter& filter)
{
	out << "Mag_filter::";

	switch (filter) {
		case Mag_filter::nearest:
			out << "nearest";
			break;

		case Mag_filter::bilinear:
			out << "bilinear";
			break;
	}

	return out;
}

std::wostream& operator<<(std::wostream& out, const Mag_filter& filter)
{
	out << "Mag_filter::";

	switch (filter) {
		case Mag_filter::nearest:
			out << "nearest";
			break;

		case Mag_filter::bilinear:
			out << "bilinear";
			break;
	}

	return out;
}

std::ostream& operator<<(std::ostream& out, const Min_filter& filter)
{
	out << "Min_filter::";

	switch (filter) {
		case Min_filter::nearest:
			out << "nearest";
			break;

		case Min_filter::bilinear:
			out << "bilinear";
			break;

		case Min_filter::nearest_mipmap:
			out << "nearest_mipmap";
			break;

		case Min_filter::bilinear_mipmap:
			out << "bilinear_mipmap";
			break;

		case Min_filter::lerp_nearest_mipmaps:
			out << "lerp_nearest_mipmaps";
			break;

		case Min_filter::lerp_bilinear_mipmaps:
			out << "lerp_bilinear_mipmaps";
			break;
	}

	return out;
}

std::wostream& operator<<(std::wostream& out, const Min_filter& filter)
{
	out << "Min_filter::";

	switch (filter) {
		case Min_filter::nearest:
			out << "nearest";
			break;

		case Min_filter::bilinear:
			out << "bilinear";
			break;

		case Min_filter::nearest_mipmap:
			out << "nearest_mipmap";
			break;

		case Min_filter::bilinear_mipmap:
			out << "bilinear_mipmap";
			break;

		case Min_filter::lerp_nearest_mipmaps:
			out << "lerp_nearest_mipmaps";
			break;

		case Min_filter::lerp_bilinear_mipmaps:
			out << "lerp_bilinear_mipmaps";
			break;
	}

	return out;
}

std::ostream& operator<<(std::ostream& out, const Wrap_mode& wrap_mode)
{
	out << "Wrap_mode::";

	switch (wrap_mode) {
		case Wrap_mode::repeat:
			out << "repeat";
			break;

		case Wrap_mode::clamp_to_border:
			out << "clamp_to_border";
			break;

		case Wrap_mode::clamp_to_edge:
			out << "clamp_to_edge";
			break;

		case Wrap_mode::mirror_repeat:
			out << "mirror_repeat";
			break;

		case Wrap_mode::mirror_clamp_to_edge:
			out << "mirror_repeat";
			break;
	}

	return out;
}

std::wostream& operator<<(std::wostream& out, const Wrap_mode& wrap_mode)
{
	out << "Wrap_mode::";

	switch (wrap_mode) {
		case Wrap_mode::repeat:
			out << "repeat";
			break;

		case Wrap_mode::clamp_to_border:
			out << "clamp_to_border";
			break;

		case Wrap_mode::clamp_to_edge:
			out << "clamp_to_edge";
			break;

		case Wrap_mode::mirror_repeat:
			out << "mirror_repeat";
			break;

		case Wrap_mode::mirror_clamp_to_edge:
			out << "mirror_repeat";
			break;
	}

	return out;
}

std::ostream& operator<<(std::ostream& out, const Sampler_config& config)
{
	out << "Sampler_config(" << config.min_filter << ", "
		<< config.mag_filter << ", " << config.wrap_u << ", "
		<< config.wrap_v << ", " << config.wrap_w << ')';
	return out;
}

std::wostream& operator<<(std::wostream& out, const Sampler_config& config)
{
	out << "Sampler_config(" << config.min_filter << ", "
		<< config.mag_filter << ", " << config.wrap_u << ", "
		<< config.wrap_v << ", " << config.wrap_w << ')';
	return out;
}

std::ostream& operator<<(std::ostream& out, const Texture_2d_sub_image_params& sub_img)
{
	out << "Texture_2d_sub_image(" << sub_img.offset << ", " << sub_img.size << ", "
		<< sub_img.mipmap_index << ", " << sub_img.pixel_type << ", "
		<< sub_img.pixel_format << ", " << sub_img.pixels << ')';
	return out;
}

std::wostream& operator<<(std::wostream& out, const Texture_2d_sub_image_params& sub_img)
{
	out << "Texture_2d_sub_image(" << sub_img.offset << ", " << sub_img.size << ", "
		<< sub_img.mipmap_index << ", " << sub_img.pixel_type << ", "
		<< sub_img.pixel_format << ", " << sub_img.pixels << ')';
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

Texture_format get_texture_format(cg::data::Image_format fmt) noexcept
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

GLenum get_texture_internal_format(Texture_format fmt) noexcept
{
	switch (fmt) {
		default:
		case Texture_format::none: return GL_NONE;

		case Texture_format::red_8: return GL_R8;
		case Texture_format::red_32f: return GL_R32F;
		case Texture_format::rg_8: return GL_RG8;
		case Texture_format::rg_32f: return GL_RG32F;
		case Texture_format::rgb_8: return GL_RGB8;
		case Texture_format::rgb_32f: return GL_RGB32F;
		case Texture_format::rgba_8: return GL_RGBA8;
		case Texture_format::rgba_32f: return GL_RGBA32F;
		case Texture_format::depth_24: return GL_DEPTH_COMPONENT24;
		case Texture_format::depth_24_stencil_8: return GL_DEPTH24_STENCIL8;
		case Texture_format::depth_32: return GL_DEPTH_COMPONENT32;
		case Texture_format::depth_32f: return GL_DEPTH_COMPONENT32F;
		case Texture_format::depth_32f_stencil_8: return GL_DEPTH32F_STENCIL8;
	}
}

GLenum get_texture_mag_filter(Mag_filter filter) noexcept
{
	switch (filter) {
		case Mag_filter::nearest: return GL_NEAREST;
		case Mag_filter::bilinear: return GL_LINEAR;
	}

	assert(false);
	return GL_NONE;
}

GLenum get_texture_min_filter(Min_filter filter) noexcept
{
	switch (filter) {
		case Min_filter::nearest: return GL_NEAREST;
		case Min_filter::bilinear: return GL_LINEAR;
		case Min_filter::nearest_mipmap: return GL_NEAREST_MIPMAP_NEAREST;
		case Min_filter::bilinear_mipmap: return GL_LINEAR_MIPMAP_NEAREST;
		case Min_filter::lerp_nearest_mipmaps: return GL_NEAREST_MIPMAP_LINEAR;
		case Min_filter::lerp_bilinear_mipmaps: return GL_LINEAR_MIPMAP_LINEAR;
	}

	assert(false);
	return GL_NONE;
}

GLenum get_texture_sub_image_format(Image_format fmt) noexcept
{
	switch (fmt) {
		default:
		case Image_format::none: return GL_NONE;

		case Image_format::red_8: return GL_RED;
		case Image_format::rgb_8: return GL_RGB;
		case Image_format::rgba_8: return GL_RGBA;
		case Image_format::bgr_8: return GL_BGR;
		case Image_format::bgra_8: return GL_BGRA;
	}
}

GLenum get_texture_sub_image_type(cg::data::Image_format fmt) noexcept
{
	switch (fmt) {
		default:
		case Image_format::none: return GL_NONE;

		case Image_format::red_8: return GL_UNSIGNED_BYTE;
		case Image_format::rgb_8: return GL_UNSIGNED_BYTE;
		case Image_format::rgba_8: return GL_UNSIGNED_BYTE;
		case Image_format::bgr_8: return GL_UNSIGNED_BYTE;
		case Image_format::bgra_8: return GL_UNSIGNED_BYTE;
	}
}

GLenum get_texture_wrap(Wrap_mode wrap_mode) noexcept
{
	switch (wrap_mode) {
		case Wrap_mode::repeat: return GL_REPEAT;
		case Wrap_mode::clamp_to_border: return GL_CLAMP_TO_BORDER;
		case Wrap_mode::clamp_to_edge: return GL_CLAMP_TO_EDGE;
		case Wrap_mode::mirror_repeat: return GL_MIRRORED_REPEAT;
		case Wrap_mode::mirror_clamp_to_edge: return GL_MIRROR_CLAMP_TO_EDGE;
	}

	assert(false);
	return GL_NONE;
}

} // namespace opengl
} // namespace cg
