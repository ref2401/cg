#ifndef CG_OPENGL_UTILITY_H_
#define CG_OPENGL_UTILITY_H_

#include "cg/data/shader.h"
#include "cg/opengl/opengl.h"


namespace cg {
namespace opengl {

class Program final {
public:

	static constexpr GLuint invalid_id = 0;


	Program(const std::string& filename);

	Program(const char* filename);

	Program(const cg::data::Shader_program_source_code& src);

	~Program() noexcept;



	GLuint id() const noexcept
	{
		return _id;
	}


private:
	GLuint _id = Program::invalid_id;
};



} // namespace opengl
} // namespace cg

#endif // CG_OPENGL_UTILITY_H_