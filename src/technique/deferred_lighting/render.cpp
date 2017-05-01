#include "technique/deferred_lighting/render.h"

#include <cassert>
#include <algorithm>
#include <numeric>
#include <memory>

using namespace cg;
using namespace cg::rnd::opengl;
using cg::data::Model_geometry_data;
using cg::data::Glsl_program_desc;
using cg::data::Vertex_attribs;

namespace {

uint2 get_scaled_viewport_size(const uint2& viewport_size, float scale) noexcept
{
	assert(scale > 0.0f);

	float width = viewport_size.width * scale;
	float height = viewport_size.height * scale;
	return uint2(uint32_t(width), uint32_t(height));
}

} // namespace


namespace deferred_lighting {

// ----- Gbuffer -----

Gbuffer::Gbuffer(const uint2& viewport_size,
	const Vertex_attrib_layout& vertex_attrib_layout,
	const Model_geometry_data<Vertex_attribs::p_tc>& rect_1x1_mesh_data) :
	_vertex_attrib_layout(vertex_attrib_layout),
	_bilinear_sampler(Sampler_desc(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE)),
	_nearest_sampler(Sampler_desc(GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE)),
	_viewport_size(viewport_size),
	_aux_depth_renderbuffer(GL_DEPTH_COMPONENT32F, viewport_size),
	_tex_aux_render_target(GL_RGBA32F, 1, viewport_size),
	_tex_lighting_ambient_term(GL_RGB32F, 1, viewport_size),
	_tex_lighting_diffuse_term(GL_RGB32F, 1, viewport_size),
	_tex_lighting_specular_term(GL_RGB32F, 1, viewport_size),
	_tex_material_lighting_result(GL_RGB32F, 1 ,viewport_size),
	_tex_nds(GL_RGBA32F, 1, viewport_size),
	_tex_shadow_map(GL_RG32F, 1, viewport_size),
	_tex_ssao_map(GL_R32F, 1, get_scaled_viewport_size(viewport_size, 0.5f)),
	_tex_ssao_map_aux(GL_R32F, 1, get_scaled_viewport_size(viewport_size, 0.5f))
{
	Static_vertex_spec_builder vs_builder(8, 8);
	vs_builder.begin<Vertex_attribs::p_tc>(kilobytes(1));
	{
		auto rect_1x1_cmd = vs_builder.push_back(rect_1x1_mesh_data);
		_aux_geometry_rect_1x1_params = rect_1x1_cmd.get_base_vertex_params();
	}
	_aux_geometry_vertex_spec = vs_builder.end(_vertex_attrib_layout, true);
}

void Gbuffer::resize(const uint2& viewport_size) noexcept
{
	_viewport_size = viewport_size;
	uint2 scaled_size = get_scaled_viewport_size(viewport_size, 0.5f);

	_aux_depth_renderbuffer.set_size(_viewport_size);
	_tex_aux_render_target.set_size(_viewport_size);
	_tex_lighting_ambient_term.set_size(_viewport_size);
	_tex_lighting_diffuse_term.set_size(_viewport_size);
	_tex_lighting_specular_term.set_size(_viewport_size);
	_tex_material_lighting_result.set_size(_viewport_size);
	_tex_nds.set_size(_viewport_size);
	_tex_shadow_map.set_size(_viewport_size);
	_tex_ssao_map.set_size(scaled_size);
	_tex_ssao_map_aux.set_size(scaled_size);
}

// ----- Gbuffer_pass -----

Gbuffer_pass::Gbuffer_pass(Gbuffer& gbuffer, const cg::data::Glsl_program_desc& source_code) :
	_prog(source_code),
	_gbuffer(gbuffer)
{
	attach_color_target(_fbo, GL_COLOR_ATTACHMENT0, _gbuffer.tex_nds());
	attach_depth_target(_fbo, _gbuffer.aux_depth_renderbuffer());
	set_draw_buffer(_fbo, GL_COLOR_ATTACHMENT0);
	set_read_buffer(_fbo, GL_NONE);
	validate(_fbo);
}

void Gbuffer_pass::begin(const cg::mat4& projection_matrix, const cg::mat4& view_matrix) noexcept
{
	glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbo.id());
	glViewport(0, 0, _gbuffer.viewport_size().width, _gbuffer.viewport_size().height);
	glClearBufferfv(GL_COLOR, 0, _clear_value_nds);
	glClearBufferfv(GL_DEPTH, 0, &_clear_value_depth_map);

	_prog.use(projection_matrix, view_matrix);
}

void Gbuffer_pass::end() noexcept
{
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, Blank::framebuffer_id);
}

void Gbuffer_pass::set_uniform_arrays(size_t rnd_offset, size_t rnd_count,
	const std::vector<float>& uniform_array_model_matrix,
	const std::vector<float>& uniform_array_smoothness,
	const std::vector<GLuint>& uniform_array_tex_normal_map) noexcept
{
	_prog.set_uniform_array_model_matrix(uniform_array_model_matrix.data() + rnd_offset * 16, GLsizei(rnd_count));
	_prog.set_uniform_array_smoothness(uniform_array_smoothness.data() + rnd_offset, GLsizei(rnd_count));

	for (GLuint curr_unit = 0; curr_unit < rnd_count; ++curr_unit) {
		glBindSampler(curr_unit, Blank::sampler_id);
		glBindTextureUnit(curr_unit, uniform_array_tex_normal_map[rnd_offset + curr_unit]);
	}
}

// ----- Lighting_pass -----

Lighting_pass::Lighting_pass(Gbuffer& gbuffer, const cg::data::Glsl_program_desc& dir_source_code) :
	_gbuffer(gbuffer),
	_dir_prog(dir_source_code)
{
	attach_color_target(_fbo, GL_COLOR_ATTACHMENT0, _gbuffer.tex_lighting_ambient_term());
	attach_color_target(_fbo, GL_COLOR_ATTACHMENT1, _gbuffer.tex_lighting_diffuse_term());
	attach_color_target(_fbo, GL_COLOR_ATTACHMENT2, _gbuffer.tex_lighting_specular_term());
	set_draw_buffers(_fbo, GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2);
	set_read_buffer(_fbo, GL_NONE);
	validate(_fbo);
}

void Lighting_pass::begin() noexcept
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbo.id());
	glViewport(0, 0, _gbuffer.viewport_size().width, _gbuffer.viewport_size().height);
	glClearColor(_clear_value_color.x, _clear_value_color.y, _clear_value_color.z, _clear_value_color.y);
	glClear(GL_COLOR_BUFFER_BIT);

	glBindSampler(0, _gbuffer.nearest_sampler().id());
	glBindTextureUnit(0, _gbuffer.tex_nds().id());
}

void Lighting_pass::end() noexcept
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, Blank::framebuffer_id);
}

void Lighting_pass::perform_directional_light_pass(const std::array<float3, 4>& far_plane_coords, 
	const Directional_light_params& dir_light) noexcept
{
	_dir_prog.use(dir_light);
	_dir_prog.set_uniform_array_far_plane_coords(far_plane_coords);

	glBindVertexArray(_gbuffer.aux_geometry_vao_id());
	draw_elements_base_vertex(_gbuffer.aux_geometry_rect_1x1_params());
}

// ----- Material_lighting_pass -----

Material_lighting_pass::Material_lighting_pass(Gbuffer& gbuffer, const cg::data::Glsl_program_desc& source_code) :
	_gbuffer(gbuffer),
	_prog(source_code)
{
	attach_color_target(_fbo, GL_COLOR_ATTACHMENT0, _gbuffer.tex_material_lighting_result());
	set_draw_buffer(_fbo, GL_COLOR_ATTACHMENT0);
	set_read_buffer(_fbo, GL_NONE);
	validate(_fbo);
}

void Material_lighting_pass::begin(const mat4& projection_matrix, const mat4& view_matrix,
	const Directional_light_params& dir_light) noexcept
{
	glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbo.id());
	glViewport(0, 0, _gbuffer.viewport_size().width, _gbuffer.viewport_size().height);
	glClearBufferfv(GL_COLOR, 0, &_clear_value_color.x);


	_prog.use(projection_matrix, view_matrix, dir_light);
	// ambient term
	glBindSampler(26, _gbuffer.nearest_sampler().id());
	glBindTextureUnit(26, _gbuffer.tex_lighting_ambient_term().id());
	// diffuse term
	glBindSampler(27, _gbuffer.nearest_sampler().id());
	glBindTextureUnit(27, _gbuffer.tex_lighting_diffuse_term().id());
	// specular term
	glBindSampler(28, _gbuffer.nearest_sampler().id());
	glBindTextureUnit(28, _gbuffer.tex_lighting_specular_term().id());
	// nds (normal, depth, smoothness)
	glBindSampler(29, _gbuffer.nearest_sampler().id());
	glBindTextureUnit(29, _gbuffer.tex_nds().id());
	// shadow map
	glBindSampler(30, _gbuffer.bilinear_sampler().id());
	glBindTextureUnit(30, _gbuffer.tex_shadow_map().id());
	// ssao map
	glBindSampler(31, _gbuffer.bilinear_sampler().id());
	glBindTextureUnit(31, _gbuffer.tex_ssao_map().id());
}

void Material_lighting_pass::end() noexcept
{
	glDisable(GL_CULL_FACE);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, Blank::framebuffer_id);

	//_fbo.set_read_buffer(GL_COLOR_ATTACHMENT0);
	//glBlitNamedFramebuffer(_fbo.id(), Blank::framebuffer_id,
	//	0, 0, _gbuffer.viewport_size().width, _gbuffer.viewport_size().height,
	//	0, 0, _gbuffer.viewport_size().width, _gbuffer.viewport_size().height,
	//	GL_COLOR_BUFFER_BIT, GL_LINEAR);
	//_fbo.set_read_buffer(GL_NONE);
}

void Material_lighting_pass::set_uniform_arrays(size_t rnd_offset, size_t rnd_count,
	const std::vector<float>& uniform_array_model_matrix,
	const std::vector<GLuint>& uniform_array_tex_diffuse_rgb,
	const std::vector<GLuint>& uniform_array_tex_specular_intensity) noexcept
{
	_prog.set_uniform_array_model_matrix(uniform_array_model_matrix.data() + rnd_offset * 16, rnd_count);

	for (GLuint curr_unit = 0; curr_unit < rnd_count; ++curr_unit) {
		// 13 - the size of every uniform array of sampler2D (material_pass.pixel.glsl)
		glBindSampler(curr_unit, Blank::sampler_id);
		glBindSampler(curr_unit + 13, Blank::sampler_id);
		glBindTextureUnit(curr_unit, uniform_array_tex_diffuse_rgb[rnd_offset + curr_unit]);
		glBindTextureUnit(curr_unit + 13, uniform_array_tex_specular_intensity[rnd_offset + curr_unit]);
	}
}

// ----- Shadow_map_pass -----

Shadow_map_pass::Shadow_map_pass(Gbuffer& gbuffer, const cg::data::Glsl_program_desc& source_code) :
	_gbuffer(gbuffer),
	_prog(source_code),
	_filter_shader_program(Filter_type::gaussian, Filter_kernel_radius::radius_03)
{
	attach_color_target(_fbo, GL_COLOR_ATTACHMENT0, _gbuffer.tex_shadow_map());
	attach_color_target(_fbo, GL_COLOR_ATTACHMENT1, _gbuffer.tex_aux_render_target());
	attach_depth_target(_fbo, _gbuffer.aux_depth_renderbuffer());
	set_draw_buffer(_fbo, GL_COLOR_ATTACHMENT0);
	set_read_buffer(_fbo, GL_NONE);
	validate(_fbo);
}

void Shadow_map_pass::begin(const Directional_light_params& dir_light) noexcept
{
	glEnable(GL_DEPTH_TEST);
	glColorMask(true, true, false, false);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbo.id());
	glViewport(0, 0, _gbuffer.viewport_size().width, _gbuffer.viewport_size().height);
	glClearBufferfv(GL_COLOR, 0, _clear_value_shadow_occlusion);
	glClearBufferfv(GL_DEPTH, 0, &_clear_value_depth);

	_prog.use(dir_light);
}

void Shadow_map_pass::end() noexcept
{
	glDisable(GL_DEPTH_TEST);
	glColorMask(true, true, true, true);

	filter_shadow_map();

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, Blank::framebuffer_id);
}

void Shadow_map_pass::filter_shadow_map() noexcept
{
	glBindVertexArray(_gbuffer.aux_geometry_vao_id());

	// horz filter pass (render to _gbuffer.tex_aux_render_target())
	set_draw_buffer(_fbo, GL_COLOR_ATTACHMENT1);

	glBindSampler(0, _gbuffer.nearest_sampler().id());
	glBindTextureUnit(0, _gbuffer.tex_shadow_map().id());
	
	_filter_shader_program.use_for_horizontal_pass();
	draw_elements_base_vertex(_gbuffer.aux_geometry_rect_1x1_params());

	// vert filter pass (render to _gbuffer.tex_shadow_map())
	set_draw_buffer(_fbo, GL_COLOR_ATTACHMENT0);
	glBindTextureUnit(0, _gbuffer.tex_aux_render_target().id());

	_filter_shader_program.use_for_vertical_pass();
	draw_elements_base_vertex(_gbuffer.aux_geometry_rect_1x1_params());
}

void Shadow_map_pass::set_uniform_arrays(size_t rnd_offset, size_t rnd_count,
	const std::vector<float>& uniform_array_model_matrix) noexcept
{
	_prog.set_uniform_array_model_matrix(uniform_array_model_matrix.data() + rnd_offset * 16, rnd_count);
}

// ----- Ssao_pass -----

Ssao_pass::Ssao_pass(Gbuffer& gbuffer, const Glsl_program_desc& source_code) :
	_gbuffer(gbuffer),
	_prog(source_code),
	_sample_rays(generate_sphere_normalized_sample_kernel(sample_ray_count + random_normal_count)),
	_filter_shader_program(Filter_type::gaussian, Filter_kernel_radius::radius_05)
{
	attach_color_target(_fbo, GL_COLOR_ATTACHMENT0, _gbuffer.tex_ssao_map());
	attach_color_target(_fbo, GL_COLOR_ATTACHMENT1, _gbuffer.tex_ssao_map_aux());
	set_draw_buffer(_fbo, GL_COLOR_ATTACHMENT0);
	set_read_buffer(_fbo, GL_NONE);
	validate(_fbo);
}

void Ssao_pass::perform() noexcept
{
	// begin
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbo.id());
	glViewport(0, 0, _gbuffer.tex_ssao_map().size().width, _gbuffer.tex_ssao_map().size().height);
	glClearBufferfv(GL_COLOR, 0, &_clear_value_ssao_map.r);

	glBindSampler(0, _gbuffer.nearest_sampler().id());
	glBindTextureUnit(0, _gbuffer.tex_nds().id());

	// perform
	_prog.use(_sample_rays, sample_ray_count, random_normal_count);

	glBindVertexArray(_gbuffer.aux_geometry_vao_id());
	draw_elements_base_vertex(_gbuffer.aux_geometry_rect_1x1_params());

	filter_ssao_map();
	
	// end
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, Blank::framebuffer_id);

	//_fbo.set_read_buffer(GL_COLOR_ATTACHMENT0);
	//glBlitNamedFramebuffer(_fbo.id(), Blank::framebuffer_id,
	//	0, 0, _gbuffer.tex_ssao_map().size().width, _gbuffer.tex_ssao_map().size().height,
	//	0, 0, _gbuffer.viewport_size().width, _gbuffer.viewport_size().height,
	//	GL_COLOR_BUFFER_BIT, GL_LINEAR);
	//_fbo.set_read_buffer(GL_NONE);
}

void Ssao_pass::filter_ssao_map() noexcept
{
	glBindVertexArray(_gbuffer.aux_geometry_vao_id());

	// horz filter pass (render to _gbuffer.tex_ssao_map_aux())
	set_draw_buffer(_fbo, GL_COLOR_ATTACHMENT1);

	glBindSampler(0, _gbuffer.nearest_sampler().id());
	glBindTextureUnit(0, _gbuffer.tex_ssao_map().id());

	_filter_shader_program.use_for_horizontal_pass();
	draw_elements_base_vertex(_gbuffer.aux_geometry_rect_1x1_params());

	// vert filter pass (render to _gbuffer.tex_shadow_map())
	set_draw_buffer(_fbo, GL_COLOR_ATTACHMENT0);
	glBindTextureUnit(0, _gbuffer.tex_ssao_map_aux().id());

	_filter_shader_program.use_for_vertical_pass();
	draw_elements_base_vertex(_gbuffer.aux_geometry_rect_1x1_params());
}

// ----- Tone_mapping_pass -----

Tone_mapping_pass::Tone_mapping_pass(Gbuffer& gbuffer, const cg::data::Glsl_program_desc& source_code) :
	_gbuffer(gbuffer),
	_prog(source_code)
{}

void Tone_mapping_pass::perform() noexcept
{
	// renders directly into the back buffer.
	glViewport(0, 0, _gbuffer.viewport_size().width, _gbuffer.viewport_size().height);
	glClearBufferfv(GL_COLOR, 0, &_clear_value_color.x);

	glBindSampler(0, _gbuffer.nearest_sampler().id());
	glBindTextureUnit(0, _gbuffer.tex_material_lighting_result().id());
	_prog.use();

	glBindVertexArray(_gbuffer.aux_geometry_vao_id());
	draw_elements_base_vertex(_gbuffer.aux_geometry_rect_1x1_params());
}

// ----- Renderer -----

Renderer::Renderer(const Renderer_config& config) :
	_gbuffer(config.viewport_size, config.vertex_attrib_layout, config.rect_1x1_mesh_data),
	_gbuffer_pass(_gbuffer, config.gbuffer_pass_code),
	_lighting_pass(_gbuffer, config.lighting_pass_dir_code),
	_shadow_map_pass(_gbuffer, config.shadow_map_pass_code),
	_ssao_pass(_gbuffer, config.ssao_pass_code),
	_material_lighting_pass(_gbuffer, config.material_lighting_pass_code),
	_tone_mapping_pass(_gbuffer, config.tone_mapping_pass_code)
{}

void Renderer::perform_gbuffer_pass(const Frame& frame) noexcept
{
	_gbuffer_pass.begin(frame.projection_matrix, frame.view_matrix);

	// for each frame packet
	glBindVertexArray(frame.vao_id());

	// for each batch is the frame packet
	for (size_t bi = 0; bi < frame.batch_count(); ++bi) {
		// rnd_offset: processed renderable object count.
		// rnd_count: The number of renderable objects in the current batch #bi.
		size_t rnd_offset = bi * frame.batch_size();
		size_t rnd_count = std::min(frame.batch_size(), frame.renderable_count() - rnd_offset);

		// uniform arrays
		_gbuffer_pass.set_uniform_arrays(rnd_offset, rnd_count, 
			frame.uniform_array_model_matrix(),
			frame.uniform_array_smoothness(),
			frame.uniform_array_tex_normal_map());
		
		
		// draw indirect
		unsigned char* draw_indirect_ptr = nullptr;
		draw_indirect_ptr += rnd_offset * sizeof(DE_indirect_params);
		glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, draw_indirect_ptr, GLsizei(rnd_count), 0);
	}

	_gbuffer_pass.end();
}

void Renderer::perform_lighting_pass(const Frame& frame) noexcept
{
	_lighting_pass.begin();
	_lighting_pass.perform_directional_light_pass(frame.far_plane_coords, frame.directional_light);
	_lighting_pass.end();
}

void Renderer::perform_material_lighting_pass(const Frame& frame) noexcept
{
	_material_lighting_pass.begin(frame.projection_matrix, frame.view_matrix, frame.directional_light);

	// for each frame packet
	glBindVertexArray(frame.vao_id());

	for (size_t bi = 0; bi < frame.batch_count(); ++bi) {
		// rnd_offset: processed renderable object count.
		// rnd_count: The number of renderable objects in the current batch #bi.
		size_t rnd_offset = bi * frame.batch_size();
		size_t rnd_count = std::min(frame.batch_size(), frame.renderable_count() - rnd_offset);

		// uniform arrays
		_material_lighting_pass.set_uniform_arrays(rnd_offset, rnd_count,
			frame.uniform_array_model_matrix(),
			frame.uniform_array_tex_diffuse_rgb(),
			frame.uniform_array_tex_specular_intensity());


		// draw indirect
		unsigned char* draw_indirect_ptr = nullptr;
		draw_indirect_ptr += rnd_offset * sizeof(DE_indirect_params);
		glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, draw_indirect_ptr, GLsizei(rnd_count), 0);
	}

	_material_lighting_pass.end();
}

void Renderer::perform_shadow_map_pass(const Frame& frame) noexcept
{
	_shadow_map_pass.begin(frame.directional_light);

	// for each frame packet
	glBindVertexArray(frame.vao_id());

	// for each batch is the frame packet
	for (size_t bi = 0; bi < frame.batch_count(); ++bi) {
		// rnd_offset: processed renderable object count.
		// rnd_count: The number of renderable objects in the current batch #bi.
		size_t rnd_offset = bi * frame.batch_size();
		size_t rnd_count = std::min(frame.batch_size(), frame.renderable_count() - rnd_offset);

		// uniform arrays
		_shadow_map_pass.set_uniform_arrays(rnd_offset, rnd_count,
			frame.uniform_array_model_matrix());


		// draw indirect
		unsigned char* draw_indirect_ptr = nullptr;
		draw_indirect_ptr += rnd_offset * sizeof(DE_indirect_params);
		glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, draw_indirect_ptr, GLsizei(rnd_count), 0);
	}

	_shadow_map_pass.end();
}

void Renderer::render(const Frame& frame) noexcept
{
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	perform_gbuffer_pass(frame);
	perform_lighting_pass(frame); 
	perform_shadow_map_pass(frame);
	_ssao_pass.perform();
	perform_material_lighting_pass(frame);
	_tone_mapping_pass.perform();
}

void Renderer::resize_viewport(const uint2& size) noexcept
{
	assert(cg::greater_than(size, 0));
	if (_gbuffer.viewport_size() == size) return;

	_gbuffer.resize(size);
}

} // namespace deferred_lighting