#include "cg/opengl/shader.h"

#include <cassert>
#include <exception>
#include <string>
#include "cg/base/base.h"
#include "cg/data/shader.h"
#include "cg/opengl/opengl_def.h"

using cg::enforce;


namespace cg {
namespace opengl {

// ----- Shader -----

Shader::Shader(GLenum type, const std::string& source_code)
	: _type(type)
{
	assert(type == GL_VERTEX_SHADER || type == GL_FRAGMENT_SHADER);
	assert(source_code.size() > 0);

	_id = glCreateShader(_type);
	const char* source_ptr = source_code.c_str();
	glShaderSource(_id, 1, &source_ptr, nullptr);
	glCompileShader(_id);

	if (!compiled()) {
		const char* type_name = (_type == GL_VERTEX_SHADER) ? "Vertex" : "Pixel";
		std::string log_msg = log();
		std::string exc_msg = concat(EXCEPTION_MSG(type_name, " shader compilation error.\n", log_msg));
		
		dispose();
		throw std::runtime_error(exc_msg);
	}
}

Shader::~Shader() noexcept
{
	dispose();
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
}

GLint Shader::get_property(GLenum prop) const noexcept
{
	assert(_id != Shader::invalid_id);
	assert(prop == GL_COMPILE_STATUS
		|| prop == GL_DELETE_STATUS
		|| prop == GL_INFO_LOG_LENGTH
		|| prop == GL_SHADER_SOURCE_LENGTH
		|| prop == GL_SHADER_TYPE);

	GLint v = -1; // opengl: If an error is generated, no change is made to the contents of value.
	glGetShaderiv(_id, prop, &v);
	assert(v != -1);

	return v;
}

std::string Shader::log() const noexcept
{
	assert(_id != Shader::invalid_id);

	GLint log_size = get_property(GL_INFO_LOG_LENGTH);
	char* msg_buffer = new char[log_size];

	GLsizei actual_size;
	glGetShaderInfoLog(_id, log_size, &actual_size, msg_buffer);

	std::string msg(msg_buffer, actual_size);
	delete[] msg_buffer;

	return msg;
}

// ----- Shader_program -----

Shader_program::Shader_program(const std::string& name, const cg::data::Shader_program_source_code& src)
	: _name(name)
{
	assert(src.vertex_source.size() > 0);
	assert(src.pixel_source.size() > 0);

	try {
		Shader vertex_shader(GL_VERTEX_SHADER, src.vertex_source);
		Shader pixel_shader(GL_FRAGMENT_SHADER, src.pixel_source);

		_id = glCreateProgram();
		glAttachShader(_id, vertex_shader.id());
		glAttachShader(_id, pixel_shader.id());
		glLinkProgram(_id); 
		// once shaders have been linked into a program they are no longer needed.
	}
	catch (...) {
		std::string exc_msg = concat(EXCEPTION_MSG("Error occured while creating the '", _name, "' shader program."));
		
		dispose();
		std::throw_with_nested(std::runtime_error(exc_msg));
	}

	if (!linked()) {
		std::string msg_log = log();
		std::string exc_msg = concat(EXCEPTION_MSG("Program linkage failed. '", _name, "'.\n", msg_log));
		
		dispose();
		throw std::runtime_error(exc_msg);
	}

	glValidateProgram(_id);
	if (!valid()) {
		std::string msg_log = log();
		std::string exc_msg = concat(EXCEPTION_MSG("Program validation failed. '", _name, "'.\n", msg_log));

		dispose();
		throw std::runtime_error(exc_msg);
	}
}

Shader_program::Shader_program(const std::string& name, const Shader& vertex_shader, const Shader& pixel_shader)
	: _name(name)
{
	_id = glCreateProgram();
	glAttachShader(_id, vertex_shader.id());
	glAttachShader(_id, pixel_shader.id());
	glLinkProgram(_id);

	if (!linked()) {
		std::string msg_log = log();
		std::string exc_msg = concat(EXCEPTION_MSG("Program linkage failed. '", _name, "'."));

		dispose();
		throw std::runtime_error(exc_msg);
	}

	glValidateProgram(_id);
	if (!valid()) {
		std::string msg_log = log();
		std::string exc_msg = concat(EXCEPTION_MSG("Program validation failed. '", _name, "'."));

		dispose();
		throw std::runtime_error(exc_msg);
	}
}

Shader_program::Shader_program(const std::string& name, const Shader& vertex_shader, const std::string& pixel_source_code)
	: _name(name)
{
	assert(pixel_source_code.size() > 0);

	try {
		Shader pixel_shader(GL_FRAGMENT_SHADER, pixel_source_code);

		_id = glCreateProgram();
		glAttachShader(_id, vertex_shader.id());
		glAttachShader(_id, pixel_shader.id());
		glLinkProgram(_id);
		// once shaders have been linked into a program they are no longer needed.
	}
	catch (...) {
		std::string exc_msg = concat(EXCEPTION_MSG("Error occured while creating the '", _name, "' shader program."));

		dispose();
		std::throw_with_nested(std::runtime_error(exc_msg));
	}

	if (!linked()) {
		std::string msg_log = log();
		std::string exc_msg = concat(EXCEPTION_MSG("Program linkage failed. '", _name, "'."));

		dispose();
		throw std::runtime_error(exc_msg);
	}

	glValidateProgram(_id);
	if (!valid()) {
		std::string msg_log = log();
		std::string exc_msg = concat(EXCEPTION_MSG("Program validation failed. '", _name, "'."));

		dispose();
		throw std::runtime_error(exc_msg);
	}
}

Shader_program::~Shader_program() noexcept
{
	dispose();
}

void Shader_program::dispose() noexcept
{
	if (_id == Invalid::shader_program_id) return;

	glDeleteProgram(_id);
	_id = Invalid::shader_program_id;
	// NOTE(ref2401): Do not clear the _name field.
	// It might be used to compose exception messages after the object has been disposed;
}

GLint Shader_program::get_property(GLenum prop) const noexcept
{
	assert(_id != Invalid::shader_program_id);
	assert(prop == GL_DELETE_STATUS
		|| prop == GL_INFO_LOG_LENGTH
		|| prop == GL_LINK_STATUS
		|| prop == GL_VALIDATE_STATUS);

	GLint v = -1; // opengl: If an error is generated, no change is made to the contents of value.
	glGetProgramiv(_id, prop, &v);
	assert(v != -1);

	return v;
}

GLint Shader_program::get_uniform_location(const std::string& uniform_name) const
{
	return get_uniform_location(uniform_name.c_str());
}

GLint Shader_program::get_uniform_location(const char* uniform_name) const
{
	assert(uniform_name);

	GLint location = glGetUniformLocation(_id, uniform_name);
	enforce(_id != Invalid::uniform_location, 
		EXCEPTION_MSG("Shader program '", _name, "' does not have a uniform called '", uniform_name, "'."));

	return location;
}

bool Shader_program::linked() const noexcept
{
	return get_property(GL_LINK_STATUS) != 0;
}

std::string Shader_program::log() const noexcept
{
	assert(_id != Invalid::shader_program_id);

	GLint log_size = get_property(GL_INFO_LOG_LENGTH);
	char* msg_buffer = new char[log_size];

	GLsizei actual_size;
	glGetProgramInfoLog(_id, log_size, &actual_size, msg_buffer);

	std::string msg(msg_buffer, actual_size);
	delete[] msg_buffer;

	return msg;
}

bool Shader_program::valid() const noexcept
{
	return get_property(GL_VALIDATE_STATUS) != 0;
}


} // namespace opengl
} // namespace cg