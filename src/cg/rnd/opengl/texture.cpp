#include "cg/rnd/opengl/texture.h"

using cg::data::Image_format;


namespace cg {
namespace rnd {
namespace opengl {

// ----- Sampler -----

Sampler::Sampler(const Sampler_desc& desc) noexcept
{
	glCreateSamplers(1, &_id);
	glSamplerParameteri(_id, GL_TEXTURE_MIN_FILTER, desc.min_filter);
	glSamplerParameteri(_id, GL_TEXTURE_MAG_FILTER, desc.mag_filter);
	glSamplerParameteri(_id, GL_TEXTURE_WRAP_S, desc.wrap_s);
	glSamplerParameteri(_id, GL_TEXTURE_WRAP_T, desc.wrap_t);
	glSamplerParameteri(_id, GL_TEXTURE_WRAP_R, desc.wrap_r);
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

Sampler& Sampler::operator=(Sampler&& smp) noexcept
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

// ----- funcs -----

std::ostream& operator<<(std::ostream& out, const Sampler_desc& desc)
{
	out << "Sampler_desc(" << desc.min_filter << ", " << desc.mag_filter << ", " 
		<< desc.wrap_s << ", " << desc.wrap_t << ", " << desc.wrap_r << ')';
	return out;
}

std::wostream& operator<<(std::wostream& out, const Sampler_desc& desc)
{
	out << "Sampler_desc(" << desc.min_filter << ", " << desc.mag_filter << ", "
		<< desc.wrap_s << ", " << desc.wrap_t << ", " << desc.wrap_r << ')';
	return out;
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

GLenum get_texture_sub_image_format(GLenum internal_format) noexcept
{
	switch (internal_format) {
		default:					return GL_NONE;

		case GL_R8:					return GL_RED;
		case GL_R32F:				return GL_RED;
		case GL_RG8:				return GL_RG;
		case GL_RG32F:				return GL_RG;
		case GL_RGB8:				return GL_RGB;
		case GL_RGB32F:				return GL_RGB;
		case GL_RGBA8:				return GL_RGBA;
		case GL_RGBA32F:			return GL_RGBA;
		case GL_DEPTH_COMPONENT24:	return GL_DEPTH_COMPONENT;
		case GL_DEPTH_COMPONENT32:	return GL_DEPTH_COMPONENT;
		case GL_DEPTH_COMPONENT32F:	return GL_DEPTH_COMPONENT;
		case GL_DEPTH24_STENCIL8:	return GL_DEPTH_STENCIL;
		case GL_DEPTH32F_STENCIL8:	return GL_DEPTH_STENCIL;
	}
}

GLenum get_texture_sub_image_type(Image_format fmt) noexcept
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

GLenum get_texture_sub_image_type(GLenum internal_format) noexcept
{
	switch (internal_format) {
		default:					return GL_NONE;

		case GL_R8:					return GL_UNSIGNED_BYTE;
		case GL_R32F:				return GL_FLOAT;
		case GL_RG8:				return GL_UNSIGNED_BYTE;
		case GL_RG32F:				return GL_FLOAT;
		case GL_RGB8:				return GL_UNSIGNED_BYTE;
		case GL_RGB32F:				return GL_FLOAT;
		case GL_RGBA8:				return GL_UNSIGNED_BYTE;
		case GL_RGBA32F:			return GL_FLOAT;
		case GL_DEPTH_COMPONENT24:	return GL_UNSIGNED_INT;
		case GL_DEPTH_COMPONENT32:	return GL_UNSIGNED_INT;
		case GL_DEPTH_COMPONENT32F:	return GL_FLOAT;
		case GL_DEPTH24_STENCIL8:	return GL_UNSIGNED_INT_24_8;
		case GL_DEPTH32F_STENCIL8:	return GL_FLOAT_32_UNSIGNED_INT_24_8_REV;
	}
}

bool is_valid_texture_internal_format(GLenum value) noexcept
{
	return (value == GL_R8)
		|| (value == GL_R32F)
		|| (value == GL_RG8)
		|| (value == GL_RG32F)
		|| (value == GL_RGB8)
		|| (value == GL_RGB32F)
		|| (value == GL_RGBA8)
		|| (value == GL_RGBA32F)
		|| (value == GL_DEPTH_COMPONENT24)
		|| (value == GL_DEPTH24_STENCIL8)
		|| (value == GL_DEPTH32F_STENCIL8)
		|| (value == GL_DEPTH_COMPONENT32)
		|| (value == GL_DEPTH_COMPONENT32F);
}

bool is_valid_texture_mag_filter(GLenum value) noexcept
{
	return (value == GL_NEAREST)
		|| (value == GL_LINEAR);
}

bool is_valid_texture_min_filter(GLenum value) noexcept
{
	return (value == GL_NEAREST)
		|| (value == GL_LINEAR)
		|| (value == GL_NEAREST_MIPMAP_NEAREST)
		|| (value == GL_LINEAR_MIPMAP_NEAREST)
		|| (value == GL_NEAREST_MIPMAP_LINEAR)
		|| (value == GL_LINEAR_MIPMAP_LINEAR);
}

bool is_valid_texture_wrap_mode(GLenum value) noexcept
{
	return (value == GL_REPEAT)
		|| (value == GL_CLAMP_TO_BORDER)
		|| (value == GL_CLAMP_TO_EDGE)
		|| (value == GL_MIRRORED_REPEAT)
		|| (value == GL_MIRROR_CLAMP_TO_EDGE);
}

void texture_2d_sub_image(GLuint tex_id, size_t mipmap_level, const uint2& offset, 
	const cg::data::Image_2d& image) noexcept
{
	assert(tex_id != Invalid::texture_id);

}

} // namespace opengl
} // namespace rnd
} // namespace cg
