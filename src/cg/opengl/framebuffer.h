#ifndef CG_OPENGL_FRAMEBUFFER_H_
#define CG_OPENGL_FRAMEBUFFER_H_

#include "cg/math/math.h"
#include "cg/opengl/opengl_def.h"
#include "cg/opengl/texture.h"
#include "cg/opengl/utility.h"


namespace cg {
namespace opengl {

// Framebuffer object supports 8 simultaneous color attachments.
class Framebuffer final {
public:

	Framebuffer() noexcept;

	Framebuffer(const Framebuffer& fb) = delete;

	Framebuffer(Framebuffer&& fb) noexcept;

	~Framebuffer() noexcept;


	Framebuffer& operator=(Framebuffer&& fb) noexcept;

	// Attach a texture as color attachment.
	void attach_color_texture(GLenum color_attachment, const Texture_2d& texture, size_t mipmap_index = 0) noexcept;

	// Attach a texture as depth attachment.
	void attach_depth_texture(const Texture_2d& texture, size_t mipmap_index = 0) noexcept;

	void detach_color_texture(GLenum color_attachment) noexcept;

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
} // namespace cg

#endif // CG_OPENGL_FRAMEBUFFER_H_