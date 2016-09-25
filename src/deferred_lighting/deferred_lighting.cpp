#include "deferred_lighting/deferred_lighting.h"

#include "cg/base/base.h"
#include "cg/file/file.h"

using cg::uint2;
using cg::megabytes;
using cg::data::Vertex_attribs;
using cg::opengl::Shader;
using cg::opengl::Shader_program;
using cg::opengl::Static_vertex_spec;
using cg::opengl::Static_vertex_spec_builder;
using cg::opengl::Vertex_attrib_layout;


namespace deferred_lighting {

Deferred_lighting::Deferred_lighting(uint2 window_size)
	: _indirect_buffer(megabytes(1)), _vertex_attrib_layout(0, 1, 2, 3)
{
	load_data();
	glViewport(0, 0, window_size.width, window_size.height);
}

void Deferred_lighting::load_data()
{
	//auto src = cg::file::load_glsl_program_source("../data/test");
	//_prog = std::make_unique<Shader_program>("test-shader", src);

	// loading models
	_renderable_objects.reserve(8);
	// square model
	auto mesh_data = cg::file::load_mesh_wavefront("../data/square_03.obj", Vertex_attribs::position);
	_vs_builder.begin(Vertex_attribs::position, cg::megabytes(4));
	_renderable_objects.push_back(_vs_builder.push_back(mesh_data));
	
	
	_vertex_spec = _vs_builder.end(_vertex_attrib_layout);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, _indirect_buffer.id());
}

void Deferred_lighting::render(float blend_state) 
{
	float rgb[4] = { 0.5f, 0.5f, 0.55f, 1.f };
	glClearBufferfv(GL_COLOR, 0, rgb);
	
	/*glBindVertexArray(_de_cmd.vao_id());
	glUseProgram(_prog->id());
	cg::opengl::draw_elements_base_vertex(_de_cmd);*/
}

} // namespace deferred_lighting