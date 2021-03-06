#include "cg/rnd/opengl/fbo.h"

#include <cassert>


namespace {

void validate_framebuffer(GLuint id, GLenum target)
{
	assert(id != cg::rnd::opengl::Blank::framebuffer_id);
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
	rnd_buff._id = Blank::renderbuffer_id;
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

	rnd_buff._id = Blank::renderbuffer_id;
	rnd_buff._internal_format = GL_NONE;
	rnd_buff._size = uint2::zero;
	return *this;
}

void Renderbuffer::dispose() noexcept
{
	if (_id == Blank::renderbuffer_id) return;

	glDeleteRenderbuffers(1, &_id);
	_id = Blank::renderbuffer_id;
	_internal_format = GL_NONE;
	_size = uint2::zero;
}

void Renderbuffer::reallocate_storage(GLenum internal_format, const uint2& size) noexcept
{
	assert(_id != Blank::renderbuffer_id);
	assert(is_valid_texture_internal_format(internal_format));
	assert(size >  0);

	_internal_format = internal_format;
	_size = size;

	glNamedRenderbufferStorage(_id, _internal_format, _size.x, _size.y);
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
	fb._id = Blank::framebuffer_id;
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

	fb._id = Blank::framebuffer_id;

	return *this;
}

void Framebuffer::dispose() noexcept
{
	if (_id == Blank::framebuffer_id) return;

	glDeleteFramebuffers(1, &_id);
	_id = Blank::framebuffer_id;
}

// ----- funcs -----

void attach_color_target(const Framebuffer& fbo, GLenum color_attachment, 
	const Texture_2d_i& texture, GLint mipmap_index) noexcept
{
	assert(fbo.id() != Blank::framebuffer_id);
	assert(is_valid_color_attachment(color_attachment));
	assert(texture.id() != Blank::texture_id);
	assert(mipmap_index < texture.mipmap_level_count());

	glNamedFramebufferTexture(fbo.id(), color_attachment, texture.id(), mipmap_index);
}

void attach_depth_target(const Framebuffer& fbo, const Renderbuffer& rnd_buff) noexcept
{
	assert(fbo.id() != Blank::framebuffer_id);
	assert(rnd_buff.id() != Blank::renderbuffer_id);

	glNamedFramebufferRenderbuffer(fbo.id(), GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rnd_buff.id());
}

void attach_depth_target(const Framebuffer& fbo, const Texture_2d_i& texture, GLint mipmap_index) noexcept
{
	assert(fbo.id() != Blank::framebuffer_id);
	assert(texture.id() != Blank::texture_id);
	assert(mipmap_index < texture.mipmap_level_count());

	glNamedFramebufferTexture(fbo.id(), GL_DEPTH_ATTACHMENT, texture.id(), mipmap_index);
}

void detach_color_target(const Framebuffer& fbo, GLenum color_attachment) noexcept
{
	assert(fbo.id() != Blank::framebuffer_id);
	assert(is_valid_color_attachment(color_attachment));
	glNamedFramebufferTexture(fbo.id(), color_attachment, Blank::texture_id, 0);
}

void detach_depth_target(const Framebuffer& fbo) noexcept
{
	assert(fbo.id() != Blank::framebuffer_id);
	glNamedFramebufferTexture(fbo.id(), GL_DEPTH_ATTACHMENT, Blank::texture_id, 0);
	glNamedFramebufferRenderbuffer(fbo.id(), GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, Blank::renderbuffer_id);
}

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

void set_draw_buffer(const Framebuffer& fbo, GLenum buff) noexcept
{
	assert(fbo.id() != Blank::framebuffer_id);
	assert(is_valid_color_attachment(buff) || buff == GL_NONE);

	glNamedFramebufferDrawBuffer(fbo.id(), buff);
}

void set_draw_buffers(const Framebuffer& fbo, 
	GLenum loc0, GLenum loc1, GLenum loc2, GLenum loc3,
	GLenum loc4, GLenum loc5, GLenum loc6, GLenum loc7) noexcept
{
	assert(fbo.id() != Blank::framebuffer_id);
	assert(is_valid_color_attachment(loc0) || loc0 == GL_NONE);
	assert(is_valid_color_attachment(loc1) || loc1 == GL_NONE);
	assert(is_valid_color_attachment(loc2) || loc2 == GL_NONE);
	assert(is_valid_color_attachment(loc3) || loc3 == GL_NONE);
	assert(is_valid_color_attachment(loc4) || loc4 == GL_NONE);
	assert(is_valid_color_attachment(loc5) || loc5 == GL_NONE);
	assert(is_valid_color_attachment(loc6) || loc6 == GL_NONE);
	assert(is_valid_color_attachment(loc7) || loc7 == GL_NONE);

	GLenum buffs[8] = { loc0, loc1, loc2, loc3, loc4, loc5, loc6, loc7 };
	glNamedFramebufferDrawBuffers(fbo.id(), std::extent<decltype(buffs)>::value, buffs);
}

void set_read_buffer(const Framebuffer& fbo, GLenum buff) noexcept
{
	assert(fbo.id() != Blank::framebuffer_id);
	assert(is_valid_color_attachment(buff) || buff == GL_NONE);

	glNamedFramebufferReadBuffer(fbo.id(), buff);
}

void validate(const Framebuffer& fbo)
{
	assert(fbo.id() != Blank::framebuffer_id);

	validate_framebuffer(fbo.id(), GL_DRAW_FRAMEBUFFER);
	validate_framebuffer(fbo.id(), GL_READ_FRAMEBUFFER);
}

} // namespace opengl
} // namespace rnd
} // namespace cg
