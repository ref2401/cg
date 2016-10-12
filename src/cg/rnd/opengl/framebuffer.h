#ifndef CG_RND_OPENGL_FRAMEBUFFER_H_
#define CG_RND_OPENGL_FRAMEBUFFER_H_

#include "cg/math/math.h"
#include "cg/rnd/opengl/opengl_def.h"
#include "cg/rnd/opengl/texture.h"
#include "cg/rnd/opengl/utility.h"


namespace cg {
namespace rnd {
namespace opengl {

class Renderbuffer final {
public:

	Renderbuffer() noexcept = default;

	Renderbuffer(Texture_format format, const uint2& size) noexcept;

	Renderbuffer(const Renderbuffer&) = delete;

	Renderbuffer(Renderbuffer&& rnd_buff) noexcept;

	~Renderbuffer() noexcept;


	Renderbuffer& operator=(Renderbuffer&& rnd_buff) noexcept;


	// Texel format of the renderbuffer.
	Texture_format format() const noexcept
	{
		return _format;
	}

	GLuint id() const noexcept
	{
		return _id;
	}

	// Reallocates a new storage for the renderbuffer object.
	void reallocate_storate(Texture_format format, const uint2& size) noexcept;

	//  Size of the renderbuffer in pixels.
	uint2 size() const noexcept
	{
		return _size;
	}

	// Sets the new size of the renderbuffer. 
	// Internally calls the reallocate_storage method. 
	void set_size(const uint2& size) noexcept;

private:
	void dispose() noexcept;

	GLuint _id = Invalid::renderbuffer_id;
	Texture_format _format = Texture_format::none;
	uint2 _size = uint2::zero;
};

// Framebuffer object supports 8 simultaneous color attachments.
class Framebuffer final {
public:

	Framebuffer() noexcept;

	Framebuffer(const Framebuffer& fb) = delete;

	Framebuffer(Framebuffer&& fb) noexcept;

	~Framebuffer() noexcept;


	Framebuffer& operator=(Framebuffer&& fb) noexcept;

	// Attaches a texture as color attachment.
	void attach_color_texture(GLenum color_attachment, const Texture_2d& texture, size_t mipmap_index = 0) noexcept;

	// Attaches a texture as color attachment.
	void attach_color_texture(GLenum color_attachment, const Texture_2d_immut& texture, size_t mipmap_index = 0) noexcept;

	// Attaches a render buffer as depth atatchment.
	void attach_depth_renderbuffer(const Renderbuffer& rnd_buff) noexcept;

	// Attaches a texture as depth attachment.
	void attach_depth_texture(const Texture_2d& texture, size_t mipmap_index = 0) noexcept;

	// Attaches a texture as depth attachment.
	void attach_depth_texture(const Texture_2d_immut& texture, size_t mipmap_index = 0) noexcept;

	// Clears color texture attachment point.
	void detach_color_texture(GLenum color_attachment) noexcept;

	// Clears depth renderbuffer attachment point.
	void detach_depth_renderbuffer() noexcept;

	// Clears depth texture attachment point.
	void detach_depth_texture() noexcept;

	GLuint id() const noexcept
	{
		return _id;
	}

	// Sets color buffer that is to be drawn into.
	// GL_NONE specifies that no write operations will be performed on this framebuffer.
	void set_draw_buffer(GLenum buff) noexcept;

	// Sets the list of color buffer to be drawn to.
	// Params:
	// -	loc{n}: specifies in which color buffer a pixel(fragment) shader writes its output.
	//				n belongs to the ragne [0, 7].
	// GL_NONE specifies that pixl(fragment) shader output value is not written into any color buffer.
	void set_draw_buffers(GLenum loc0 = GL_NONE, GLenum loc1 = GL_NONE, 
		GLenum loc2 = GL_NONE, GLenum loc3 = GL_NONE, GLenum loc4 = 
		GL_NONE, GLenum loc5 = GL_NONE, GLenum loc6 = GL_NONE, GLenum loc7 = GL_NONE) noexcept;

	void set_read_buffer(GLenum buff) noexcept;

	// Checks this framebuffer's completeness status as draw and read framebuffer target.
	// Throws an exception if OpenGL considers the framebuffer is not complete.
	void validate() const;

private:
	void dispose() noexcept;

	GLuint _id = Invalid::framebuffer_id;
};

} // namespace opengl
} // namespace rnd
} // namespace cg

#endif // CG_RND_OPENGL_FRAMEBUFFER_H_