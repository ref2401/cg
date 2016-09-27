#include "technique/deferred_lighting/deferred_lighting.h"

#include <type_traits>
#include "cg/base/base.h"
#include "cg/file/file.h"

using cg::float3;
using cg::mat4;
using cg::uint2;

using cg::data::Vertex_attribs;
using cg::opengl::DE_cmd;


namespace deferred_lighting {

Deferred_lighting::Deferred_lighting(uint2 window_size)
{
	using cg::megabytes;
	using cg::perspective_matrix;
	using cg::translation_matrix;
	using cg::view_matrix;

	auto src = cg::file::load_glsl_program_source("../data/test");
	_renderer = std::make_unique<Renderer>(window_size, src);
	
	// scene
	// square geometry
	auto mesh_data = cg::file::load_mesh_wavefront("../data/square_03.obj", Vertex_attribs::position);
	_vs_builder.begin(Vertex_attribs::position, megabytes(4));
	DE_cmd cmd = _vs_builder.push_back(mesh_data);
	// ... load other geometry ...
	_vertex_spec = _vs_builder.end(_renderer->vertex_attrib_layout());
	_renderer->register_vao(_vertex_spec->vao_id(), _vertex_spec->vertex_buffer_binding_index() + 1);

	_frame = std::make_unique<Frame>(cmd.vao_id());
	_frame->set_projection_matrix(cg::perspective_matrix(cg::pi_3, window_size.aspect_ratio(), 1, 50));
	_frame->set_view_matrix(cg::view_matrix(float3(0, 0, 2), float3::zero));
	// put square three times as different models into frame
	_frame->push_back_renderable(cmd, translation_matrix(float3::zero));
	_frame->push_back_renderable(cmd, translation_matrix(float3(0.2f, 0.2f, 0)));
	_frame->push_back_renderable(cmd, translation_matrix(float3(0.4f, 0.4f, 0)));
}

void Deferred_lighting::render(float blend_state) 
{
	_renderer->render(*_frame.get());
}

} // namespace deferred_lighting