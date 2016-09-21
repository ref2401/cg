#include "deferred_lighting/deferred_lighting.h"

#include "cg/file/file.h"

using cg::opengl::Shader;
using cg::opengl::Shader_program;


namespace deferred_lighting {

Deferred_lighting::Deferred_lighting()
{
	auto src = cg::file::load_glsl_program_source("../data/test");
	_program = std::make_unique<Shader_program>("test", src);

	glCreateVertexArrays(1, &_vao_id);
	glBindVertexArray(_vao_id);
	glPointSize(8);
}

void Deferred_lighting::render(float blend_state) 
{
	float rgb[4] = { 0.5f, 0.5f, 0.55f, 1.f };
	glClearBufferfv(GL_COLOR, 0, rgb);
	glUseProgram(_program->id());
	glDrawArrays(GL_POINTS, 0, 1);
}

} // namespace deferred_lighting