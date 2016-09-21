#include "deferred_lighting/deferred_lighting.h"

#include "cg/file/file.h"

using cg::opengl::Shader;
using cg::opengl::Shader_program;

namespace deferred_lighting {

Deferred_lighting::Deferred_lighting()
{
	auto src_v = cg::file::load_text("../data/test.vertex.glsl");
	auto src_p = cg::file::load_text("../data/test.pixel.glsl");
	Shader s(GL_VERTEX_SHADER, src_v);
	Shader s1 = std::move(s);
	s1 = Shader(GL_FRAGMENT_SHADER, src_p);
}

void Deferred_lighting::render(float blend_state) 
{
	float rgb[4] = { 0.5f, 0.5f, 0.55f, 1.f };
	glClearBufferfv(GL_COLOR, 0, rgb);
}

} // namespace deferred_lighting