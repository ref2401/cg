#ifndef CG_RND_OPENGL_TEXTURE_H_
#define CG_RND_OPENGL_TEXTURE_H_

#include <ostream>
#include <utility>
#include "cg/data/image.h"
#include "cg/math/math.h"
#include "cg/rnd/opengl/buffer.h"
#include "cg/rnd/opengl/opengl_def.h"
#include "cg/rnd/opengl/opengl_utility.h"


namespace cg {
namespace rnd {
namespace opengl {

struct Sampler_desc final {

	Sampler_desc() noexcept = default;

	Sampler_desc(GLenum mag_filter, GLenum min_filter, GLenum wrap_mode) noexcept;

	Sampler_desc(GLenum mag_filter, GLenum min_filter,
		GLenum wrap_s, GLenum wrap_t, GLenum wrap_r) noexcept;


	// Magnification filter is used when the pixel being textured maps to an area less than or equal to one texture's texel.
	GLenum mag_filter = GL_LINEAR;

	// Minifying filter is used whenever the pixel being textured maps to an area greater than one texture's texel. 
	GLenum min_filter = GL_LINEAR;

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
	virtual GLuint mipmap_level_count() const noexcept = 0;

	// Texture's size in texels.
	virtual uint2 size() const noexcept = 0;
};

// Texture_2d represents a 2D texture with mutable storage.
class Texture_2d final : public Texture_2d_i {
public:

	Texture_2d() noexcept = default;

	Texture_2d(GLenum internal_format, GLuint mipmap_level_count, const uint2& size) noexcept;

	Texture_2d(GLenum internal_format, GLuint mipmap_level_count,
		const Sampler_desc& sampler_desc, const uint2& size) noexcept;

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

	// Resizes the texture object. Preserves texture's format and mipmap count.
	void set_size(const uint2& size) noexcept;

private:

	void dispose() noexcept;

	// Reallocates a new mutable storage for this texture object.
	// Previous texture's contents will be lost.
	void reallocate_storage(GLenum internal_format, GLuint mipmap_level_count, uint2 size) noexcept;

	GLuint _id = Invalid::texture_id;
	GLenum _internal_format = GL_NONE;
	GLuint _mipmap_level_count = 0;
	uint2 _size;
};

// Texture_2d_immut represents a 2D texture with immutable storage.
class Texture_2d_immut final : public virtual Texture_2d_i {
public:

	Texture_2d_immut() noexcept = default;

	Texture_2d_immut(GLenum internal_format, GLuint mipmap_level_count, const uint2& size) noexcept;

	Texture_2d_immut(GLenum internal_format, GLuint mipmap_level_count,
		const Sampler_desc& sampler_desc, const uint2& size) noexcept;

	Texture_2d_immut(GLenum internal_format, GLuint mipmap_level_count,
		const Sampler_desc& sampler_desc, const cg::data::Image_2d& image) noexcept;

	Texture_2d_immut(const Texture_2d_immut&) = delete;

	Texture_2d_immut(Texture_2d_immut&& tex) noexcept;

	~Texture_2d_immut() noexcept;


	Texture_2d_immut& operator=(const Texture_2d_immut&) = delete;

	Texture_2d_immut& operator=(Texture_2d_immut&& tex) noexcept;


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


	void write(GLuint mipmap_level, const uint2& offset, const cg::data::Image_2d& image) noexcept;

private:

	void dispose() noexcept;

	GLuint _id = Invalid::texture_id;
	GLenum _internal_format = GL_NONE;
	GLuint _mipmap_level_count = 0;
	uint2 _size;
};

class Texture_3d_i {
public:

	virtual ~Texture_3d_i() noexcept {}


	// Texture's unique id.
	virtual GLuint id() const noexcept = 0;

	// Sized texel format that is used to store texture data.
	virtual GLenum internal_format() const noexcept = 0;

	// How many mipmaps the texture has.
	virtual GLuint mipmap_level_count() const noexcept = 0;

	// Texture's size in texels.
	virtual uint3 size() const noexcept = 0;
};

// Texture_3d_immut represents a 3D texture with immutable storage.
class Texture_3d_immut final : Texture_3d_i {
public:

	Texture_3d_immut() noexcept = default;

	Texture_3d_immut(GLenum internal_format, GLuint mipmap_level_count, const uint3& size) noexcept;

	Texture_3d_immut(GLenum internal_format, GLuint mipmap_level_count,
		const Sampler_desc& sampler_desc, const uint3& size) noexcept;

	Texture_3d_immut(const Texture_3d_immut&) = delete;

	Texture_3d_immut(Texture_3d_immut&& tex) noexcept;

	~Texture_3d_immut() noexcept;


	Texture_3d_immut& operator=(const Texture_3d_immut&) = delete;

	Texture_3d_immut& operator=(Texture_3d_immut&& tex) noexcept;


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
	GLuint mipmap_level_count() const noexcept override
	{
		return _mipmap_level_count;
	}

	// Texture's size in texels.
	uint3 size() const noexcept override
	{
		return _size;
	}

	void write(GLuint mipmap_level, const uint3& offset, const cg::data::Image_2d& image) noexcept;

private:

	void dispose() noexcept;

	GLuint _id = Invalid::texture_id;
	GLenum _internal_format = GL_NONE;
	GLuint _mipmap_level_count = 0;
	uint3 _size;
};

// A texture buffer is a one-dimensional texture whose storage comes from a buffer object.
template<typename Buffer>
class Texture_buffer final {
public:

	static_assert(is_opengl_buffer_v<Buffer>, "Buffer must be of the OpenGL buffer types.");


	Texture_buffer() noexcept = default;

	template<typename T>
	Texture_buffer(GLenum internal_format, const std::vector<T>& vector) noexcept;

	Texture_buffer(GLenum internal_format, size_t byte_count, const void* data_ptr = nullptr) noexcept;

	Texture_buffer(const Texture_buffer&) = delete;

	Texture_buffer(Texture_buffer&& tb) noexcept;


	Texture_buffer& operator=(const Texture_buffer&) = delete;

	Texture_buffer& operator=(Texture_buffer&& tb) noexcept;


	// Buffer object which represents a data storage.
	Buffer_i& buffer() noexcept
	{
		return _buffer;
	}

	// Texture's unique id.
	GLuint id() const noexcept
	{
		return _id;
	}

	// Represent format of the data is store belonging to buffer.
	GLenum internal_format() const noexcept
	{
		return _internal_format;
	}

private:

	void dispose() noexcept;

	GLuint _id = Invalid::texture_id;
	GLenum _internal_format = GL_NONE;
	Buffer _buffer;
};

template<typename Buffer>
template<typename T>
Texture_buffer<Buffer>::Texture_buffer(GLenum internal_format, 
	const std::vector<T>& vector) noexcept
	: Texture_buffer(internal_format, cg::byte_count(vector), vector.data())
{}

template<typename Buffer>
Texture_buffer<Buffer>::Texture_buffer(GLenum internal_format,
	size_t byte_count, const void* data_ptr = nullptr) noexcept
	: _internal_format(internal_format),
	_buffer(byte_count, data_ptr)
{
	assert(is_valid_texture_buffer_internal_format(internal_format));

	glCreateTextures(GL_TEXTURE_BUFFER, 1, &_id);
	glTextureBuffer(_id, _internal_format, _buffer.id());
}

template<typename Buffer>
Texture_buffer<Buffer>::Texture_buffer(Texture_buffer&& tb) noexcept
	: _id(tb._id),
	_internal_format(tb._internal_format),
	_buffer(std::move(tb._buffer))
{
	tb._id = Invalid::texture_id;
	tb._internal_format = GL_NONE;
}

template<typename Buffer>
void Texture_buffer<Buffer>::dispose() noexcept
{
	if (_id == Invalid::texture_id) return;

	glDeleteTextures(1, &_id);
	_id = Invalid::texture_id;
	_internal_format = GL_NONE;
	_buffer.~Buffer();
}

template<typename Buffer>
Texture_buffer<Buffer>& Texture_buffer<Buffer>::operator=(Texture_buffer&& tb) noexcept
{
	if (this == &tb) return *this;

	dispose();
	
	_id = tb._id;
	_internal_format = tb._internal_format;
	_buffer = std::move(tb._buffer);

	tb._id = Invalid::texture_id;
	tb._internal_format = GL_NONE;

	return *this;
}


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

// Infers an appropriate format value for the glTexImage/glTexSubImage/glTextureSubImage call 
// based on the specified image format.
// Returns GL_NONE if fmt value eqauls to Pixel_format::none.
GLenum get_texture_sub_image_format(cg::data::Pixel_format fmt) noexcept;

// Infers an appropriate format value for the glTexImage/glTexSubImage/glTextureSubImage 
// based on the specified texture internal format.
// Returns GL_NONE if internal_format value is not a valid value.
GLenum get_texture_sub_image_format(GLenum internal_format) noexcept;

// Infers an appropriate type value for the glTexImage/glTexSubImage/glTextureSubImage call 
// based on the specified image format.
// Returns GL_NONE if fmt value eqauls to Pixel_format::none.
GLenum get_texture_sub_image_type(cg::data::Pixel_format fmt) noexcept;

// Infers an appropriate type value for the glTexImage/glTexSubImage/glTextureSubImage call 
// based on the specified texture internal format.
// Returns GL_NONE if internal_format value is not a valid value.
GLenum get_texture_sub_image_type(GLenum internal_format) noexcept;

// Validates glTexImage/glTexStorage/glTextureStorage 'internalformat' argument value.
bool is_valid_texture_internal_format(GLenum value) noexcept;

// Validates glTexBuffer/glTextureBuffer 'internalFormat' argument value.
bool is_valid_texture_buffer_internal_format(GLenum value) noexcept;

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
