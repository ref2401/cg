#include "cg/rnd/opengl/program.h"

#include <cassert>
#include <exception>
#include <string>
#include "cg/base/base.h"


namespace cg {
namespace rnd {
namespace opengl {

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
		|| (value == GL_FRAGMENT_SHADER);
}

} // namespace opengl
} // namespace rnd
} // namespace cg
