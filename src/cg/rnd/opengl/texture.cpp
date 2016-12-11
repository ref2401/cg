#include "cg/rnd/opengl/texture.h"

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

bool is_valid_texture_internal_format(const GLenum& value) noexcept
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

bool is_valid_texture_mag_filter(const GLenum& value) noexcept
{
	return (value == GL_NEAREST)
		|| (value == GL_LINEAR);
}

bool is_valid_texture_min_filter(const GLenum& value) noexcept
{
	return (value == GL_NEAREST)
		|| (value == GL_LINEAR)
		|| (value == GL_NEAREST_MIPMAP_NEAREST)
		|| (value == GL_LINEAR_MIPMAP_NEAREST)
		|| (value == GL_NEAREST_MIPMAP_LINEAR)
		|| (value == GL_LINEAR_MIPMAP_LINEAR);
}

bool is_valid_texture_wrap_mode(const GLenum& value) noexcept
{
	return (value == GL_REPEAT)
		|| (value == GL_CLAMP_TO_BORDER)
		|| (value == GL_CLAMP_TO_EDGE)
		|| (value == GL_MIRRORED_REPEAT)
		|| (value == GL_MIRROR_CLAMP_TO_EDGE);
}

} // namespace opengl
} // namespace rnd
} // namespace cg
