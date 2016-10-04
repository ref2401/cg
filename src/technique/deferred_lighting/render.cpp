#include "technique/deferred_lighting/render.h"

#include <cassert>
#include <algorithm>
#include <numeric>
#include <memory>

using namespace cg;
using namespace cg::opengl;
using cg::data::Interleaved_mesh_data;
using cg::data::Shader_program_source_code;


namespace deferred_lighting {

// ----- Gbuffer -----

Gbuffer::Gbuffer(uint2 viewport_size,
	const cg::opengl::Vertex_attrib_layout& vertex_attrib_layout,
	const Interleaved_mesh_data& rect_1x1_mesh_data) noexcept :
	_vertex_attrib_layout(vertex_attrib_layout),
	_bilinear_sampler(Sampler_config(Min_filter::bilinear, Mag_filter::bilinear, Wrap_mode::clamp_to_edge)),
	_nearest_sampler(Sampler_config(Min_filter::nearest, Mag_filter::nearest, Wrap_mode::clamp_to_edge))
{
	Static_vertex_spec_builder vs_builder(8, 8);
	vs_builder.begin(rect_1x1_mesh_data.attribs(), kilobytes(1));
	{
		auto rect_1x1_cmd = vs_builder.push_back(rect_1x1_mesh_data);
		_aux_geometry_rect_1x1_params = rect_1x1_cmd.get_base_vertex_params();
	}
	vs_builder.end(_vertex_attrib_layout, true);
	resize(viewport_size);
}

void Gbuffer::resize(uint2 viewport_size) noexcept
{
	_viewport_size = viewport_size;
	_tex_depth_map = Texture_2d(Texture_format::depth_32, viewport_size);
	_tex_normal_smoothness = Texture_2d(Texture_format::rgba_32f, viewport_size);
	_tex_lighting_ambient_term = Texture_2d(Texture_format::rgb_32f, viewport_size);
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
	
	glBindFramebuffer(GL_READ_FRAMEBUFFER, _fbo.id());
	_fbo.set_read_buffer(GL_COLOR_ATTACHMENT0);

	glBlitFramebuffer(
		0, 0, _gbuffer.viewport_size().width, _gbuffer.viewport_size().height,
		0, 0, _gbuffer.viewport_size().width, _gbuffer.viewport_size().height,
		GL_COLOR_BUFFER_BIT, GL_NEAREST);
	_fbo.set_read_buffer(GL_NONE);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, Invalid::framebuffer_id);
}

void Gbuffer_pass::set_uniform_arrays(size_t rnd_offset, size_t rnd_count,
	const std::vector<float>& uniform_array_model_matrix,
	const std::vector<float>& uniform_array_smoothness,
	const std::vector<GLuint>& uniform_array_tex_normal_map) noexcept
{
	_prog.set_uniform_array_model_matrix(uniform_array_model_matrix.data() + rnd_offset * 16, rnd_count);
	_prog.set_uniform_array_smoothness(uniform_array_smoothness.data() + rnd_offset, rnd_count);

	size_t curr_unit = 0;
	for (; curr_unit < rnd_count; ++curr_unit)
		glBindTextureUnit(curr_unit, uniform_array_tex_normal_map[rnd_offset + curr_unit]);
}

// ----- Lighting_pass -----

Lighting_pass::Lighting_pass(Gbuffer& gbuffer, const cg::data::Shader_program_source_code& dir_source_code) :
	_gbuffer(gbuffer),
	_dir_prog(dir_source_code)
{}

// ----- Renderer -----

Renderer::Renderer(Renderer_config& config) :
	_gbuffer(config.viewport_size, config.vertex_attrib_layout, config.rect_1x1_mesh_data),
	_gbuffer_pass(_gbuffer, config.gbuffer_pass_code),
	_lighting_pass(_gbuffer, config.lighting_pass_dir_code)
{}

void Renderer::perform_gbuffer_pass(const Frame& frame) noexcept
{
	_gbuffer_pass.begin(frame.projection_matrix(), frame.view_matrix());

	// for each frame packet
	glBindVertexArray(frame.vao_id());

	// for each batch is the frame packet
	auto r = frame.batch_count();
	for (GLuint bi = 0; bi < frame.batch_count(); ++bi) {
		glBindSampler(bi, _gbuffer.bilinear_sampler().id());

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
	glBindTextureUnit(0, _gbuffer.tex_normal_smoothness().id());
	glBindTextureUnit(1, _gbuffer.tex_depth_map().id());
}

void Renderer::perform_lighting_pass(const Frame& frame) noexcept
{

}

void Renderer::render(const Frame& frame) noexcept
{
	perform_gbuffer_pass(frame);
	
	// bind vao, 
	// for each Rnd_obj until vao_id has not changed do:
	// current_pass.batch_size() - how many object may be in one indirect call for the current pass.
	// populate indirect buffer

	
}

void Renderer::resize_viewport(uint2 size) noexcept
{
	assert(cg::greater_than(size, 0));

	glViewport(0, 0, size.width, size.height);
	_gbuffer.resize(size);
}

} // namespace deferred_lighting