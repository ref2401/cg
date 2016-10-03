#include "technique/deferred_lighting/deferred_lighting.h"

#include <type_traits>
#include "cg/base/base.h"
#include "cg/file/file.h"

using cg::float3;
using cg::mat3;
using cg::mat4;
using cg::uint2;

using cg::data::Vertex_attribs;
using cg::opengl::DE_cmd;
using cg::opengl::Texture_2d;
using cg::opengl::Texture_format;


namespace deferred_lighting {

Deferred_lighting::Deferred_lighting(uint2 window_size) :
	_projection_matrix(cg::perspective_matrix(cg::pi_3, window_size.aspect_ratio(), 1, 50)),
	_view_matrix(cg::view_matrix(float3(0, 0, 5), float3::zero))
{
	using cg::from_axis_angle_rotation;
	using cg::megabytes;
	using cg::perspective_matrix;
	using cg::tr_matrix;
	using cg::translation_matrix;
	using cg::view_matrix;

	auto src = cg::file::load_glsl_program_source("../data/test");
	_renderer = std::make_unique<Renderer>(window_size, src);

	// scene
	// materials

	// cube geometry
	auto vertex_attribs = Vertex_attribs::mesh_textured | Vertex_attribs::normal;
	auto mesh_data = cg::file::load_mesh_wavefront("../data/cube.obj", vertex_attribs);
	_vs_builder.begin(vertex_attribs, megabytes(4));
	DE_cmd cmd = _vs_builder.push_back(mesh_data);
	// ... load other geometry ...
	_vertex_spec0 = _vs_builder.end(_renderer->vertex_attrib_layout());
	_renderer->register_vao(_vertex_spec0->vao_id(), _vertex_spec0->vertex_buffer_binding_index() + 1);

}

void Deferred_lighting::render(float blend_state) 
{
	_frame.begin();

	_frame.set_projection_matrix(_projection_matrix);
	_frame.set_view_matrix(_view_matrix);

	// populate the frame here...
	//_frame->push_back_renderable(cmd, 
	//	trs_matrix(float3(-0.2f, -0.2f, 0), from_axis_angle_rotation(float3::unit_y, cg::pi_4), float3(2)));
	_frame.end();
	
	_renderer->render(_frame);
}

} // namespace deferred_lighting