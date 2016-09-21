#include "cg/opengl/utility.h"

#include <cassert>
#include <string>
#include "cg/base/base.h"
#include "cg/data/shader.h"
#include "cg/file/file.h"
#include "cg/opengl/opengl.h"

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
		std::string msg = concat(type_name, " shader compilation error:\n", log());

		throw std::runtime_error(msg);
	}
}

Shader::Shader(Shader&& shader) noexcept
	: _id(shader._id), _type(shader._type)
{
	shader._id = Shader::invalid_id;
	shader._type = Shader::invalid_type;
}

Shader::~Shader() noexcept
{
	dispose();
}

Shader& Shader::operator=(Shader&& shader) noexcept
{
	dispose();

	_id = shader._id;
	_type = shader._type;
	shader._id = Shader::invalid_id;
	shader._type = Shader::invalid_type;

	return *this;
}

bool Shader::compiled() const noexcept
{
	return get_property(GL_COMPILE_STATUS) == 1;
}

void Shader::dispose() noexcept
{
	if (_id == Shader::invalid_id) return;

	glDeleteShader(_id);
	_id = Shader::invalid_id;
	_type = Shader::invalid_type;
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

Shader_program::Shader_program(const std::string& filename)
	: Shader_program(filename.c_str())
{}

Shader_program::Shader_program(const char* filename)
	: Shader_program(cg::file::load_glsl_program_source(filename))
{}

Shader_program::Shader_program(const cg::data::Shader_program_source_code& src)
{
	enforce(src.vertex_source.size() > 0, "Vertex source code is empty.");
	enforce(src.pixel_source.size() > 0, "Pixel source code is empty.");


}

Shader_program::~Shader_program() noexcept
{
	if (_id == Shader_program::invalid_id) return;

	glDeleteProgram(_id);
	_id = Shader_program::invalid_id;
}

// ----- funcs -----

Shader_program make_program(const cg::data::Shader_program_source_code& src)
{
	
	return Shader_program(src);
}

} // namespace opengl
} // namespace cg