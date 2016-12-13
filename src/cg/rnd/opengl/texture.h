#ifndef CG_RND_OPENGL_TEXTURE_H_
#define CG_RND_OPENGL_TEXTURE_H_

#include <ostream>
#include "cg/data/image.h"
#include "cg/math/math.h"
#include "cg/rnd/opengl/opengl_def.h"
#include "cg/rnd/opengl/opengl_utility.h"


namespace cg {
namespace rnd {
namespace opengl {

struct Sampler_desc final {

	Sampler_desc() noexcept = default;

	Sampler_desc(GLenum min_filter, GLenum mag_filter, GLenum wrap_mode) noexcept :
		min_filter(min_filter), mag_filter(mag_filter),
		wrap_s(wrap_mode), wrap_t(wrap_mode), wrap_r(wrap_mode)
	{}

	Sampler_desc(GLenum min_filter, GLenum mag_filter,
		GLenum wrap_s, GLenum wrap_t, GLenum wrap_r) noexcept :
		min_filter(min_filter), mag_filter(mag_filter), 
		wrap_s(wrap_s), wrap_t(wrap_t), wrap_r(wrap_r)
	{}

	// Minifying filter is used whenever the pixel being textured maps to an area greater than one texture's texel. 
	GLenum min_filter = GL_LINEAR;

	// Magnification filter is used when the pixel being textured maps to an area less than or equal to one texture's texel.
	GLenum mag_filter = GL_LINEAR;

	// Wrap mod for texture coordinate u. It's called 'u' sometimes.
	GLenum wrap_s = GL_CLAMP_TO_EDGE;

	// Wrap mod for texture coordinate v. It's called 'v' sometimes.
	GLenum wrap_t = GL_CLAMP_TO_EDGE;

	// Wrap mod for texture coordinate w. It's called 'w' sometimes.
	GLenum wrap_r = GL_CLAMP_TO_EDGE;
};

// Incapsulates the sampler state of a texture object.
class Sampler final {
public:

	Sampler() noexcept = default;

	explicit Sampler(const Sampler_desc& desc) noexcept;

	Sampler(const Sampler&) = delete;

	Sampler(Sampler&& smp) noexcept;

	~Sampler() noexcept;


	Sampler& operator=(const Sampler&) = delete;

	Sampler& operator=(Sampler&& smp) noexcept;


	GLuint id() const noexcept
	{
		return _id;
	}

private:

	void dispose() noexcept;

	GLuint _id = Invalid::sampler_id;
};

class Texture_2d_i {
public:

	virtual ~Texture_2d_i() noexcept {}


	// Texture's unique id.
	virtual GLuint id() const noexcept = 0;

	// Sized texel format that is used to store texture data.
	virtual GLenum internal_format() const noexcept = 0;

	// How many mipmaps the texture has.
	virtual size_t mipmap_level_count() const noexcept = 0;

	// Texture's size in texels.
	virtual uint2 size() const noexcept = 0;
};

class Texture_2d final : public virtual Texture_2d_i {
public:

	Texture_2d() noexcept = default;

	Texture_2d(GLenum internal_format, size_t mipmap_level_count, const uint2& size) noexcept;

	Texture_2d(GLenum internal_format, size_t mipmap_level_count,
		const Sampler_desc& sampler_desc, const uint2& size) noexcept;

	Texture_2d(GLenum internal_format, size_t mipmap_level_count,
		cg::data::Image_2d& image) noexcept;

	Texture_2d(GLenum internal_format, size_t mipmap_level_count,
		const Sampler_desc& sampler_desc, const cg::data::Image_2d& image) noexcept;

	Texture_2d(const Texture_2d&) = delete;

	Texture_2d(Texture_2d&& tex) noexcept;

	~Texture_2d() noexcept;


	Texture_2d& operator=(const Texture_2d&) = delete;

	Texture_2d& operator=(Texture_2d&& tex) noexcept;


	// Texture's unique id.
	GLuint id() const noexcept override
	{
		return _id;
	}

	// Sized texel format that is used to store texture data.
	GLenum internal_format() const noexcept override
	{
		return _internal_format;
	}

	// How many mipmaps the texture has.
	size_t mipmap_level_count() const noexcept override
	{
		return _mipmap_level_count;
	}

	// Texture's size in texels.
	uint2 size() const noexcept override
	{
		return _size;
	}


	void write(size_t mipmap_level, const uint2& offset, const cg::data::Image_2d& image) noexcept;

private:

	void dispose() noexcept;

	GLuint _id = Invalid::texture_id;
	GLenum _internal_format = GL_NONE;
	size_t _mipmap_level_count = 0;
	uint2 _size;
};


inline bool operator==(const Sampler_desc& lhs, const Sampler_desc& rhs) noexcept
{
	return (lhs.min_filter == rhs.min_filter)
		&& (lhs.mag_filter == rhs.mag_filter)
		&& (lhs.wrap_s == rhs.wrap_s)
		&& (lhs.wrap_t == rhs.wrap_t)
		&& (lhs.wrap_r == rhs.wrap_r);
}

inline bool operator!=(const Sampler_desc& lhs, const Sampler_desc& rhs) noexcept
{
	return !(lhs == rhs);
}

std::ostream& operator<<(std::ostream& out, const Sampler_desc& desc);

std::wostream& operator<<(std::wostream& out, const Sampler_desc& desc);

// Inferes an appropriate format value for the glTexImage/glTexSubImage/glTextureSubImage call 
// based on the specified image format.
// Returns GL_NONE if fmt value eqauls to Image_format::none.
GLenum get_texture_sub_image_format(cg::data::Image_format fmt) noexcept;

// Inferes an appropriate format value for the glTexImage/glTexSubImage/glTextureSubImage 
// based on the specified texture internal format.
// Returns GL_NONE if internal_format value is not a valid value.
GLenum get_texture_sub_image_format(GLenum internal_format) noexcept;

// Inferes an appropriate type value for the glTexImage/glTexSubImage/glTextureSubImage call 
// based on the specified image format.
// Returns GL_NONE if fmt value eqauls to Image_format::none.
GLenum get_texture_sub_image_type(cg::data::Image_format fmt) noexcept;

// Inferes an appropriate type value for the glTexImage/glTexSubImage/glTextureSubImage call 
// based on the specified texture internal format.
// Returns GL_NONE if internal_format value is not a valid value.
GLenum get_texture_sub_image_type(GLenum internal_format) noexcept;

// Validates glTexImage/glTexStorage/glTextureStorage 'internalformat' argument value.
bool is_valid_texture_internal_format(GLenum value) noexcept;

//  Validates sampler/texture GL_TEXTURE_MAG_FILTER parameter value.
bool is_valid_texture_mag_filter(GLenum value) noexcept;

//  Validates sampler/texture GL_TEXTURE_MIN_FILTER parameter value.
bool is_valid_texture_min_filter(GLenum value) noexcept;

// Validates sampler/texture WRAP_{S/T/R} parameter value.
bool is_valid_texture_wrap_mode(GLenum value) noexcept;


} // namespace opengl
} // namespace rnd
} // namespace cg

#endif // CG_RND_OPENGL_TEXTURE_H_
