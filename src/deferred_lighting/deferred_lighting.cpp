#include "deferred_lighting/deferred_lighting.h"

#include "cg/base/base.h"
#include "cg/file/file.h"

using cg::data::Vertex_attribs;
using cg::opengl::Shader;
using cg::opengl::Shader_program;
using cg::opengl::Static_vertex_spec_builder;


namespace deferred_lighting {

Deferred_lighting::Deferred_lighting()
{
	auto mesh_data = cg::file::load_mesh_wavefront("../data/cube-vnt.obj", Vertex_attribs::mesh_textured);
	Static_vertex_spec_builder vs_builder;

	vs_builder.begin(Vertex_attribs::mesh_textured, cg::megabytes(4));
	vs_builder.push_back(mesh_data);
	vs_builder.push_back(mesh_data);
	vs_builder.end();
}

void Deferred_lighting::render(float blend_state) 
{
	float rgb[4] = { 0.5f, 0.5f, 0.55f, 1.f };
	glClearBufferfv(GL_COLOR, 0, rgb);
	
}

} // namespace deferred_lighting