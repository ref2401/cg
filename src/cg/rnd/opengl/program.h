#ifndef CG_RND_OPENGL_PROGRAM_H_
#define CG_RND_OPENGL_PROGRAM_H_

#include "cg/data/shader.h"
#include "cg/math/math.h"
#include "cg/rnd/opengl/opengl_def.h"
#include "cg/rnd/opengl/opengl_utility.h"


namespace cg {
namespace rnd {
namespace opengl {

class Glsl_program final {
public:

	Glsl_program() noexcept = default;

	Glsl_program(const std::string& name, const cg::data::Glsl_program_data& prog_data);

	Glsl_program(const Glsl_program&) = delete;

	Glsl_program(Glsl_program&& prog) noexcept;

	~Glsl_program() noexcept;


	Glsl_program& operator=(const Glsl_program) = delete;

	Glsl_program& operator=(Glsl_program&& prog) noexcept;


	// Program's unique id.
	GLuint id() const noexcept
	{
		return _id;
	}

	// Returns true if the last link operation was successful.
	bool linked() const noexcept;

	// Returns information log for this program. The information log for a program
	// is modified when the shader is linked or validated. 
	std::string log() const noexcept;

	// Program's name
	const std::string& name() const noexcept
	{
		return _name;
	}

	// Returns true if the last validate operation was successful.
	bool valid() const noexcept;

private:

	void dispose() noexcept;

	GLint get_property(GLenum prop) const noexcept;

	GLuint _id = Invalid::glsl_program_id;
	std::string _name;
};

class Shader final {
public:

	Shader() noexcept = default;

	Shader(GLenum type, const std::string& source_code);

	Shader(const Shader&) = delete;

	Shader(Shader&& shader) noexcept;

	~Shader() noexcept;


	Shader& operator=(const Shader&) = delete;

	Shader& operator=(Shader&& shader) noexcept;

	// Returns true if the last compile operation was successful.
	bool compiled() const noexcept;

	// Shader's unique id.
	GLuint id() const noexcept
	{
		return _id;
	}

	// Returns information log for this shader. The information log for a shader
	// is modified when the shader is compiled.
	std::string log() const noexcept;

	// Type of this shader object.	
	GLenum type() const noexcept
	{
		return _type;
	}

private:

	void dispose() noexcept;

	GLint get_property(GLenum prop) const noexcept;

	GLuint _id = Invalid::shader_id;
	GLenum _type = GL_NONE;
};


// Validate glGetProgram 'pname' argument value;
bool is_valid_program_property(GLenum value) noexcept;

// Validate glGetShader 'pname' argument value.
bool is_valid_shader_property(GLenum value) noexcept;

// Validates shader object type value. 
bool is_valid_shader_type(GLenum value) noexcept;

} // namespace opengl
} // namespace rnd
} // namespace cg

#endif // CG_RND_OPENGL_PROGRAM_H_
