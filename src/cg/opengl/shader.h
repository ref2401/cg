#ifndef CG_OPENGL_SHADER_H_
#define CG_OPENGL_SHADER_H_

#include "cg/data/shader.h"
#include "cg/opengl/opengl_def.h"


namespace cg {
namespace opengl {

class Shader final {
public:

	static constexpr GLuint invalid_id = 0;

	static constexpr GLenum invalid_type = GL_NONE;


	Shader(GLenum type, const std::string& source_code);

	Shader(const Shader& shader) = delete;

	Shader(Shader&& shader) noexcept;

	~Shader() noexcept;

	
	Shader& operator=(const Shader& shader) = delete;

	Shader& operator=(Shader&& shader) noexcept;


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

	GLuint _id = Shader::invalid_id;
	GLenum _type = Shader::invalid_type;
};

class Shader_program final {
public:

	static constexpr GLuint invalid_id = 0;


	Shader_program(const std::string& name, const cg::data::Shader_program_source_code& src);

	Shader_program(const std::string& name, const Shader& vertex_shader, const Shader& pixel_shader);

	Shader_program(const std::string& name, const Shader& vertex_shader, const std::string& pixel_source_code);

	Shader_program(const Shader_program& prog) = delete;

	Shader_program(Shader_program&& prog) noexcept;

	~Shader_program() noexcept;

	Shader_program& operator=(const Shader_program& prog) = delete;

	Shader_program& operator=(Shader_program&& prog) noexcept;


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


	GLuint _id = Shader_program::invalid_id;
	std::string _name;
};



} // namespace opengl
} // namespace cg

#endif // CG_OPENGL_SHADER_H_