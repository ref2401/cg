#include "technique/deferred_lighting/render.h"

#include <cassert>
#include <algorithm>
#include <numeric>
#include <memory>

using cg::float3;
using cg::mat3;
using cg::mat4;
using cg::uint2;
using cg::put_in_column_major_order;
using cg::data::Shader_program_source_code;
using cg::opengl::DE_indirect_params;
using cg::opengl::DE_cmd;
using cg::opengl::Invalid;
using cg::opengl::Persistent_buffer;
using cg::opengl::Sampler;
using cg::opengl::Sampler_config;
using cg::opengl::Shader_program;
using cg::opengl::Static_buffer;
using cg::opengl::Texture_2d;
using cg::opengl::Texture_format;
using cg::opengl::Vertex_attrib_layout;
using cg::opengl::set_uniform;
using cg::opengl::set_uniform_array;
using cg::opengl::wait_for;


namespace {

size_t get_max_texture_unit_count() noexcept
{
	GLint value = -1;
	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &value);
	
	assert(value > -1);
	return value;
}

} // namespace

namespace deferred_lighting {

// ----- Gbuffer -----

Gbuffer::Gbuffer(uint2 viewport_size) noexcept :
	_texture_units(get_max_texture_unit_count())
{
	std::iota(_texture_units.begin(), _texture_units.end(), 0);
	resize(viewport_size);
}

void Gbuffer::resize(uint2 viewport_size) noexcept
{
	_viewport_size = viewport_size;
	_tex_depth_map = Texture_2d(Texture_format::depth_32, viewport_size);
	_tex_normal_smoothness = Texture_2d(Texture_format::rgba_32f, viewport_size);
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
	const std::vector<GLuint>& uniform_array_tex_normal_map) noexcept
{
	_prog.set_uniform_array_model_matrix(uniform_array_model_matrix.data() + rnd_offset * 16, rnd_count);

	size_t curr_unit = 0;
	for (; curr_unit < rnd_count; ++curr_unit)
		glBindTextureUnit(curr_unit, uniform_array_tex_normal_map[rnd_offset + curr_unit]);
}

// ----- Renderer_config -----

Renderer_config::Renderer_config(cg::uint2 viewport_size, 
	const cg::data::Shader_program_source_code& gbuffer_pass_code) noexcept :
	viewport_size(viewport_size),
	gbuffer_pass_code(gbuffer_pass_code)
{
	assert(greater_than(viewport_size, 0));
	assert(gbuffer_pass_code.vertex_source.size() > 0);
	assert(gbuffer_pass_code.pixel_source.size() > 0);
}

// ----- Renderer -----

Renderer::Renderer(const Renderer_config& config) :
	_vertex_attrib_layout(0, 1, 2, 3),
	_gbuffer(config.viewport_size),
	_gbuffer_pass(_gbuffer, config.gbuffer_pass_code),
	_default_sampler(Sampler_config())
{
}

void Renderer::perform_gbuffer_pass(const Frame& frame) noexcept
{
	_gbuffer_pass.begin(frame.projection_matrix(), frame.view_matrix());

	// for each frame packet
	glBindVertexArray(frame.vao_id());

	// for each batch is the frame packet
	auto r = frame.batch_count();
	for (GLuint bi = 0; bi < frame.batch_count(); ++bi) {
		glBindSampler(bi, _default_sampler.id());

		// rnd_offset: processed renderable object count.
		// rnd_count: The number of renderable objects in the current batch #bi.
		size_t rnd_offset = bi * frame.batch_size();
		size_t rnd_count = std::min(frame.batch_size(), frame.renderable_count() - rnd_offset);

		// uniform arrays
		_gbuffer_pass.set_uniform_arrays(rnd_offset, rnd_count, 
			frame.uniform_array_model_matrix(),
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