#include "cg/opengl/framebuffer.h"

#include <cassert>
#include <exception>
#include <type_traits>


namespace {

void validate_framebuffer(GLuint id, GLenum target)
{
	assert(id != cg::opengl::Invalid::framebuffer_id);
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
namespace opengl {

Framebuffer::Framebuffer() noexcept
{
	glCreateFramebuffers(1, &_id);
}

Framebuffer::Framebuffer(Framebuffer&& fb) noexcept :
_id(fb._id)
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

void Framebuffer::attach_color_texture(GLenum color_attachment, const
	Texture_2d& texture, size_t mipmap_index) noexcept
{
	assert(_id != Invalid::framebuffer_id);
	assert(is_valid_color_attachment(color_attachment) && color_attachment != GL_NONE);
	assert(texture.id() != Invalid::texture_id);

	glNamedFramebufferTexture(_id, color_attachment, texture.id(), mipmap_index);
}

void Framebuffer::attach_depth_texture(const Texture_2d& texture, size_t mipmap_index) noexcept
{
	assert(_id != Invalid::framebuffer_id);
	assert(texture.id() != Invalid::texture_id);

	glNamedFramebufferTexture(_id, GL_DEPTH_ATTACHMENT, texture.id(), mipmap_index);
}

void Framebuffer::detach_color_texture(GLenum color_attachment) noexcept
{
	assert(is_valid_color_attachment(color_attachment) && color_attachment != GL_NONE);
	glNamedFramebufferTexture(_id, color_attachment, Invalid::texture_id, 0);
}

void Framebuffer::detach_depth_texture() noexcept
{
	glNamedFramebufferTexture(_id, GL_DEPTH_ATTACHMENT, Invalid::texture_id, 0);
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
	assert(is_valid_color_attachment(buff));

	glNamedFramebufferDrawBuffer(_id, buff);
}

void Framebuffer::set_draw_buffers(GLenum loc0, GLenum loc1, GLenum loc2, GLenum loc3,
	GLenum loc4, GLenum loc5, GLenum loc6, GLenum loc7) noexcept
{
	assert(is_valid_color_attachment(loc0));
	assert(is_valid_color_attachment(loc1));
	assert(is_valid_color_attachment(loc2));
	assert(is_valid_color_attachment(loc3));
	assert(is_valid_color_attachment(loc4));
	assert(is_valid_color_attachment(loc5));
	assert(is_valid_color_attachment(loc6));
	assert(is_valid_color_attachment(loc7));

	GLenum buffs[8] = { loc0, loc1, loc2, loc3, loc4, loc5, loc6, loc7 };
	glNamedFramebufferDrawBuffers(_id, std::extent<decltype(buffs)>::value, buffs);
}

void Framebuffer::set_read_buffer(GLenum buff) noexcept
{
	assert(is_valid_color_attachment(buff));
	glNamedFramebufferReadBuffer(_id, buff);
}

void Framebuffer::validate() const
{
	validate_framebuffer(_id, GL_DRAW_FRAMEBUFFER);
	validate_framebuffer(_id, GL_READ_FRAMEBUFFER);
}

} // namespace opengl
} // namespace cg
