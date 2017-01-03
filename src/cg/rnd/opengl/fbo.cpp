#include "cg/rnd/opengl/fbo.h"

#include <cassert>


namespace {

void validate_framebuffer(GLuint id, GLenum target)
{
	assert(id != cg::rnd::opengl::Invalid::framebuffer_id);
	assert(target == GL_FRAMEBUFFER
		|| target == GL_READ_FRAMEBUFFER
		|| target == GL_DRAW_FRAMEBUFFER);

	auto status = glCheckNamedFramebufferStatus(id, target);
	switch (status) {
		case GL_FRAMEBUFFER_COMPLETE: return;

		case GL_FRAMEBUFFER_UNDEFINED: throw std::runtime_error("GL_FRAMEBUFFER_UNDEFINED");
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: throw std::runtime_error("GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT");
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: throw std::runtime_error("GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT ");
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: throw std::runtime_error("GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER");
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: throw std::runtime_error("GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER");
		case GL_FRAMEBUFFER_UNSUPPORTED: throw std::runtime_error("GL_FRAMEBUFFER_UNSUPPORTED");
		case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: throw std::runtime_error("GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE");
		case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS: throw std::runtime_error("GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS ");
	}
}

} // namespace


namespace cg {
namespace rnd {
namespace opengl {

// ----- Renderbuffer -----

Renderbuffer::Renderbuffer(GLenum internal_format, const uint2& size) noexcept
{
	glCreateRenderbuffers(1, &_id);
	reallocate_storage(internal_format, size);
}

Renderbuffer::Renderbuffer(Renderbuffer&& rnd_buff) noexcept 
	: _id(rnd_buff._id),
	_internal_format(rnd_buff._internal_format),
	_size(rnd_buff._size)
{
	rnd_buff._id = Invalid::renderbuffer_id;
	rnd_buff._internal_format = GL_NONE;
	rnd_buff._size = uint2::zero;
}

Renderbuffer::~Renderbuffer() noexcept
{
	dispose();
}

Renderbuffer& Renderbuffer::operator=(Renderbuffer&& rnd_buff) noexcept
{
	if (this == &rnd_buff) return *this;

	dispose();
	_id = rnd_buff._id;
	_internal_format = rnd_buff._internal_format;
	_size = rnd_buff._size;

	rnd_buff._id = Invalid::renderbuffer_id;
	rnd_buff._internal_format = GL_NONE;
	rnd_buff._size = uint2::zero;
	return *this;
}

void Renderbuffer::dispose() noexcept
{
	if (_id = Invalid::renderbuffer_id) return;

	glDeleteRenderbuffers(1, &_id);
	_id = Invalid::renderbuffer_id;
	_internal_format = GL_NONE;
	_size = uint2::zero;
}

void Renderbuffer::reallocate_storage(GLenum internal_format, const uint2& size) noexcept
{
	assert(_id != Invalid::renderbuffer_id);
	assert(is_valid_texture_internal_format(internal_format));
	assert(greater_than(size, 0));

	_internal_format = internal_format;
	_size = size;

	glNamedRenderbufferStorage(_id, _internal_format, _size.width, _size.height);
}

void Renderbuffer::set_size(const uint2& size) noexcept
{
	if (size == _size) return;

	reallocate_storage(_internal_format, size);
}

// ----- Framebuffer -----

Framebuffer::Framebuffer() noexcept
{
	glCreateFramebuffers(1, &_id);
}

Framebuffer::Framebuffer(Framebuffer&& fb) noexcept 
	: _id(fb._id)
{
	fb._id = Invalid::framebuffer_id;
}

Framebuffer::~Framebuffer() noexcept
{
	dispose();
}

Framebuffer& Framebuffer::operator=(Framebuffer&& fb) noexcept
{
	if (this == &fb) return *this;

	dispose();

	_id = fb._id;

	fb._id = Invalid::framebuffer_id;

	return *this;
}

void Framebuffer::attach_color_target(GLenum color_attachment, const Texture_2d_i& texture, GLuint mipmap_index) noexcept
{
	assert(_id != Invalid::framebuffer_id);
	assert(is_valid_color_attachment(color_attachment));
	assert(texture.id() != Invalid::texture_id);
	assert(mipmap_index < texture.mipmap_level_count());

	glNamedFramebufferTexture(_id, color_attachment, texture.id(), mipmap_index);
}

void Framebuffer::attach_depth_target(const Renderbuffer& rnd_buff) noexcept
{
	assert(_id != Invalid::framebuffer_id);
	assert(rnd_buff.id() != Invalid::renderbuffer_id);

	glNamedFramebufferRenderbuffer(_id, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rnd_buff.id());
}

void Framebuffer::attach_depth_target(const Texture_2d_i& texture, GLuint mipmap_index) noexcept
{
	assert(_id != Invalid::framebuffer_id);
	assert(texture.id() != Invalid::texture_id);
	assert(mipmap_index < texture.mipmap_level_count());

	glNamedFramebufferTexture(_id, GL_DEPTH_ATTACHMENT, texture.id(), mipmap_index);
}

void Framebuffer::detach_color_target(GLenum color_attachment) noexcept
{
	assert(_id != Invalid::framebuffer_id);
	assert(is_valid_color_attachment(color_attachment));
	glNamedFramebufferTexture(_id, color_attachment, Invalid::texture_id, 0);
}

void Framebuffer::detach_depth_target() noexcept
{
	assert(_id != Invalid::framebuffer_id);
	glNamedFramebufferTexture(_id, GL_DEPTH_ATTACHMENT, Invalid::texture_id, 0);
	glNamedFramebufferRenderbuffer(_id, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, Invalid::renderbuffer_id);
}

void Framebuffer::dispose() noexcept
{
	if (_id == Invalid::framebuffer_id) return;

	glDeleteFramebuffers(1, &_id);
	_id = Invalid::framebuffer_id;
}

void Framebuffer::set_draw_buffer(GLenum buff) noexcept
{
	assert(_id != Invalid::framebuffer_id);
	assert(is_valid_color_attachment(buff) || buff == GL_NONE);

	glNamedFramebufferDrawBuffer(_id, buff);
}

void Framebuffer::set_draw_buffers(GLenum loc0, GLenum loc1, GLenum loc2, GLenum loc3,
	GLenum loc4, GLenum loc5, GLenum loc6, GLenum loc7) noexcept
{
	assert(is_valid_color_attachment(loc0) || loc0 == GL_NONE);
	assert(is_valid_color_attachment(loc1) || loc1 == GL_NONE);
	assert(is_valid_color_attachment(loc2) || loc2 == GL_NONE);
	assert(is_valid_color_attachment(loc3) || loc3 == GL_NONE);
	assert(is_valid_color_attachment(loc4) || loc4 == GL_NONE);
	assert(is_valid_color_attachment(loc5) || loc5 == GL_NONE);
	assert(is_valid_color_attachment(loc6) || loc6 == GL_NONE);
	assert(is_valid_color_attachment(loc7) || loc7 == GL_NONE);

	GLenum buffs[8] = { loc0, loc1, loc2, loc3, loc4, loc5, loc6, loc7 };
	glNamedFramebufferDrawBuffers(_id, std::extent<decltype(buffs)>::value, buffs);
}

void Framebuffer::set_read_buffer(GLenum buff) noexcept
{
	assert(_id != Invalid::framebuffer_id);
	assert(is_valid_color_attachment(buff) || buff == GL_NONE);
	
	glNamedFramebufferReadBuffer(_id, buff);
}

void Framebuffer::validate() const
{
	assert(_id != Invalid::framebuffer_id);

	validate_framebuffer(_id, GL_DRAW_FRAMEBUFFER);
	validate_framebuffer(_id, GL_READ_FRAMEBUFFER);
}

// ----- funcs -----

bool is_valid_color_attachment(const GLenum& v) noexcept
{
	return (v == GL_COLOR_ATTACHMENT0)
		|| (v == GL_COLOR_ATTACHMENT1)
		|| (v == GL_COLOR_ATTACHMENT2)
		|| (v == GL_COLOR_ATTACHMENT3)
		|| (v == GL_COLOR_ATTACHMENT4)
		|| (v == GL_COLOR_ATTACHMENT5)
		|| (v == GL_COLOR_ATTACHMENT6)
		|| (v == GL_COLOR_ATTACHMENT7);
}

} // namespace opengl
} // namespace rnd
} // namespace cg
