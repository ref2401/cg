#ifndef CG_OPENGL_UTILITY_H_
#define CG_OPENGL_UTILITY_H_

#include "cg/data/shader.h"
#include "cg/opengl/opengl.h"


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

	GLint get_property(GLenum prop) const noexcept;

	void dispose() noexcept;

	GLuint _id = Shader::invalid_id;
	GLenum _type = Shader::invalid_type;
};

class Shader_program final {
public:

	static constexpr GLuint invalid_id = 0;


	Shader_program(const std::string& filename);

	Shader_program(const char* filename);

	Shader_program(const cg::data::Shader_program_source_code& src);

	~Shader_program() noexcept;



	GLuint id() const noexcept
	{
		return _id;
	}


private:
	GLuint _id = Shader_program::invalid_id;
};



} // namespace opengl
} // namespace cg

#endif // CG_OPENGL_UTILITY_H_