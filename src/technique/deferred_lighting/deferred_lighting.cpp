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


namespace {

using deferred_lighting::Renderer_config;

Renderer_config make_render_config(uint2 viewport_size)
{
	auto gbuffer_pass_code = cg::file::load_glsl_program_source("../data/deferred_lighting/gbuffer_pass");

	return Renderer_config(viewport_size, gbuffer_pass_code);
}

} // namespace


namespace deferred_lighting {

// ----- Material -----

Material::Material(Texture_2d tex_normal_map) noexcept :
	tex_normal_map(std::move(tex_normal_map))
{}

// ----- Deferred_lighting -----

Deferred_lighting::Deferred_lighting(uint2 window_size) :
	_projection_matrix(cg::perspective_matrix(cg::pi_3, window_size.aspect_ratio(), 1, 50)),
	_view_matrix(cg::view_matrix(float3(0, 0, 5), float3::zero)),
	_renderer(make_render_config(window_size)),
	_frame(16)
{
	using cg::from_axis_angle_rotation;
	using cg::megabytes;
	using cg::tr_matrix;
	using cg::translation_matrix;

	// scene
	// materials
	auto normal_map_image = cg::file::load_image_tga("../data/normal_map.tga");
	_material.tex_normal_map = Texture_2d(Texture_format::rgb_8, normal_map_image);

	auto default_normal_map_image = cg::file::load_image_tga("../data/material-default-normal_map.tga");
	_tex_default_normal_map = Texture_2d(Texture_format::rgb_8, default_normal_map_image);

	// cube geometry
	auto vertex_attribs = Vertex_attribs::mesh_textured | Vertex_attribs::normal;
	auto cube_mesh_data = cg::file::load_mesh_wavefront("../data/cube.obj", vertex_attribs);
	auto square_mesh_data = cg::file::load_mesh_wavefront("../data/square_03.obj", vertex_attribs);
	_vs_builder.begin(vertex_attribs, megabytes(4));
	DE_cmd cube_cmd = _vs_builder.push_back(cube_mesh_data);
	DE_cmd square_cmd = _vs_builder.push_back(square_mesh_data);
	// ... load other geometry ...
	_vertex_spec0 = _vs_builder.end(_renderer.vertex_attrib_layout());

	// scene objects
	_rednerable_objects.reserve(16);
	_rednerable_objects.emplace_back(cube_cmd, 
		trs_matrix(float3::zero, from_axis_angle_rotation(float3::unit_y, cg::pi_4), float3(2)),
		_material.tex_normal_map.id());
	
	_rednerable_objects.emplace_back(square_cmd, 
		translation_matrix(float3(-2.f, 1, 1)),
		_material.tex_normal_map.id());

	_rednerable_objects.emplace_back(square_cmd, 
		tr_matrix(float3(2.f, -1, 1), from_axis_angle_rotation(float3::unit_y, -cg::pi_8)),
		_tex_default_normal_map.id());
}

void Deferred_lighting::render(float blend_state) 
{
	_frame.reset(*_vertex_spec0.get());

	_frame.set_projection_matrix(_projection_matrix);
	_frame.set_view_matrix(_view_matrix);

	for (const auto& rnd : _rednerable_objects) {
		_frame.push_back_renderable(rnd);
	}
	
	_frame.begin_rendering();
	_renderer.render(_frame);
	_frame.end_rendering();
}

} // namespace deferred_lighting