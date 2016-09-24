#include "deferred_lighting/deferred_lighting.h"

#include "cg/base/base.h"
#include "cg/file/file.h"

using cg::uint2;
using cg::data::Vertex_attribs;
using cg::opengl::Shader;
using cg::opengl::Shader_program;
using cg::opengl::Static_vertex_spec;
using cg::opengl::Static_vertex_spec_builder;
using cg::opengl::Vertex_attrib_layout;


namespace deferred_lighting {

Deferred_lighting::Deferred_lighting(uint2 window_size)
{
	auto src = cg::file::load_glsl_program_source("../data/test");
	_prog = std::make_unique<Shader_program>("test-shader", src);

	Vertex_attrib_layout attrib_layout(0, 1, 2, 3);
	auto mesh_data = cg::file::load_mesh_wavefront("../data/square_03.obj", Vertex_attribs::position);
	Static_vertex_spec_builder vs_builder;

	vs_builder.begin(Vertex_attribs::position, cg::megabytes(4));
	_de_cmd = vs_builder.push_back(mesh_data);
	_vertex_spec = std::make_unique<Static_vertex_spec>(vs_builder.end(attrib_layout));
	glViewport(0, 0, window_size.width, window_size.height);
}

void Deferred_lighting::render(float blend_state) 
{
	float rgb[4] = { 0.5f, 0.5f, 0.55f, 1.f };
	glClearBufferfv(GL_COLOR, 0, rgb);
	
	glBindVertexArray(_de_cmd.vao_id());
	glUseProgram(_prog->id());
	cg::opengl::draw_elements_base_vertex(_de_cmd);
}

} // namespace deferred_lighting