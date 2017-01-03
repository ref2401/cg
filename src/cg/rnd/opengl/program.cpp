#include "cg/rnd/opengl/program.h"

#include <cassert>
#include <exception>
#include <string>
#include <utility>
#include "cg/base/base.h"


namespace cg {
namespace rnd {
namespace opengl {

// ----- Glsl_program -----

Glsl_program::Glsl_program(const std::string& name, const cg::data::Glsl_compute_data& compute_data)
	: _name(name)
{
	assert(!name.empty());
	assert(!compute_data.compute_shader_source_code.empty());

	try {
		Shader shader(GL_COMPUTE_SHADER, compute_data.compute_shader_source_code);

		_id = glCreateProgram();
		glAttachShader(_id, shader.id());
		glLinkProgram(_id);
		glDetachShader(_id, shader.id()); // once shaders have been linked into a program they are no longer needed.
	}
	catch (...) {
		std::string exc_msg = EXCEPTION_MSG("Error occured while creating the '", _name, "' shader program.");

		dispose();
		std::throw_with_nested(std::runtime_error(exc_msg));
	}

	validate();
}

Glsl_program::Glsl_program(const std::string& name, const cg::data::Glsl_program_data& prog_data)
	: _name(name)
{
	assert(!name.empty());
	assert(prog_data.has_vertex_shader() && prog_data.has_fragment_shader());

	try {
		Shader vertex_shader(GL_VERTEX_SHADER, prog_data.vertex_shader_source_code);
		Shader pixel_shader(GL_FRAGMENT_SHADER, prog_data.fragment_shader_source_code);

		_id = glCreateProgram();
		glAttachShader(_id, vertex_shader.id());
		glAttachShader(_id, pixel_shader.id());
		glLinkProgram(_id);

		// once shaders have been linked into a program they are no longer needed.
		glDetachShader(_id, vertex_shader.id());
		glDetachShader(_id, pixel_shader.id());
	}
	catch (...) {
		std::string exc_msg = EXCEPTION_MSG("Error occured while creating the '", _name, "' shader program.");

		dispose();
		std::throw_with_nested(std::runtime_error(exc_msg));
	}

	validate();
}

Glsl_program::Glsl_program(Glsl_program&& prog) noexcept
	: _id(prog._id),
	_name(std::move(prog._name))
{
	prog._id = Invalid::glsl_program_id;
}

Glsl_program::~Glsl_program() noexcept
{
	dispose();
}

Glsl_program& Glsl_program::operator=(Glsl_program&& prog) noexcept
{
	if (this == &prog) return *this;

	dispose();
	_id = prog._id;
	_name = std::move(prog._name);

	prog._id = Invalid::glsl_program_id;

	return *this;
}

void Glsl_program::dispose() noexcept
{
	if (_id == Invalid::glsl_program_id) return;

	glDeleteProgram(_id);
	_id = Invalid::glsl_program_id;
	_name.clear();
}

GLint Glsl_program::get_property(GLenum prop) const noexcept
{
	assert(_id != Invalid::glsl_program_id);
	assert(is_valid_program_property(prop));

	GLint v = -1; // opengl: If an error is generated, no change is made to the contents of value.
	glGetProgramiv(_id, prop, &v);
	assert(v != -1);

	return v;
}

GLint Glsl_program::get_uniform_location(const std::string& uniform_name) const
{
	return get_uniform_location(uniform_name.c_str());
}

GLint Glsl_program::get_uniform_location(const char* uniform_name) const
{
	assert(_id != Invalid::glsl_program_id);
	assert(uniform_name);

	GLint location = glGetUniformLocation(_id, uniform_name);
	ENFORCE(location != Invalid::uniform_location,
		"Shader program '", _name, "' does not have a uniform called '", uniform_name, "'.");

	return location;
}

bool Glsl_program::linked() const noexcept
{
	assert(_id != Invalid::glsl_program_id);
	return get_property(GL_LINK_STATUS) != 0;
}

std::string Glsl_program::log() const noexcept
{
	assert(_id != Invalid::glsl_program_id);

	GLint log_size = get_property(GL_INFO_LOG_LENGTH);
	if (log_size == 0) return {};

	GLsizei actual_size;
	char* msg_buffer = new char[log_size];
	glGetProgramInfoLog(_id, log_size, &actual_size, msg_buffer);

	std::string msg(msg_buffer, actual_size);
	delete[] msg_buffer;

	return msg;
}

bool Glsl_program::valid() const noexcept
{
	assert(_id != Invalid::glsl_program_id);
	return get_property(GL_VALIDATE_STATUS) != 0;
}

void Glsl_program::validate()
{
	if (!linked()) {
		std::string msg_log = log();
		std::string exc_msg = EXCEPTION_MSG("Program linkage failed. '", _name, "'.\n", msg_log);

		dispose();
		throw std::runtime_error(exc_msg);
	}

	glValidateProgram(_id);
	if (!valid()) {
		std::string msg_log = log();
		std::string exc_msg = EXCEPTION_MSG("Program validation failed. '", _name, "'.\n", msg_log);

		dispose();
		throw std::runtime_error(exc_msg);
	}
}

// ----- Shader -----

Shader::Shader(GLenum type, const std::string& source_code)
	: _type(type)
{
	assert(is_valid_shader_type(type));
	assert(!source_code.empty());

	_id = glCreateShader(_type);
	const char* source_ptr = source_code.c_str();
	glShaderSource(_id, 1, &source_ptr, nullptr);
	glCompileShader(_id);
	if (compiled()) return; // The compilation succeeded.


	// Shader compilation error handling
	const char* type_name = nullptr;
	switch (_type) {
		case GL_VERTEX_SHADER: 
			type_name = "Vertex";
			break;

		case GL_FRAGMENT_SHADER:
			type_name = "Fragment";
			break;

		case GL_COMPUTE_SHADER:
			type_name = "Compute";
			break;
	}
		
	std::string log_msg = log();
	std::string exc_msg = EXCEPTION_MSG(type_name, " shader compilation error.\n", log_msg);

	dispose();
	throw std::runtime_error(exc_msg);
}

Shader::Shader(Shader&& shader) noexcept
	: _id(shader._id),
	_type(shader._type)
{
	shader._id = Invalid::shader_id;
	shader._type = GL_NONE;
}

Shader::~Shader() noexcept
{
	dispose();
}

Shader& Shader::operator=(Shader&& shader) noexcept
{
	if (this == &shader) return *this;

	dispose();
	_id = shader._id;
	_type = shader._type;

	shader._id = Invalid::shader_id;
	shader._type = GL_NONE;

	return *this;
}

bool Shader::compiled() const noexcept
{
	return get_property(GL_COMPILE_STATUS) == 1;
}

void Shader::dispose() noexcept
{
	if (_id == Invalid::shader_id) return;

	glDeleteShader(_id);
	_id = Invalid::shader_id;
	_type = GL_NONE;
}

GLint Shader::get_property(GLenum prop) const noexcept
{
	assert(_id != Invalid::shader_id);
	assert(is_valid_shader_property(prop));

	GLint v = -1; // opengl: If an error is generated, no change is made to the contents of value.
	glGetShaderiv(_id, prop, &v);
	assert(v != -1);

	return v;
}

std::string Shader::log() const noexcept
{
	assert(_id != Invalid::shader_id);

	GLint log_size = get_property(GL_INFO_LOG_LENGTH);
	if (log_size == 0) return {};

	GLsizei actual_size;
	char* msg_buffer = new char[log_size];
	glGetShaderInfoLog(_id, log_size, &actual_size, msg_buffer);

	std::string msg(msg_buffer, actual_size);
	delete[] msg_buffer;

	return msg;
}

// ----- funcs -----

bool is_valid_program_property(GLenum value) noexcept
{
	return (value == GL_DELETE_STATUS)
		|| (value == GL_INFO_LOG_LENGTH)
		|| (value == GL_LINK_STATUS)
		|| (value == GL_VALIDATE_STATUS);
}

bool is_valid_shader_property(GLenum value) noexcept
{
	return (value == GL_COMPILE_STATUS)
		|| (value == GL_DELETE_STATUS)
		|| (value == GL_INFO_LOG_LENGTH)
		|| (value == GL_SHADER_SOURCE_LENGTH)
		|| (value == GL_SHADER_TYPE);
}

bool is_valid_shader_type(GLenum value) noexcept
{
	return (value == GL_VERTEX_SHADER)
		|| (value == GL_FRAGMENT_SHADER)
		|| (value == GL_COMPUTE_SHADER);
}

void set_uniform(GLint location, unsigned int val) noexcept
{
	assert(location != Invalid::uniform_location);
	glUniform1ui(location, val);
}

void set_uniform(GLint location, float val) noexcept
{
	assert(location != Invalid::uniform_location);
	glUniform1f(location, val);
}

void set_uniform(GLint location, const uint2& v) noexcept
{
	assert(location != Invalid::uniform_location);
	glUniform2ui(location, v.x, v.y);
}

void set_uniform(GLint location, const float3& v) noexcept
{
	assert(location != Invalid::uniform_location);
	glUniform3f(location, v.x, v.y, v.z);
}

void set_uniform(GLint location, const mat3& mat) noexcept
{
	assert(location != Invalid::uniform_location);

	float arr[9];
	to_array_column_major_order(mat, arr);
	glUniformMatrix3fv(location, 1, false, arr);
}

void set_uniform(GLint location, const mat4& mat) noexcept
{
	assert(location != Invalid::uniform_location);

	float arr[16];
	to_array_column_major_order(mat, arr);
	glUniformMatrix4fv(location, 1, false, arr);
}

} // namespace opengl
} // namespace rnd
} // namespace cg
