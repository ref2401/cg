#include "cg/rnd/opengl/fbo.h"

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

//Framebuffer::Framebuffer() noexcept
//{
//	glCreateFramebuffers(1, &_id);
//}
//
//Framebuffer::Framebuffer(Framebuffer&& fb) noexcept :
//_id(fb._id)
//{
//	fb._id = Invalid::framebuffer_id;
//}
//
//Framebuffer::~Framebuffer() noexcept
//{
//	dispose();
//}
//
//Framebuffer& Framebuffer::operator=(Framebuffer&& fb) noexcept
//{
//	if (this == &fb) return *this;
//
//	dispose();
//	_id = fb._id;
//	fb._id = Invalid::framebuffer_id;
//
//	return *this;
//}
//
//void Framebuffer::attach_color_texture(GLenum color_attachment, const Texture_2d& texture, size_t mipmap_index) noexcept
//{
//	assert(_id != Invalid::framebuffer_id);
//	assert(is_valid_color_attachment(color_attachment) && color_attachment != GL_NONE);
//	assert(texture.id() != Invalid::texture_id);
//
//	glNamedFramebufferTexture(_id, color_attachment, texture.id(), mipmap_index);
//}
//
//void Framebuffer::attach_color_texture(GLenum color_attachment,
//	const Texture_2d_immut& texture, size_t mipmap_index) noexcept
//{
//	assert(_id != Invalid::framebuffer_id);
//	assert(is_valid_color_attachment(color_attachment) && color_attachment != GL_NONE);
//	assert(texture.id() != Invalid::texture_id);
//
//	glNamedFramebufferTexture(_id, color_attachment, texture.id(), mipmap_index);
//}
//
//void Framebuffer::attach_depth_renderbuffer(const Renderbuffer& rnd_buff) noexcept
//{
//	assert(_id != Invalid::framebuffer_id);
//	assert(rnd_buff.id() != Invalid::renderbuffer_id);
//
//	glNamedFramebufferRenderbuffer(_id, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rnd_buff.id());
//}
//
//void Framebuffer::attach_depth_texture(const Texture_2d& texture, size_t mipmap_index) noexcept
//{
//	assert(_id != Invalid::framebuffer_id);
//	assert(texture.id() != Invalid::texture_id);
//
//	glNamedFramebufferTexture(_id, GL_DEPTH_ATTACHMENT, texture.id(), mipmap_index);
//}
//
//void Framebuffer::attach_depth_texture(const Texture_2d_immut& texture, size_t mipmap_index) noexcept
//{
//	assert(_id != Invalid::framebuffer_id);
//	assert(texture.id() != Invalid::texture_id);
//
//	glNamedFramebufferTexture(_id, GL_DEPTH_ATTACHMENT, texture.id(), mipmap_index);
//}
//
//void Framebuffer::detach_color_texture(GLenum color_attachment) noexcept
//{
//	assert(is_valid_color_attachment(color_attachment) && color_attachment != GL_NONE);
//	glNamedFramebufferTexture(_id, color_attachment, Invalid::texture_id, 0);
//}
//
//void Framebuffer::detach_depth_renderbuffer() noexcept
//{
//	assert(_id != Invalid::framebuffer_id);
//	glNamedFramebufferRenderbuffer(_id, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, Invalid::renderbuffer_id);
//}
//
//void Framebuffer::detach_depth_texture() noexcept
//{
//	glNamedFramebufferTexture(_id, GL_DEPTH_ATTACHMENT, Invalid::texture_id, 0);
//}
//
//void Framebuffer::dispose() noexcept
//{
//	if (_id == Invalid::framebuffer_id) return;
//
//	glDeleteFramebuffers(1, &_id);
//	_id = Invalid::framebuffer_id;
//}
//
//void Framebuffer::set_draw_buffer(GLenum buff) noexcept
//{
//	assert(_id != Invalid::framebuffer_id);
//	assert(is_valid_color_attachment(buff));
//
//	glNamedFramebufferDrawBuffer(_id, buff);
//}
//
//void Framebuffer::set_draw_buffers(GLenum loc0, GLenum loc1, GLenum loc2, GLenum loc3,
//	GLenum loc4, GLenum loc5, GLenum loc6, GLenum loc7) noexcept
//{
//	assert(is_valid_color_attachment(loc0));
//	assert(is_valid_color_attachment(loc1));
//	assert(is_valid_color_attachment(loc2));
//	assert(is_valid_color_attachment(loc3));
//	assert(is_valid_color_attachment(loc4));
//	assert(is_valid_color_attachment(loc5));
//	assert(is_valid_color_attachment(loc6));
//	assert(is_valid_color_attachment(loc7));
//
//	GLenum buffs[8] = { loc0, loc1, loc2, loc3, loc4, loc5, loc6, loc7 };
//	glNamedFramebufferDrawBuffers(_id, std::extent<decltype(buffs)>::value, buffs);
//}
//
//void Framebuffer::set_read_buffer(GLenum buff) noexcept
//{
//	assert(is_valid_color_attachment(buff));
//	glNamedFramebufferReadBuffer(_id, buff);
//}
//
//void Framebuffer::validate() const
//{
//	validate_framebuffer(_id, GL_DRAW_FRAMEBUFFER);
//	validate_framebuffer(_id, GL_READ_FRAMEBUFFER);
//}

// ----- funcs -----

bool is_valid_color_attachment(const GLenum& v) noexcept
{
	return (v == GL_NONE)
		|| (v == GL_COLOR_ATTACHMENT0)
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
