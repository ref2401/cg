#include "technique/deferred_lighting/deferred_lighting.h"

#include "cg/base/base.h"
#include "cg/file/file.h"

using cg::uint2;
using cg::kilobytes;
using cg::megabytes;
using cg::data::Vertex_attribs;
using cg::opengl::DE_cmd;
using cg::opengl::Shader;
using cg::opengl::Shader_program;
using cg::opengl::Static_vertex_spec;
using cg::opengl::Static_vertex_spec_builder;
using cg::opengl::Persistent_buffer;
using cg::opengl::Vertex_attrib_layout;
using cg::opengl::wait_for;


namespace deferred_lighting {

Deferred_lighting::Deferred_lighting(uint2 window_size)
	: _vertex_attrib_layout(0, 1, 2, 3)
{
	_indirect_buffer = std::make_unique<Persistent_buffer>(megabytes(1));
	_draw_index_buffer = std::make_unique<Persistent_buffer>(kilobytes(512));
	load_data();
	glViewport(0, 0, window_size.width, window_size.height);
}

void Deferred_lighting::load_data()
{
	// load shader programs
	auto src = cg::file::load_glsl_program_source("../data/test");
	_prog = std::make_unique<Shader_program>("test-shader", src);

	// load models
	_renderable_objects.reserve(8);
	// square model
	auto mesh_data = cg::file::load_mesh_wavefront("../data/square_03.obj", Vertex_attribs::position);
	_vs_builder.begin(Vertex_attribs::position, cg::megabytes(4));
	DE_cmd cmd = _vs_builder.push_back(mesh_data);
	_renderable_objects.push_back(cmd);
	_renderable_objects.push_back(cmd);
	_renderable_objects.push_back(cmd);
	
	// other models ...
	
	_vertex_spec = _vs_builder.end(_vertex_attrib_layout);
	// bind indirect buffer & draw index buffer to vao
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, _indirect_buffer->id());
	// draw_index attrib location == buffer binding index.
	GLuint di_binding_index = 10;
	glVertexArrayVertexBuffer(_vertex_spec->vao_id(), di_binding_index, _draw_index_buffer->id(), 0, sizeof(GLuint));
	glVertexArrayAttribBinding(_vertex_spec->vao_id(), di_binding_index, di_binding_index);
	glVertexArrayAttribFormat(_vertex_spec->vao_id(), di_binding_index, 1, GL_UNSIGNED_INT, false, 0);
	glVertexArrayBindingDivisor(_vertex_spec->vao_id(), di_binding_index, 1);
	glEnableVertexArrayAttrib(_vertex_spec->vao_id(), di_binding_index);
}

void Deferred_lighting::render(float blend_state) 
{
	wait_for(_frame_sync_obj);
	glDeleteSync(_frame_sync_obj);
	_frame_sync_obj = nullptr;


	float rgb[4] = { 0.5f, 0.5f, 0.55f, 1.f };
	glClearBufferfv(GL_COLOR, 0, rgb);
	glUseProgram(_prog->id());
	glBindVertexArray(_vertex_spec->vao_id());
	
	size_t offset_indirect = 0;
	size_t offset_draw_index = 0;
	for (size_t i = 0; i < _renderable_objects.size(); ++i) {
		auto params = _renderable_objects[i].get_indirect_params();
		params.base_instance = i; // base index is required to calculate an index to draw_index_buffer
		offset_indirect = _indirect_buffer->write(offset_indirect, &params);
		offset_draw_index = _draw_index_buffer->write<GLuint>(offset_draw_index, &i);
	}

	glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, 0, _renderable_objects.size(), 0);
	_frame_sync_obj = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
}

} // namespace deferred_lighting