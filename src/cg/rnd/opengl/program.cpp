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

Glsl_program::Glsl_program(const cg::data::Glsl_compute_desc& compute_desc)
	: _name(compute_desc.name)
{
	assert(!compute_desc.name.empty());
	assert(!compute_desc.compute_shader_source_code.empty());

	try {
		Glsl_shader shader(GL_COMPUTE_SHADER, compute_desc.compute_shader_source_code);

		_id = glCreateProgram();
		glAttachShader(_id, shader.id());

		link(*this);
		validate(*this);

		// once shaders have been linked into a program they are no longer needed.
		glDetachShader(_id, shader.id()); 
	}
	catch (...) {
		std::string exc_msg = EXCEPTION_MSG("Error occured while creating the '", _name, "' compute shader program.");

		dispose();
		std::throw_with_nested(std::runtime_error(exc_msg));
	}
}

Glsl_program::Glsl_program(const cg::data::Glsl_program_desc& prog_desc)
	: _name(prog_desc.name)
{
	assert(!prog_desc.name.empty());
	assert(prog_desc.has_vertex_shader());

	try {
		_id = glCreateProgram();

		// vertex shader
		Glsl_shader vertex_shader(GL_VERTEX_SHADER, prog_desc.vertex_shader_source_code);
		glAttachShader(_id, vertex_shader.id());

		// fragment shader
		Glsl_shader fragment_shader;
		if (prog_desc.has_fragment_shader()) {
			fragment_shader = Glsl_shader(GL_FRAGMENT_SHADER, prog_desc.fragment_shader_source_code);
			glAttachShader(_id, fragment_shader.id());
		}

		// transform feedback
		if (prog_desc.transform_feedback_is_used()) {
			const GLsizei varying_count = GLsizei(prog_desc.tf_varying_names.size());
			const GLenum buffer_mode = (prog_desc.tf_interleaved_buffer_mode)
				? (GL_INTERLEAVED_ATTRIBS) : (GL_SEPARATE_ATTRIBS);

			const char** varying_names = new const char*[varying_count];
			const char** tmp = varying_names;
			for (const auto& name : prog_desc.tf_varying_names) {
				*tmp = name.c_str();
				++tmp;
			}
			
			glTransformFeedbackVaryings(_id, varying_count, varying_names, buffer_mode);

			delete[] varying_names;
		}

		link(*this);
		validate(*this);

		// once shaders have been linked into a program they are no longer needed.
		glDetachShader(_id, vertex_shader.id());
		if (prog_desc.has_fragment_shader()) glDetachShader(_id, fragment_shader.id());
	}
	catch (...) {
		std::string exc_msg = EXCEPTION_MSG("Error occured while creating the '", _name, "' shader program.");

		dispose();
		std::throw_with_nested(std::runtime_error(exc_msg));
	}
}

Glsl_program::Glsl_program(Glsl_program&& prog) noexcept
	: _id(prog._id),
	_name(std::move(prog._name))
{
	prog._id = Blank::program_id;
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

	prog._id = Blank::program_id;

	return *this;
}

void Glsl_program::dispose() noexcept
{
	if (_id == Blank::program_id) return;

	glDeleteProgram(_id);
	_id = Blank::program_id;
	_name.clear();
}

// ----- Glsl_shader -----

Glsl_shader::Glsl_shader(GLenum type, const std::string& source_code)
	: _type(type)
{
	assert(is_valid_shader_type(type));
	assert(!source_code.empty());

	_id = glCreateShader(_type);
	const char* source_ptr = source_code.c_str();
	glShaderSource(_id, 1, &source_ptr, nullptr);
	glCompileShader(_id);
	if (compile_status(*this)) return; // The compilation succeeded.


	// Glsl_shader compilation error handling
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
		
	std::string log_msg = info_log(*this);
	std::string exc_msg = EXCEPTION_MSG(type_name, " shader compilation error.\n", log_msg);

	dispose();
	throw std::runtime_error(exc_msg);
}

Glsl_shader::Glsl_shader(Glsl_shader&& shader) noexcept
	: _id(shader._id),
	_type(shader._type)
{
	shader._id = Blank::shader_id;
	shader._type = GL_NONE;
}

Glsl_shader::~Glsl_shader() noexcept
{
	dispose();
}

Glsl_shader& Glsl_shader::operator=(Glsl_shader&& shader) noexcept
{
	if (this == &shader) return *this;

	dispose();
	_id = shader._id;
	_type = shader._type;

	shader._id = Blank::shader_id;
	shader._type = GL_NONE;

	return *this;
}

void Glsl_shader::dispose() noexcept
{
	if (_id == Blank::shader_id) return;

	glDeleteShader(_id);
	_id = Blank::shader_id;
	_type = GL_NONE;
}

// ----- funcs -----

bool compile_status(const Glsl_shader& shader) noexcept
{
	assert(shader.id() != Blank::shader_id);
	return parameter(shader, GL_COMPILE_STATUS) == GL_TRUE;
}

std::string info_log(const Glsl_program& program) noexcept
{
	assert(program.id() != Blank::program_id);

	GLint length = parameter(program, GL_INFO_LOG_LENGTH);
	if (length == 0) return{};

	GLsizei actual_length;
	char* msg_buffer = new char[length];
	glGetProgramInfoLog(program.id(), length, &actual_length, msg_buffer);
	std::string msg(msg_buffer, actual_length);
	delete[] msg_buffer;

	return msg;
}

std::string info_log(const Glsl_shader& shader) noexcept
{
	assert(shader.id() != Blank::shader_id);

	GLint length = parameter(shader, GL_INFO_LOG_LENGTH);
	if (length == 0) return{};

	GLsizei actual_length = 0;
	char* msg_buffer = new char[length];
	glGetShaderInfoLog(shader.id(), length, &actual_length, msg_buffer);
	std::string msg(msg_buffer, actual_length);
	delete[] msg_buffer;

	return msg;
}

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

bool is_valid_program_parameter(GLenum pname) noexcept
{
	return pname == GL_ACTIVE_ATTRIBUTES
		|| pname == GL_ACTIVE_ATTRIBUTE_MAX_LENGTH
		|| pname == GL_ACTIVE_UNIFORM_BLOCKS
		|| pname == GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH
		|| pname == GL_ACTIVE_UNIFORMS
		|| pname == GL_ACTIVE_UNIFORM_MAX_LENGTH
		|| pname == GL_ATTACHED_SHADERS
		|| pname == GL_DELETE_STATUS
		|| pname == GL_INFO_LOG_LENGTH
		|| pname == GL_LINK_STATUS
		|| pname == GL_PROGRAM_BINARY_RETRIEVABLE_HINT
		|| pname == GL_TRANSFORM_FEEDBACK_BUFFER_MODE
		|| pname == GL_TRANSFORM_FEEDBACK_VARYINGS
		|| pname == GL_TRANSFORM_FEEDBACK_VARYING_MAX_LENGTH
		|| pname == GL_VALIDATE_STATUS;
}

bool is_valid_shader_parameter(GLenum pname) noexcept
{
	return pname == GL_SHADER_TYPE
		|| pname == GL_DELETE_STATUS
		|| pname == GL_COMPILE_STATUS
		|| pname == GL_INFO_LOG_LENGTH
		|| pname == GL_SHADER_SOURCE_LENGTH;
}

void link(const Glsl_program& program)
{
	assert(program.id() != Blank::program_id);

	glLinkProgram(program.id());
	if (!link_status(program)) {
		std::string msg_log = info_log(program);
		std::string exc_msg = EXCEPTION_MSG("Program linkage failed. '",
			program.name(), "'.\n", msg_log);

		throw std::runtime_error(exc_msg);
	}
}

bool link_status(const Glsl_program& program) noexcept
{
	assert(program.id());
	return parameter(program, GL_LINK_STATUS) == GL_TRUE;
}

GLint parameter(const Glsl_program& program, GLenum pname) noexcept
{
	assert(program.id() != Blank::program_id);
	assert(is_valid_program_parameter(pname));

	GLint v = -1; // opengl: If an error is generated, no change is made to the contents of value.
	glGetProgramiv(program.id(), pname, &v);
	assert(v != -1);

	return v;
}

GLint parameter(const Glsl_shader& shader, GLenum pname) noexcept
{
	assert(shader.id() != Blank::shader_id);
	assert(is_valid_shader_parameter(pname));

	GLint v = -1; // opengl: If an error is generated, no change is made to the contents of value.
	glGetShaderiv(shader.id(), pname, &v);
	assert(v != -1);

	return v;
}

void set_uniform(GLint location, unsigned int val) noexcept
{
	assert(location != Blank::uniform_location);
	glUniform1ui(location, val);
}

void set_uniform(GLint location, float val) noexcept
{
	assert(location != Blank::uniform_location);
	glUniform1f(location, val);
}

void set_uniform(GLint location, const uint2& v) noexcept
{
	assert(location != Blank::uniform_location);
	glUniform2ui(location, v.x, v.y);
}

void set_uniform(GLint location, const float2& v) noexcept
{
	assert(location != Blank::uniform_location);
	glUniform2f(location, v.x, v.y);
}

void set_uniform(GLint location, const float3& v) noexcept
{
	assert(location != Blank::uniform_location);
	glUniform3f(location, v.x, v.y, v.z);
}

void set_uniform(GLint location, const float4& v) noexcept
{
	assert(location != Blank::uniform_location);
	glUniform4f(location, v.x, v.y, v.z, v.w);
}

void set_uniform(GLint location, const float3x3& mat) noexcept
{
	assert(location != Blank::uniform_location);

	float arr[9];
	to_array_column_major_order(mat, arr);
	glUniformMatrix3fv(location, 1, false, arr);
}

void set_uniform(GLint location, const float4x4& mat) noexcept
{
	assert(location != Blank::uniform_location);

	float arr[16];
	to_array_column_major_order(mat, arr);
	glUniformMatrix4fv(location, 1, false, arr);
}

void set_uniform_array_int(GLint location, const GLint* ptr, GLsizei count) noexcept
{
	assert(location != Blank::uniform_location);
	assert(count > 0);

	glUniform1iv(location, count, ptr);
}

void set_uniform_array_float(GLint location, const float* ptr, GLsizei count) noexcept
{
	assert(location != Blank::uniform_location);
	assert(count > 0);

	glUniform1fv(location, count, ptr);
}

void set_uniform_array_float3(GLint location, const float* ptr, GLsizei count) noexcept
{
	assert(location != Blank::uniform_location);
	assert(count > 0);

	glUniform3fv(location, count, ptr);
}

void set_uniform_array_mat3(GLint location, const float3x3* ptr, GLsizei count)
{
	assert(location != Blank::uniform_location);
	assert(count > 0);

	constexpr size_t component_count = 9; // each mat3 has exactly 9 components

	std::vector<float> arr(count * component_count);
	for (size_t i = 0; i < count; ++i)
		to_array_column_major_order(ptr[i], arr.data() + i * component_count);

	glUniformMatrix3fv(location, count, false, arr.data());
}

void set_uniform_array_mat4(GLint location, const float4x4* ptr, GLsizei count)
{
	assert(location != Blank::uniform_location);
	assert(count > 0);

	constexpr size_t component_count = 16; // each mat4 has exactly 16 components
	
	std::vector<float> arr(count * component_count);
	for (size_t i = 0; i < count; ++i)
		to_array_column_major_order(ptr[i], arr.data() + i * component_count);

	glUniformMatrix4fv(location, count, false, arr.data());
}

void set_uniform_array_mat4(GLint location, const float* ptr, GLsizei count) noexcept
{
	assert(location != Blank::uniform_location);
	assert(count > 0);

	glUniformMatrix4fv(location, count, false, ptr);
}

GLint uniform_location(const Glsl_program& program, const char* uniform_name)
{
	assert(program.id() != Blank::program_id);
	assert(uniform_name && std::strlen(uniform_name) > 0);

	GLint location = glGetUniformLocation(program.id(), uniform_name);
	ENFORCE(location != Blank::uniform_location,
		"Shader program '", program.name(), "' does not have a uniform called '", uniform_name, "'.");

	return location;
}

void validate(const Glsl_program& program)
{
	assert(program.id() != Blank::program_id);

	glValidateProgram(program.id());
	if (!validate_status(program)) {
		std::string msg_log = info_log(program);
		std::string exc_msg = EXCEPTION_MSG("Program validation failed. '",
			program.name(), "'.\n", msg_log);

		throw std::runtime_error(exc_msg);
	}
}

bool validate_status(const Glsl_program& program) noexcept
{
	assert(program.id());
	return parameter(program, GL_VALIDATE_STATUS) == GL_TRUE;
}

} // namespace opengl
} // namespace rnd
} // namespace cg
