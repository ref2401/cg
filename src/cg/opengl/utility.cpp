#include "cg/opengl/utility.h"

#include <cassert>
#include <string>
#include "cg/base/base.h"
#include "cg/data/shader.h"
#include "cg/file/file.h"
#include "cg/opengl/opengl.h"


namespace {

GLint get_shader_property(GLuint shader_id, GLenum prop) noexcept
{
	assert(shader_id != 0);
	assert(prop == GL_COMPILE_STATUS 
		|| prop == GL_DELETE_STATUS
		|| prop == GL_INFO_LOG_LENGTH
		|| prop == GL_SHADER_SOURCE_LENGTH
		|| prop == GL_SHADER_TYPE);

	GLint v = -1; // opengl: If an error is generated, no change is made to the contents of value.
	glGetShaderiv(shader_id, prop, &v);
	assert(v != -1);

	return v;
}

std::string get_shader_log(GLuint shader_id) noexcept
{
	assert(shader_id != 0);
	GLint log_size = get_shader_property(shader_id, GL_INFO_LOG_LENGTH);
	char* log_buffer = new char[log_size];

	GLsizei actual_size;
	glGetShaderInfoLog(shader_id, log_size, &actual_size, log_buffer);

	std::string log(log_buffer, actual_size);
	delete[] log_buffer;

	return log;
}

GLint make_shader(GLenum shader_type, const std::string& source_code)
{
	using cg::concat;
	using cg::enforce;

	assert(shader_type == GL_VERTEX_SHADER
		|| shader_type == GL_FRAGMENT_SHADER);
	assert(source_code.size() > 0);

	GLuint shader_id = glCreateShader(shader_type);
	const char* source_ptr = source_code.c_str();
	glShaderSource(shader_id, 1, &source_ptr, nullptr);
	glCompileShader(shader_id);

	auto compiled = get_shader_property(shader_id, GL_COMPILE_STATUS);
	if (!compiled) {
		const char* type_name = (shader_type == GL_VERTEX_SHADER) ? "Vertex" : "Pixel";
		std::string log = get_shader_log(shader_id);
		std::string msg = concat(type_name, " shader compilation error:\n", log);
	}

	return shader_id;
}

} // namespace

namespace cg {
namespace opengl {

Program::Program(const std::string& filename)
	: Program(filename.c_str())
{}

Program::Program(const char* filename)
	: Program(cg::file::load_glsl_program_source(filename))
{}

Program::Program(const cg::data::Shader_program_source_code& src)
{
	enforce(src.vertex_source.size() > 0, "Vertex source code is empty.");
	enforce(src.pixel_source.size() > 0, "Pixel source code is empty.");


}

Program::~Program() noexcept
{
	if (_id == Program::invalid_id) return;

	glDeleteProgram(_id);
	_id = Program::invalid_id;
}

// ----- funcs -----

Program make_program(const cg::data::Shader_program_source_code& src)
{
	
	return Program(src);
}

} // namespace opengl
} // namespace cg