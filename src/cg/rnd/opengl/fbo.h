#ifndef CG_RND_OPENGL_FBO_H_
#define CG_RND_OPENGL_FBO_H_

#include "cg/math/math.h"
#include "cg/rnd/opengl/opengl_def.h"
#include "cg/rnd/opengl/opengl_utility.h"
#include "cg/rnd/opengl/texture.h"


namespace cg {
namespace rnd {
namespace opengl {

class Renderbuffer final {
public:

	Renderbuffer() noexcept = default;

	Renderbuffer(GLenum internal_format, const cg::uint2& size) noexcept;

	Renderbuffer(const Renderbuffer&) = delete;

	Renderbuffer(Renderbuffer&& rnd_buff) noexcept;

	~Renderbuffer() noexcept;


	Renderbuffer& operator=(const Renderbuffer&) = delete;

	Renderbuffer& operator=(Renderbuffer&& rnd_buff) noexcept;


	void dispose() noexcept;

	GLuint id() const noexcept
	{
		return _id;
	}

	// Texel format of the renderbuffer.
	GLenum internal_format() const noexcept
	{
		return _internal_format;
	}

	// Reallocates a new storage for the renderbuffer object.
	void reallocate_storage(GLenum internal_format, const cg::uint2& size) noexcept;

	//  Size of the renderbuffer in pixels.
	cg::uint2 size() const noexcept
	{
		return _size;
	}

	// Sets the new size of the renderbuffer. 
	// Calls the reallocate storage method.
	void set_size(const cg::uint2& size) noexcept;

private:

	GLuint _id = Blank::renderbuffer_id;
	GLenum _internal_format = GL_NONE;
	cg::uint2 _size = cg::uint2::zero;
};

// Framebuffer object supports 8 simultaneous color attachments.
class Framebuffer final {
public:

	Framebuffer() noexcept;

	Framebuffer(const Framebuffer&) = delete;

	Framebuffer(Framebuffer&& fb) noexcept;

	~Framebuffer() noexcept;


	Framebuffer& operator=(const Framebuffer& fb) = delete;

	Framebuffer& operator=(Framebuffer&& fb) noexcept;

	void dispose() noexcept;

	GLuint id() const noexcept
	{
		return _id;
	}

private:

	GLuint _id = Blank::framebuffer_id;
};


// Attaches a texture as color attachment.
void attach_color_target(const Framebuffer& fbo, GLenum color_attachment, 
	const Texture_2d_i& texture, GLint mipmap_index = 0) noexcept;

// Attaches a renderbuffer as depth atatchment.
void attach_depth_target(const Framebuffer& fbo, const Renderbuffer& rnd_buff) noexcept;

// Attaches a texture as depth attachment.
void attach_depth_target(const Framebuffer& fbo, const Texture_2d_i& texture, GLint mipmap_index = 0) noexcept;

// Clears color texture attachment point.
void detach_color_target(const Framebuffer& fbo, GLenum color_attachment) noexcept;

// Clears depth texture attachment point & depth renderbuffer attachment point.
void detach_depth_target(const Framebuffer& fbo) noexcept;

// Validate glFramebufferTexture{1/2/3D}, glNamedFramebufferTexture 'attachment' argument value.
bool is_valid_color_attachment(const GLenum& v) noexcept;

// Sets color buffer that is to be drawn into.
// GL_NONE specifies that no write operations will be performed on this framebuffer.
void set_draw_buffer(const Framebuffer& fbo, GLenum buff) noexcept;

// Sets the list of color buffer to be drawn to.
// Params:
// -	loc{n}: specifies in which color buffer a pixel(fragment) shader writes its output.
//				n belongs to the ragne [0, 7].
// GL_NONE specifies that pixl(fragment) shader output value is not written into any color buffer.
void set_draw_buffers(const Framebuffer& fbo, 
	GLenum loc0 = GL_NONE, GLenum loc1 = GL_NONE, GLenum loc2 = GL_NONE, GLenum loc3 = GL_NONE, 
	GLenum loc4 = GL_NONE, GLenum loc5 = GL_NONE, GLenum loc6 = GL_NONE, GLenum loc7 = GL_NONE) noexcept;

void set_read_buffer(const Framebuffer& fbo, GLenum buff) noexcept;

// Checks this framebuffer's completeness status as draw and read framebuffer target.
// Throws an exception if OpenGL considers the framebuffer is not complete.
void validate(const Framebuffer& fbo);

} // namespace opengl
} // namespace rnd
} // namespace cg

#endif // CG_RND_OPENGL_FBO_H_

