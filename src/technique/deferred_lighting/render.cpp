#include "technique/deferred_lighting/render.h"

#include <cassert>
#include <algorithm>
#include <numeric>
#include <memory>

using namespace cg;
using namespace cg::rnd::opengl;
using cg::data::Interleaved_mesh_data;
using cg::data::Shader_program_source_code;


namespace deferred_lighting {

// ----- Gbuffer -----

Gbuffer::Gbuffer(const uint2& viewport_size,
	const cg::rnd::opengl::Vertex_attrib_layout& vertex_attrib_layout,
	const Interleaved_mesh_data& rect_1x1_mesh_data) :
	_vertex_attrib_layout(vertex_attrib_layout),
	_bilinear_sampler(Sampler_config(Min_filter::bilinear, Mag_filter::bilinear, Wrap_mode::clamp_to_edge)),
	_nearest_sampler(Sampler_config(Min_filter::nearest, Mag_filter::nearest, Wrap_mode::clamp_to_edge)),
	_gaussian_filter_shader_program(cg::rnd::utility::Gaussian_filter_kernel_size::radius_5),
	_viewport_size(viewport_size),
	_aux_depth_renderbuffer(Texture_format::depth_32f, viewport_size),
	_tex_aux_render_target(Texture_format::rg_32f, viewport_size),
	_tex_depth_map(Texture_format::depth_32f, viewport_size),
	_tex_normal_smoothness(Texture_format::rgba_32f, viewport_size),
	_tex_shadow_map(Texture_format::rg_32f, viewport_size),
	_tex_lighting_ambient_term(Texture_format::rgb_32f, viewport_size),
	_tex_lighting_diffuse_term(Texture_format::rgb_32f, viewport_size),
	_tex_lighting_specular_term(Texture_format::rgb_32f, viewport_size),
	_tex_material_lighting_result(Texture_format::rgb_32f, viewport_size)
{
	Static_vertex_spec_builder vs_builder(8, 8);
	vs_builder.begin(rect_1x1_mesh_data.attribs(), kilobytes(1));
	{
		auto rect_1x1_cmd = vs_builder.push_back(rect_1x1_mesh_data);
		_aux_geometry_rect_1x1_params = rect_1x1_cmd.get_base_vertex_params();
	}
	_aux_geometry_vertex_spec = vs_builder.end(_vertex_attrib_layout, true);
}

void Gbuffer::resize(const uint2& viewport_size) noexcept
{
	_viewport_size = viewport_size;

	_aux_depth_renderbuffer.set_size(_viewport_size);
	_tex_aux_render_target.set_size(_viewport_size);
	_tex_depth_map.set_size(_viewport_size);
	_tex_normal_smoothness.set_size(_viewport_size);
	_tex_shadow_map.set_size(_viewport_size);
	_tex_lighting_ambient_term.set_size(_viewport_size);
	_tex_lighting_diffuse_term.set_size(_viewport_size);
	_tex_lighting_specular_term.set_size(_viewport_size);
	_tex_material_lighting_result.set_size(_viewport_size);
}

// ----- Gbuffer_pass -----

Gbuffer_pass::Gbuffer_pass(Gbuffer& gbuffer, const cg::data::Shader_program_source_code& source_code) :
	_prog(source_code),
	_gbuffer(gbuffer)
{
	_fbo.attach_color_texture(GL_COLOR_ATTACHMENT0, _gbuffer.tex_normal_smoothness());
	_fbo.attach_depth_texture(_gbuffer.tex_depth_map());
	_fbo.set_draw_buffer(GL_COLOR_ATTACHMENT0);
	_fbo.set_read_buffer(GL_NONE);
	_fbo.validate();
}

void Gbuffer_pass::begin(const cg::mat4& projection_matrix, const cg::mat4& view_matrix) noexcept
{
	glEnable(GL_DEPTH_TEST);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbo.id());
	glViewport(0, 0, _gbuffer.viewport_size().width, _gbuffer.viewport_size().height);
	glClearBufferfv(GL_COLOR, 0, _clear_value_normal_smoothness);
	glClearBufferfv(GL_DEPTH, 0, &_clear_value_depth_map);

	_prog.use(projection_matrix, view_matrix);
}

void Gbuffer_pass::end() noexcept
{
	glDisable(GL_DEPTH_TEST);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, Invalid::framebuffer_id);
}

void Gbuffer_pass::set_uniform_arrays(size_t rnd_offset, size_t rnd_count,
	const std::vector<float>& uniform_array_model_matrix,
	const std::vector<float>& uniform_array_smoothness,
	const std::vector<GLuint>& uniform_array_tex_normal_map) noexcept
{
	_prog.set_uniform_array_model_matrix(uniform_array_model_matrix.data() + rnd_offset * 16, rnd_count);
	_prog.set_uniform_array_smoothness(uniform_array_smoothness.data() + rnd_offset, rnd_count);

	for (GLuint curr_unit = 0; curr_unit < rnd_count; ++curr_unit) {
		glBindSampler(curr_unit, Invalid::sampler_id);
		glBindTextureUnit(curr_unit, uniform_array_tex_normal_map[rnd_offset + curr_unit]);
	}
}

// ----- Lighting_pass -----

Lighting_pass::Lighting_pass(Gbuffer& gbuffer, const cg::data::Shader_program_source_code& dir_source_code) :
	_gbuffer(gbuffer),
	_dir_prog(dir_source_code)
{
	_fbo.attach_color_texture(GL_COLOR_ATTACHMENT0, _gbuffer.tex_lighting_ambient_term());
	_fbo.attach_color_texture(GL_COLOR_ATTACHMENT1, _gbuffer.tex_lighting_diffuse_term());
	_fbo.attach_color_texture(GL_COLOR_ATTACHMENT2, _gbuffer.tex_lighting_specular_term());
	_fbo.set_draw_buffers(GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2);
	_fbo.set_read_buffer(GL_NONE);
	_fbo.validate();
}

void Lighting_pass::begin() noexcept
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbo.id());
	glViewport(0, 0, _gbuffer.viewport_size().width, _gbuffer.viewport_size().height);
	glClearColor(_clear_value_color.r, _clear_value_color.g, _clear_value_color.b, _clear_value_color.a);
	glClear(GL_COLOR_BUFFER_BIT);

	glBindSampler(0, _gbuffer.nearest_sampler().id());
	glBindTextureUnit(0, _gbuffer.tex_normal_smoothness().id());
	glBindSampler(1, _gbuffer.nearest_sampler().id());
	glBindTextureUnit(1, _gbuffer.tex_depth_map().id());
}

void Lighting_pass::end() noexcept
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, Invalid::framebuffer_id);

	//size_t hw = _gbuffer.viewport_size().width / 2;
	//size_t hh = _gbuffer.viewport_size().height / 2;
	//
	//// ambient_term -> upper-left
	//_fbo.set_read_buffer(GL_COLOR_ATTACHMENT0);
	//glBlitNamedFramebuffer(_fbo.id(), Invalid::framebuffer_id,
	//	0, 0, _gbuffer.viewport_size().width, _gbuffer.viewport_size().height,
	//	0, hh, hw - 1, _gbuffer.viewport_size().height,
	//	GL_COLOR_BUFFER_BIT, GL_LINEAR);
	//
	//// diffuse_term -> upper-right
	//_fbo.set_read_buffer(GL_COLOR_ATTACHMENT1);
	//glBlitNamedFramebuffer(_fbo.id(), Invalid::framebuffer_id,
	//	0, 0, _gbuffer.viewport_size().width, _gbuffer.viewport_size().height,
	//	hw, hh, _gbuffer.viewport_size().width, _gbuffer.viewport_size().height,
	//	GL_COLOR_BUFFER_BIT, GL_LINEAR);
	//
	//// specular_term -> upper-right
	//_fbo.set_read_buffer(GL_COLOR_ATTACHMENT2);
	//glBlitNamedFramebuffer(_fbo.id(), Invalid::framebuffer_id,
	//	0, 0, _gbuffer.viewport_size().width, _gbuffer.viewport_size().height,
	//	0, 0, hw -1 , hh - 1,
	//	GL_COLOR_BUFFER_BIT, GL_LINEAR);
	//
	//_fbo.set_read_buffer(GL_NONE);
}

void Lighting_pass::perform_directional_light_pass(const cg::mat4& projection_matrix, 
	const Directional_light_params& dir_light) noexcept
{
	_dir_prog.use(_gbuffer.viewport_size(), projection_matrix, dir_light);
	
	glBindVertexArray(_gbuffer.aux_geometry_vao_id());
	draw_elements_base_vertex(_gbuffer.aux_geometry_rect_1x1_params());
}

// ----- Material_lighting_pass -----

Material_lighting_pass::Material_lighting_pass(Gbuffer& gbuffer, const cg::data::Shader_program_source_code& source_code) :
	_gbuffer(gbuffer),
	_prog(source_code)
{
	_fbo.attach_color_texture(GL_COLOR_ATTACHMENT0, _gbuffer.tex_material_lighting_result());
	_fbo.attach_depth_texture(_gbuffer.tex_depth_map());
	_fbo.set_draw_buffer(GL_COLOR_ATTACHMENT0);
	_fbo.set_read_buffer(GL_NONE);
	_fbo.validate();
}

void Material_lighting_pass::begin(const mat4& projection_view_matrix, const Directional_light_params& dir_light) noexcept
{
	glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glDepthMask(false);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbo.id());
	glViewport(0, 0, _gbuffer.viewport_size().width, _gbuffer.viewport_size().height);
	glClearBufferfv(GL_COLOR, 0, &_clear_value_color.x);

	_prog.use(projection_view_matrix, dir_light);
	// ambient term
	glBindSampler(28, _gbuffer.nearest_sampler().id());
	glBindTextureUnit(28, _gbuffer.tex_lighting_ambient_term().id());
	// diffuse term
	glBindSampler(29, _gbuffer.nearest_sampler().id());
	glBindTextureUnit(29, _gbuffer.tex_lighting_diffuse_term().id());
	// specular term
	glBindSampler(30, _gbuffer.nearest_sampler().id());
	glBindTextureUnit(30, _gbuffer.tex_lighting_specular_term().id());
	// shadow map
	glBindSampler(31, _gbuffer.bilinear_sampler().id());
	glBindTextureUnit(31, _gbuffer.tex_shadow_map().id());
}

void Material_lighting_pass::end() noexcept
{
	glDisable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glDepthMask(true);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, Invalid::framebuffer_id);

	_fbo.set_read_buffer(GL_COLOR_ATTACHMENT0);
	glBlitNamedFramebuffer(_fbo.id(), Invalid::framebuffer_id,
		0, 0, _gbuffer.viewport_size().width, _gbuffer.viewport_size().height,
		0, 0, _gbuffer.viewport_size().width, _gbuffer.viewport_size().height,
		GL_COLOR_BUFFER_BIT, GL_LINEAR);
	_fbo.set_read_buffer(GL_NONE);
}

void Material_lighting_pass::set_uniform_arrays(size_t rnd_offset, size_t rnd_count,
	const std::vector<float>& uniform_array_model_matrix,
	const std::vector<GLuint>& uniform_array_tex_diffuse_rgb,
	const std::vector<GLuint>& uniform_array_tex_specular_intensity) noexcept
{
	_prog.set_uniform_array_model_matrix(uniform_array_model_matrix.data() + rnd_offset * 16, rnd_count);

	for (GLuint curr_unit = 0; curr_unit < rnd_count; ++curr_unit) {
		// 14 - the size of every uniform array of sampler2D (material_pass.pixel.glsl)
		glBindSampler(curr_unit, Invalid::sampler_id);
		glBindSampler(curr_unit + 14, Invalid::sampler_id);
		glBindTextureUnit(curr_unit, uniform_array_tex_diffuse_rgb[rnd_offset + curr_unit]);
		glBindTextureUnit(curr_unit + 14, uniform_array_tex_specular_intensity[rnd_offset + curr_unit]);
	}
}

// ----- Shadow_map_pass -----

Shadow_map_pass::Shadow_map_pass(Gbuffer& gbuffer, const cg::data::Shader_program_source_code& source_code) :
	_gbuffer(gbuffer),
	_prog(source_code)
{
	_fbo.attach_color_texture(GL_COLOR_ATTACHMENT0, _gbuffer.tex_shadow_map());
	_fbo.attach_depth_renderbuffer(_gbuffer.aux_depth_renderbuffer());
	_fbo.set_draw_buffers(GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1);
	_fbo.set_read_buffer(GL_NONE);
	_fbo.validate();
}

void Shadow_map_pass::begin(const Directional_light_params& dir_light) noexcept
{
	glEnable(GL_DEPTH_TEST);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbo.id());
	glViewport(0, 0, _gbuffer.viewport_size().width, _gbuffer.viewport_size().height);
	glClearBufferfv(GL_COLOR, 0, _clear_value_shadow_map);
	glClearBufferfv(GL_DEPTH, 0, &_clear_value_depth);

	_prog.use(dir_light);
}

void Shadow_map_pass::end() noexcept
{
	glDisable(GL_DEPTH_TEST);
	filter_shadow_map();

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, Invalid::framebuffer_id);

	//_fbo.set_read_buffer(GL_COLOR_ATTACHMENT0);
	//glBlitNamedFramebuffer(_fbo.id(), Invalid::framebuffer_id,
	//	0, 0, _gbuffer.viewport_size().width, _gbuffer.viewport_size().height,
	//	0, 0, _gbuffer.viewport_size().width, _gbuffer.viewport_size().height,
	//	GL_COLOR_BUFFER_BIT, GL_LINEAR);
	//_fbo.set_read_buffer(GL_NONE);
}

void Shadow_map_pass::filter_shadow_map() noexcept
{
	glBindVertexArray(_gbuffer.aux_geometry_vao_id());

	// horz filter pass (render to _gbuffer.tex_aux_render_target())
	_fbo.attach_color_texture(GL_COLOR_ATTACHMENT0, _gbuffer.tex_aux_render_target());

	glBindSampler(0, _gbuffer.nearest_sampler().id());
	glBindTextureUnit(0, _gbuffer.tex_shadow_map().id());
	
	_gbuffer.gaussian_filter_shader_program().use_for_horizontal_pass();
	draw_elements_base_vertex(_gbuffer.aux_geometry_rect_1x1_params());

	// vert filter pass (render to _gbuffer.tex_shadow_map())
	_fbo.attach_color_texture(GL_COLOR_ATTACHMENT0, _gbuffer.tex_shadow_map());
	glBindTextureUnit(0, _gbuffer.tex_aux_render_target().id());

	_gbuffer.gaussian_filter_shader_program().use_for_vertical_pass();
	draw_elements_base_vertex(_gbuffer.aux_geometry_rect_1x1_params());
}

void Shadow_map_pass::set_uniform_arrays(size_t rnd_offset, size_t rnd_count,
	const std::vector<float>& uniform_array_model_matrix) noexcept
{
	_prog.set_uniform_array_model_matrix(uniform_array_model_matrix.data() + rnd_offset * 16, rnd_count);
}

// ----- Renderer -----

Renderer::Renderer(const Renderer_config& config) :
	_gbuffer(config.viewport_size, config.vertex_attrib_layout, config.rect_1x1_mesh_data),
	_gbuffer_pass(_gbuffer, config.gbuffer_pass_code),
	_shadow_map_pass(_gbuffer, config.shadow_map_pass_code),
	_lighting_pass(_gbuffer, config.lighting_pass_dir_code),
	_material_lighting_pass(_gbuffer, config.material_pass_code)
{}

void Renderer::perform_gbuffer_pass(const Frame& frame) noexcept
{
	_gbuffer_pass.begin(frame.projection_matrix, frame.view_matrix);

	// for each frame packet
	glBindVertexArray(frame.vao_id());

	// for each batch is the frame packet
	auto r = frame.batch_count();
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
		glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, draw_indirect_ptr, rnd_count, 0);
	}

	_gbuffer_pass.end();
}

void Renderer::perform_shadow_map_pass(const Frame& frame) noexcept
{
	_shadow_map_pass.begin(frame.directional_light);

	// for each frame packet
	glBindVertexArray(frame.vao_id());

	// for each batch is the frame packet
	auto r = frame.batch_count();
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
		glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, draw_indirect_ptr, rnd_count, 0);
	}

	_shadow_map_pass.end();
}

void Renderer::perform_lighting_pass(const Frame& frame) noexcept
{
	_lighting_pass.begin();
	_lighting_pass.perform_directional_light_pass(frame.projection_matrix, frame.directional_light);
	_lighting_pass.end();
}

void Renderer::perform_material_lighting_pass(const Frame& frame) noexcept
{
	_material_lighting_pass.begin(frame.projection_view_matrix(), frame.directional_light);

	// for each frame packet
	glBindVertexArray(frame.vao_id());

	// for each batch is the frame packet
	auto r = frame.batch_count();
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
		glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, draw_indirect_ptr, rnd_count, 0);
	}

	_material_lighting_pass.end();
}

void Renderer::render(const Frame& frame) noexcept
{
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	perform_gbuffer_pass(frame);
	perform_shadow_map_pass(frame);
	perform_lighting_pass(frame);
	perform_material_lighting_pass(frame);
}

void Renderer::resize_viewport(const uint2& size) noexcept
{
	assert(cg::greater_than(size, 0));
	if (_gbuffer.viewport_size() == size) return;

	_gbuffer.resize(size);
}

} // namespace deferred_lighting