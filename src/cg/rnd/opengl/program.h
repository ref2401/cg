#ifndef CG_RND_OPENGL_PROGRAM_H_
#define CG_RND_OPENGL_PROGRAM_H_

#include "cg/data/shader.h"
#include "cg/base/math.h"
#include "cg/rnd/opengl/opengl_def.h"
#include "cg/rnd/opengl/opengl_utility.h"


namespace cg {
namespace rnd {
namespace opengl {

class Glsl_program final {
public:

	Glsl_program() noexcept = default;

	explicit Glsl_program(const cg::data::Glsl_compute_desc& compute_desc);

	explicit Glsl_program(const cg::data::Glsl_program_desc& prog_data);

	Glsl_program(const Glsl_program&) = delete;

	Glsl_program(Glsl_program&& prog) noexcept;

	~Glsl_program() noexcept;


	Glsl_program& operator=(const Glsl_program&) = delete;

	Glsl_program& operator=(Glsl_program&& prog) noexcept;


	void dispose() noexcept;

	// Program's unique id.
	GLuint id() const noexcept
	{
		return _id;
	}

	// Program's name
	const std::string& name() const noexcept
	{
		return _name;
	}

private:

	GLuint _id = Blank::program_id;
	std::string _name;
};

class Glsl_shader final {
public:

	Glsl_shader() noexcept = default;

	Glsl_shader(GLenum type, const std::string& source_code);

	Glsl_shader(const Glsl_shader&) = delete;

	Glsl_shader(Glsl_shader&& shader) noexcept;

	~Glsl_shader() noexcept;


	Glsl_shader& operator=(const Glsl_shader&) = delete;

	Glsl_shader& operator=(Glsl_shader&& shader) noexcept;


	void dispose() noexcept;

	// Glsl_shader's unique id.
	GLuint id() const noexcept
	{
		return _id;
	}

	// Type of this shader object.	
	GLenum type() const noexcept
	{
		return _type;
	}

private:

	GLuint _id = Blank::shader_id;
	GLenum _type = GL_NONE;
};


// Returns true if the last compile operation was successful.
bool compile_status(const Glsl_shader& shader) noexcept;

// Returns information log for the specified program. 
// The information log for a program is modified when the shader is linked or validated. 
std::string info_log(const Glsl_program& program) noexcept;

// Returns information log for the specified shader. 
// The information log for a shader is modified when the shader is compiled.
std::string info_log(const Glsl_shader& shader) noexcept;

// Validate glGetProgram 'pname' argument value;
bool is_valid_program_property(GLenum value) noexcept;

// Validate glGetShader 'pname' argument value.
bool is_valid_shader_property(GLenum value) noexcept;

// Validates shader object type value. 
bool is_valid_shader_type(GLenum value) noexcept;

bool is_valid_program_parameter(GLenum pname) noexcept;

bool is_valid_shader_parameter(GLenum pname) noexcept;

void link(const Glsl_program& program);

// Returns true if the last link operation was successful.
bool link_status(const Glsl_program& program) noexcept;

GLint parameter(const Glsl_program& program, GLenum pname) noexcept;

GLint parameter(const Glsl_shader& shader, GLenum pname) noexcept;

void set_uniform(GLint location, unsigned int val) noexcept;

void set_uniform(GLint location, float val) noexcept;

void set_uniform(GLint location, const uint2& v) noexcept;

void set_uniform(GLint location, const float2& v) noexcept;

void set_uniform(GLint location, const float3& v) noexcept;

void set_uniform(GLint location, const float4& v) noexcept;

void set_uniform(GLint location, const float3x3& mat) noexcept;

void set_uniform(GLint location, const float4x4& mat) noexcept;

void set_uniform_array_int(GLint location, const GLint* ptr, GLsizei count) noexcept;

void set_uniform_array_float(GLint location, const float* ptr, GLsizei count) noexcept;

void set_uniform_array_float3(GLint location, const float* ptr, GLsizei count) noexcept;

void set_uniform_array_mat3(GLint location, const float3x3* ptr, GLsizei count);

void set_uniform_array_mat4(GLint location, const float4x4* ptr, GLsizei count);

void set_uniform_array_mat4(GLint location, const float* ptr, GLsizei count) noexcept;

// Returns a location value of the specified uniform.
// Throws if the uniform name does not belong to this shader program.
GLint uniform_location(const Glsl_program& program, const char* uniform_name);

// Returns a location value of the specified uniform.
// Throws if the uniform name does not belong to this shader program.
inline GLint uniform_location(const Glsl_program& program, const std::string& uniform_name)
{
	return uniform_location(program, uniform_name.c_str());
}


void validate(const Glsl_program& program);

// Returns true if the last validate operation was successful.
bool validate_status(const Glsl_program& program) noexcept;

} // namespace opengl
} // namespace rnd
} // namespace cg

#endif // CG_RND_OPENGL_PROGRAM_H_
