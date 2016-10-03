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

Static_buffer make_draw_index_buffer(size_t draw_call_count)
{
	std::vector<GLuint> draw_indices(draw_call_count);
	std::iota(draw_indices.begin(), draw_indices.end(), 0);
	return Static_buffer(draw_indices.data(), draw_call_count);
}

} // namespace

namespace deferred_lighting {

// ----- Gbuffer -----

Gbuffer::Gbuffer(uint2 viewport_size) noexcept :
	_max_tex_unit_count(get_max_texture_unit_count())
{
	resize(viewport_size);
}

void Gbuffer::resize(uint2 viewport_size) noexcept
{
	_pixel_size = viewport_size;
	_tex_depth_map = Texture_2d(Texture_format::depth_32, viewport_size);
	_tex_normal_smoothness = Texture_2d(Texture_format::rgba_32f, viewport_size);
}

// ----- Gbuffer_pass -----

Gbuffer_pass::Gbuffer_pass(Gbuffer& gbuffer) noexcept :
	_gbuffer(gbuffer)
{
	_fbo.attach_color_texture(GL_COLOR_ATTACHMENT0, _gbuffer.tex_normal_smoothness());
	_fbo.attach_depth_texture(_gbuffer.tex_depth_map());
	_fbo.set_draw_buffer(GL_COLOR_ATTACHMENT0);
	_fbo.set_read_buffer(GL_NONE);
	_fbo.validate();
}

// ----- Renderer -----

Renderer::Renderer(uint2 viewport_size, const Shader_program_source_code& src) :
	_vertex_attrib_layout(0, 1, 2, 3),
	_gbuffer(viewport_size),
	_gbuffer_pass(_gbuffer),
	_indirect_buffer(3, _gbuffer.max_tex_unit_count() * sizeof(DE_indirect_params)),
	_draw_index_buffer(make_draw_index_buffer(_gbuffer.max_tex_unit_count()))
{
}

void Renderer::render(const Frame& frame) noexcept
{
	GLsync sync_obj = _sync_objects[_indirect_buffer.current_partition_index()];
	wait_for(sync_obj);
	glDeleteSync(sync_obj);
	_sync_objects[_indirect_buffer.current_partition_index()] = nullptr;

	static constexpr float clear_color[4] = { 0.5f, 0.5f, 0.55f, 1.f };
	glClearBufferfv(GL_COLOR, 0, clear_color);

	// bind vao, 
	// for each Rnd_obj until vao_id has not changed do:
	// current_pass.batch_size() - how many object may be in one indirect call for the current pass.
	// populate indirect buffer

	_sync_objects[_indirect_buffer.current_partition_index()] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
	_indirect_buffer.move_next_partition();
}

void Renderer::register_vao(GLuint vao_id, GLuint draw_index_binding_index) noexcept
{
	assert(vao_id != Invalid::vao_id);

	glBindVertexArray(vao_id);
	
	// bind indirect buffer & draw index buffer to vao
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, _indirect_buffer.id());
	
	// draw_index attrib location == buffer binding index.
	static constexpr GLuint draw_index_attrib_location = 15;

	glVertexArrayVertexBuffer(vao_id, draw_index_binding_index, _draw_index_buffer.id(), 0, sizeof(GLuint));
	glVertexArrayAttribBinding(vao_id, draw_index_attrib_location, draw_index_binding_index);
	glVertexArrayAttribIFormat(vao_id, draw_index_attrib_location, 1, GL_UNSIGNED_INT, 0);
	glVertexArrayBindingDivisor(vao_id, draw_index_binding_index, 1);
	glEnableVertexArrayAttrib(vao_id, draw_index_attrib_location);

	glBindVertexArray(Invalid::vao_id);
}

void Renderer::resize_viewport(uint2 size) noexcept
{
	assert(cg::greater_than(size, 0));

	glViewport(0, 0, size.width, size.height);
	_gbuffer.resize(size);
}

} // namespace deferred_lighting