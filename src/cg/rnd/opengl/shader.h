#ifndef CG_RND_OPENGL_SHADER_H_
#define CG_RND_OPENGL_SHADER_H_

#include <vector>
#include "cg/data/shader.h"
#include "cg/data/mesh.h"
#include "cg/math/math.h"
#include "cg/rnd/opengl/opengl_def.h"
#include "cg/rnd/opengl/utility.h"


namespace cg {
namespace rnd {
namespace opengl {

class Shader final {
public:

	static constexpr GLuint invalid_id = 0;

	static constexpr GLenum invalid_type = GL_NONE;


	Shader(GLenum type, const std::string& source_code);

	Shader(const Shader&) = delete;

	Shader(Shader&&) = delete;

	~Shader() noexcept;


	bool compiled() const noexcept;

	GLuint id() const noexcept
	{
		return _id;
	}

	std::string log() const noexcept;

	GLenum type() const noexcept
	{
		return _type;
	}

private:

	void dispose() noexcept;

	GLint get_property(GLenum prop) const noexcept;

	GLuint _id = Invalid::shader_id;
	GLenum _type;
};

class Shader_program final {
public:

	Shader_program() noexcept = default;

	Shader_program(const std::string& name, const cg::data::Glsl_program_data& src);

	Shader_program(const std::string& name, const Shader& vertex_shader, const Shader& pixel_shader);

	Shader_program(const std::string& name, const Shader& vertex_shader, const std::string& pixel_source_code);

	Shader_program(const Shader_program&) = delete;

	Shader_program(Shader_program&& prog) noexcept;

	~Shader_program() noexcept;


	Shader_program& operator=(Shader_program&& prog) noexcept;


	// Returns a location value of the specified uniform.
	// Throws if the uniform name does not belong to this shader program.
	GLint get_uniform_location(const std::string& uniform_name) const;

	// Returns a location value of the specified uniform.
	// Throws if the uniform name does not belong to this shader program.
	GLint get_uniform_location(const char* uniform_name) const;

	GLuint id() const noexcept
	{
		return _id;
	}

	bool linked() const noexcept;

	std::string log() const noexcept;

	const std::string& name() const noexcept
	{
		return _name;
	}

	bool valid() const noexcept;

private:

	void dispose() noexcept;

	GLint get_property(GLenum prop) const noexcept;


	GLuint _id = Invalid::shader_program_id;
	std::string _name;
};

// Desribis attribute indices within a particular shader.
// If a location equals to Invalid_id::vertex_attrib_location it means that there is no such attribute is the shader.
struct Vertex_attrib_layout {

	Vertex_attrib_layout() noexcept = default;

	Vertex_attrib_layout(GLint position_location, GLint normal_location,
		GLint tex_coord_location, GLint tangent_h_location) noexcept
		: position_location(position_location), normal_location(normal_location),
		tex_coord_location(tex_coord_location), tangent_h_location(tangent_h_location)
	{}


	cg::data::Vertex_attribs attribs() const noexcept;

	GLint position_location = Invalid::vertex_attrib_location;
	GLint normal_location = Invalid::vertex_attrib_location;
	GLint tex_coord_location = Invalid::vertex_attrib_location;
	GLint tangent_h_location = Invalid::vertex_attrib_location;
};


// Sets a uniform variable value.
// Params:
// -	location: Uniform location in the currently used shader program.
// -	value: New value to be used for the specified uniform variable.
template<typename T>
void set_uniform(GLint location, const T& value) noexcept;

// Sets a uniform array variable.
// Template params:
// -	T:		Uniform array variable type which is specified in the shaderp.
//				T defines which glUnifrom fucntion will be called.
//				Example: T = float -> glUnifrom1fv, T = mat4 -> glUnifromMatrix4fv.
//
// -	TPtr:	Type of the data pointer. Ususally T and TPtr are the same.
//				Example: T = mat4, TPtr = float. 
//				That means that matrices are already packed into an array pointed to by ptr.
//
// Params:
// -	ptr:	Pointer to the first element of the data.
// -	count:	How many elements of type T are stored in the array pointer to by ptr.
template<typename T, typename TPtr>
void set_uniform_array(GLint location, const TPtr* ptr, size_t count) noexcept;

} // namespace opengl
} // namespace rnd
} // namespace cg

#endif // CG_RND_OPENGL_SHADER_H_