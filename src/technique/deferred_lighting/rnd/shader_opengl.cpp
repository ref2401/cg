#include "technique/deferred_lighting/rnd/shader.h"

#include <cassert>
#include <exception>
#include <string>
#include "cg/base/base.h"
#include "cg/data/shader.h"
#include "cg/rnd/opengl/opengl_def.h"

using namespace cg;


namespace deferred_lighting {
namespace rnd {

// ----- Glsl_shader -----

Glsl_shader::Glsl_shader(GLenum type, const std::string& source_code)
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
		std::string exc_msg = EXCEPTION_MSG(type_name, " shader compilation error.\n", log_msg);
		
		dispose();
		throw std::runtime_error(exc_msg);
	}
}

Glsl_shader::~Glsl_shader() noexcept
{
	dispose();
}

bool Glsl_shader::compiled() const noexcept
{
	return get_property(GL_COMPILE_STATUS) == 1;
}

void Glsl_shader::dispose() noexcept
{
	if (_id == Invalid::shader_id) return;

	glDeleteShader(_id);
	_id = Invalid::shader_id;
}

GLint Glsl_shader::get_property(GLenum prop) const noexcept
{
	assert(_id != Glsl_shader::invalid_id);
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

std::string Glsl_shader::log() const noexcept
{
	assert(_id != Glsl_shader::invalid_id);

	GLint log_size = get_property(GL_INFO_LOG_LENGTH);
	char* msg_buffer = new char[log_size];

	GLsizei actual_size;
	glGetShaderInfoLog(_id, log_size, &actual_size, msg_buffer);

	std::string msg(msg_buffer, actual_size);
	delete[] msg_buffer;

	return msg;
}

// ----- Shader_program -----

Shader_program::Shader_program(const std::string& name, const cg::data::Glsl_program_desc& src)
	: _name(name)
{
	assert(src.vertex_shader_source_code.size() > 0);
	assert(src.fragment_shader_source_code.size() > 0);

	try {
		Glsl_shader vertex_shader(GL_VERTEX_SHADER, src.vertex_shader_source_code);
		Glsl_shader pixel_shader(GL_FRAGMENT_SHADER, src.fragment_shader_source_code);

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

Shader_program::Shader_program(const std::string& name, const Glsl_shader& vertex_shader, const Glsl_shader& pixel_shader)
	: _name(name)
{
	_id = glCreateProgram();
	glAttachShader(_id, vertex_shader.id());
	glAttachShader(_id, pixel_shader.id());
	glLinkProgram(_id);

	if (!linked()) {
		std::string msg_log = log();
		std::string exc_msg = EXCEPTION_MSG("Program linkage failed. '", _name, "'.");

		dispose();
		throw std::runtime_error(exc_msg);
	}

	glValidateProgram(_id);
	if (!valid()) {
		std::string msg_log = log();
		std::string exc_msg = EXCEPTION_MSG("Program validation failed. '", _name, "'.");

		dispose();
		throw std::runtime_error(exc_msg);
	}
}

Shader_program::Shader_program(const std::string& name, const Glsl_shader& vertex_shader, const std::string& pixel_source_code)
	: _name(name)
{
	assert(pixel_source_code.size() > 0);

	try {
		Glsl_shader pixel_shader(GL_FRAGMENT_SHADER, pixel_source_code);

		_id = glCreateProgram();
		glAttachShader(_id, vertex_shader.id());
		glAttachShader(_id, pixel_shader.id());
		glLinkProgram(_id);
		// once shaders have been linked into a program they are no longer needed.
	}
	catch (...) {
		std::string exc_msg = EXCEPTION_MSG("Error occured while creating the '", _name, "' shader program.");

		dispose();
		std::throw_with_nested(std::runtime_error(exc_msg));
	}

	if (!linked()) {
		std::string msg_log = log();
		std::string exc_msg = EXCEPTION_MSG("Program linkage failed. '", _name, "'.");

		dispose();
		throw std::runtime_error(exc_msg);
	}

	glValidateProgram(_id);
	if (!valid()) {
		std::string msg_log = log();
		std::string exc_msg = EXCEPTION_MSG("Program validation failed. '", _name, "'.");

		dispose();
		throw std::runtime_error(exc_msg);
	}
}

Shader_program::Shader_program(Shader_program&& prog) noexcept :
	_id(prog._id),
	_name(std::move(prog._name))
{
	prog._id = Invalid::shader_program_id;
}

Shader_program::~Shader_program() noexcept
{
	dispose();
}

Shader_program& Shader_program::operator=(Shader_program&& prog) noexcept
{
	if (this == &prog) return *this;

	dispose();

	_id = prog._id;
	_name = std::move(prog._name);

	prog._id = Invalid::shader_program_id;

	return *this;
}

void Shader_program::dispose() noexcept
{
	if (_id == Invalid::shader_program_id) return;

	glDeleteProgram(_id);
	_id = Invalid::shader_program_id;
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
	assert(_id != Invalid::shader_program_id);
	assert(uniform_name);

	GLint location = glGetUniformLocation(_id, uniform_name);
	ENFORCE(location != Invalid::uniform_location,
		"Shader program '", _name, "' does not have a uniform called '", uniform_name, "'.");

	return location;
}

bool Shader_program::linked() const noexcept
{
	assert(_id != Invalid::shader_program_id);
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
	assert(_id != Invalid::shader_program_id);
	return get_property(GL_VALIDATE_STATUS) != 0;
}

// ----- funcs -----

template<>
void set_uniform<float>(GLint location, const float& val) noexcept
{
	assert(location != Invalid::uniform_location);
	glUniform1f(location, val);
}

template<>
void set_uniform<uint2>(GLint location, const uint2& v) noexcept
{
	assert(location != Invalid::uniform_location);
	glUniform2ui(location, v.x, v.y);
}

template<>
void set_uniform<float3>(GLint location, const float3& v) noexcept
{
	assert(location != Invalid::uniform_location);
	glUniform3f(location, v.x, v.y, v.z);
}

template<>
void set_uniform<cg::mat3>(GLint location, const cg::mat3& mat) noexcept
{
	assert(location != Invalid::uniform_location);

	float arr[9];
	cg::to_array_column_major_order(mat, arr);
	glUniformMatrix3fv(location, 1, false, arr);
}

template<>
void set_uniform<cg::mat4>(GLint location, const cg::mat4& mat) noexcept
{
	assert(location != Invalid::uniform_location);

	float arr[16];
	cg::to_array_column_major_order(mat, arr);
	glUniformMatrix4fv(location, 1, false, arr);
}

template<>
void set_uniform_array<GLint, GLint>(GLint location, const GLint* ptr, size_t count) noexcept
{
	assert(location != Invalid::uniform_location);
	assert(count > 0);

	glUniform1iv(location, count, ptr);
}

template<>
void set_uniform_array<float, float>(GLint location, const float* ptr, size_t count) noexcept
{
	assert(location != Invalid::uniform_location);
	assert(count > 0);

	glUniform1fv(location, count, ptr);
}

template<>
void set_uniform_array<float3, float>(GLint location, const float* ptr, size_t count) noexcept
{
	assert(location != Invalid::uniform_location);
	assert(count > 0);

	glUniform3fv(location, count, ptr);
}

template<>
void set_uniform_array<mat3, mat3>(GLint location, const mat3* ptr, size_t count) noexcept
{
	assert(location != Invalid::uniform_location);
	assert(count > 0);

	constexpr size_t component_count = 9; // each mat3 has exactly 3 components
	std::vector<float> arr(count * component_count);
	for (size_t i = 0; i < count; ++i) {
		cg::to_array_column_major_order(ptr[i], arr.data() + i * component_count);
	}

	glUniformMatrix3fv(location, count, false, arr.data());
}

template<>
void set_uniform_array<mat4, float>(GLint location, const float* ptr, size_t count)
{
	assert(location != Invalid::uniform_location);
	assert(count > 0);
	glUniformMatrix4fv(location, count, false, ptr);
}

template<>
void set_uniform_array<mat4, mat4>(GLint location, const mat4* ptr, size_t count) noexcept
{
	assert(location != Invalid::uniform_location);
	assert(count > 0);

	constexpr size_t component_count = 16; // each mat4 has exactly 16 components
	std::vector<float> arr(count * component_count);
	for (size_t i = 0; i < count; ++i) {
		cg::to_array_column_major_order(ptr[i], arr.data() + i * component_count);
	}

	glUniformMatrix4fv(location, count, false, arr.data());
}

} // namespace rnd
} // namespace deferred_lighting
