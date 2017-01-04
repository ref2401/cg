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

// ----- Texture_2d -----

Texture_2d::Texture_2d(GLenum internal_format, GLuint mipmap_level_count, const uint2& size) noexcept
{
	glGenTextures(1, &_id);
	reallocate_storage(internal_format, mipmap_level_count, size);
}

Texture_2d::Texture_2d(GLenum internal_format, GLuint mipmap_level_count,
	const Sampler_desc& sampler_desc, const uint2& size) noexcept
	: Texture_2d(internal_format, mipmap_level_count, size)
{
	glTextureParameteri(_id, GL_TEXTURE_MIN_FILTER, sampler_desc.min_filter);
	glTextureParameteri(_id, GL_TEXTURE_MAG_FILTER, sampler_desc.mag_filter);
	glTextureParameteri(_id, GL_TEXTURE_WRAP_S, sampler_desc.wrap_s);
	glTextureParameteri(_id, GL_TEXTURE_WRAP_T, sampler_desc.wrap_t);
	glTextureParameteri(_id, GL_TEXTURE_WRAP_R, sampler_desc.wrap_r);
}

Texture_2d::Texture_2d(Texture_2d&& tex) noexcept :
	_id(tex._id),
	_internal_format(tex._internal_format),
	_mipmap_level_count(tex._mipmap_level_count),
	_size(tex._size)
{
	tex._id = Invalid::texture_id;
	tex._internal_format = GL_NONE;
	tex._mipmap_level_count = 0;
	tex._size = uint2::zero;
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
	_internal_format = tex._internal_format;
	_mipmap_level_count = tex._mipmap_level_count;
	_size = tex._size;

	tex._id = Invalid::texture_id;
	tex._internal_format = GL_NONE;
	tex._mipmap_level_count = 0;
	tex._size = uint2::zero;

	return *this;
}

void Texture_2d::dispose() noexcept
{
	if (_id == Invalid::texture_id) return;

	glDeleteTextures(1, &_id);
	_id = Invalid::texture_id;
	_internal_format = GL_NONE;
	_mipmap_level_count = 0;
	_size = uint2::zero;
}

void Texture_2d::reallocate_storage(GLenum internal_format, GLuint mipmap_level_count, uint2 size) noexcept
{
	assert(_id != Invalid::texture_id);
	assert(is_valid_texture_internal_format(internal_format));
	assert(greater_than(size, 0));
	assert(mipmap_level_count > 0);

	_internal_format = internal_format;
	_size = size;
	_mipmap_level_count = mipmap_level_count;
	assert(_mipmap_level_count == 1); // > 1 not implemented

	glBindTexture(GL_TEXTURE_2D, _id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_LOD, 0.f);

	glTexImage2D(GL_TEXTURE_2D, 0, _internal_format,
		_size.width, _size.height, 0,
		get_texture_sub_image_format(_internal_format),
		get_texture_sub_image_type(_internal_format),
		nullptr);

	glBindTexture(GL_TEXTURE_2D, Invalid::texture_id);
}

void Texture_2d::set_size(const uint2& size) noexcept
{
	if (size == _size) return;

	reallocate_storage(_internal_format, _mipmap_level_count, size);
}

// ----- Texture_2d_immut -----

Texture_2d_immut::Texture_2d_immut(GLenum internal_format, GLuint mipmap_level_count, const uint2& size) noexcept
	: _internal_format(internal_format),
	_mipmap_level_count(mipmap_level_count),
	_size(size)
{
	assert(is_valid_texture_internal_format(internal_format));
	assert(mipmap_level_count > 0);
	assert(greater_than(size, 0));

	glCreateTextures(GL_TEXTURE_2D, 1, &_id);
	glTextureStorage2D(_id, _mipmap_level_count, _internal_format, _size.width, _size.height);
	glTextureParameteri(_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

Texture_2d_immut::Texture_2d_immut(GLenum internal_format, GLuint mipmap_level_count,
	const Sampler_desc& sampler_desc, const uint2& size) noexcept 
	: Texture_2d_immut(internal_format, mipmap_level_count, size)
{
	glTextureParameteri(_id, GL_TEXTURE_MIN_FILTER, sampler_desc.min_filter);
	glTextureParameteri(_id, GL_TEXTURE_MAG_FILTER, sampler_desc.mag_filter);
	glTextureParameteri(_id, GL_TEXTURE_WRAP_S, sampler_desc.wrap_s);
	glTextureParameteri(_id, GL_TEXTURE_WRAP_T, sampler_desc.wrap_t);
	glTextureParameteri(_id, GL_TEXTURE_WRAP_R, sampler_desc.wrap_r);
}

Texture_2d_immut::Texture_2d_immut(GLenum internal_format, GLuint mipmap_level_count,
	cg::data::Image_2d& image) noexcept
	: Texture_2d_immut(internal_format, mipmap_level_count, image.size())
{
	assert(image.format() != Image_format::none);
	write(0, uint2::zero, image);
}

Texture_2d_immut::Texture_2d_immut(GLenum internal_format, GLuint mipmap_level_count,
	const Sampler_desc& sampler_desc, const cg::data::Image_2d& image) noexcept
	: Texture_2d_immut(internal_format, mipmap_level_count, sampler_desc, image.size())
{
	assert(image.format() != Image_format::none);
	write(0, uint2::zero, image);
}

Texture_2d_immut::Texture_2d_immut(Texture_2d_immut&& tex) noexcept :
	_id(tex._id),
	_internal_format(tex._internal_format),
	_mipmap_level_count(tex._mipmap_level_count),
	_size(tex._size)
{
	tex._id = Invalid::texture_id;
	tex._internal_format = GL_NONE;
	tex._mipmap_level_count = 0;
	tex._size = uint2::zero;
}

Texture_2d_immut::~Texture_2d_immut() noexcept
{
	dispose();
}

Texture_2d_immut& Texture_2d_immut::operator=(Texture_2d_immut&& tex) noexcept
{
	if (this == &tex) return *this;

	dispose();
	_id = tex._id;
	_internal_format = tex._internal_format;
	_mipmap_level_count = tex._mipmap_level_count;
	_size = tex._size;

	tex._id = Invalid::texture_id;
	tex._internal_format = GL_NONE;
	tex._mipmap_level_count = 0;
	tex._size = uint2::zero;

	return *this;
}

void Texture_2d_immut::dispose() noexcept
{
	if (_id == Invalid::texture_id) return;

	glDeleteTextures(1, &_id);
	_id = Invalid::texture_id;
	_internal_format = GL_NONE;
}

void Texture_2d_immut::write(GLuint mipmap_level, const uint2& offset, const cg::data::Image_2d& image) noexcept
{
	assert(_id != Invalid::texture_id);
	assert(mipmap_level < _mipmap_level_count);
	assert(image.format() != Image_format::none);
	// TODO(ref2401): check offset + image.size() for the specified mipmap_level
	assert(_size.width >= offset.width + image.size().width);
	assert(_size.height >= offset.height + image.size().height);

	if (image.size() == uint2::zero) return;

	glTextureSubImage2D(_id, mipmap_level, offset.x , offset.y, 
		image.size().width, image.size().height,
		get_texture_sub_image_format(image.format()),
		get_texture_sub_image_type(image.format()),
		image.data());
}

// ----- Texture_3d_immut -----

Texture_3d_immut::Texture_3d_immut(GLenum internal_format, GLuint mipmap_level_count, const uint3& size) noexcept
	: _internal_format(internal_format),
	_mipmap_level_count(mipmap_level_count),
	_size(size)
{
	assert(is_valid_texture_internal_format(internal_format));
	assert(mipmap_level_count > 0);
	assert(greater_than(size, 0));

	glCreateTextures(GL_TEXTURE_3D, 1, &_id);
	glTextureStorage3D(_id, _mipmap_level_count, _internal_format, 
		_size.width, _size.height, _size.depth);
}

Texture_3d_immut::Texture_3d_immut(GLenum internal_format, GLuint mipmap_level_count,
	const Sampler_desc& sampler_desc, const uint3& size) noexcept
	: Texture_3d_immut(internal_format, mipmap_level_count, size)
{
	glTextureParameteri(_id, GL_TEXTURE_MIN_FILTER, sampler_desc.min_filter);
	glTextureParameteri(_id, GL_TEXTURE_MAG_FILTER, sampler_desc.mag_filter);
	glTextureParameteri(_id, GL_TEXTURE_WRAP_S, sampler_desc.wrap_s);
	glTextureParameteri(_id, GL_TEXTURE_WRAP_T, sampler_desc.wrap_t);
	glTextureParameteri(_id, GL_TEXTURE_WRAP_R, sampler_desc.wrap_r);
}

Texture_3d_immut::Texture_3d_immut(Texture_3d_immut&& tex) noexcept :
	_id(tex._id),
	_internal_format(tex._internal_format),
	_mipmap_level_count(tex._mipmap_level_count),
	_size(tex._size)
{
	tex._id = Invalid::texture_id;
	tex._internal_format = GL_NONE;
	tex._mipmap_level_count = 0;
	tex._size = uint3::zero;
}

Texture_3d_immut::~Texture_3d_immut() noexcept
{
	dispose();
}

Texture_3d_immut& Texture_3d_immut::operator=(Texture_3d_immut&& tex) noexcept
{
	if (this == &tex) return *this;

	dispose();
	_id = tex._id;
	_internal_format = tex._internal_format;
	_mipmap_level_count = tex._mipmap_level_count;
	_size = tex._size;

	tex._id = Invalid::texture_id;
	tex._internal_format = GL_NONE;
	tex._mipmap_level_count = 0;
	tex._size = uint3::zero;

	return *this;
}

void Texture_3d_immut::dispose() noexcept
{
	if (_id == Invalid::texture_id) return;

	glDeleteTextures(1, &_id);
	_id = Invalid::texture_id;
	_internal_format = GL_NONE;
}

void Texture_3d_immut::write(GLuint mipmap_level, const uint3& offset, const cg::data::Image_2d& image) noexcept
{
	assert(_id != Invalid::texture_id);
	assert(mipmap_level < _mipmap_level_count);
	assert(image.format() != Image_format::none);
	// TODO(ref2401): check offset + image.size() for the specified mipmap_level
	assert(_size.width >= offset.width + image.size().width);
	assert(_size.height >= offset.height + image.size().height);

	if (image.size() == uint2::zero) return;

	glTextureSubImage3D(_id, mipmap_level, offset.x, offset.y, offset.z,
		image.size().width, image.size().height, 1,
		get_texture_sub_image_format(image.format()),
		get_texture_sub_image_type(image.format()),
		image.data());
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

//void texture_2d_sub_image(GLuint tex_id, size_t mipmap_level, const uint2& offset, 
//	const cg::data::Image_2d& image) noexcept
//{
//	assert(tex_id != Invalid::texture_id);
//
//}

} // namespace opengl
} // namespace rnd
} // namespace cg
