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
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, Invalid::framebuffer_id);
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
	_indirect_buffer(3, _gbuffer.max_tex_unit_count() * sizeof(DE_indirect_params)),
	_draw_index_buffer(make_draw_index_buffer(_gbuffer.max_tex_unit_count()))
{
	size_t max_draw_call_count = _gbuffer_pass.batch_size();
	_model_matrix_uniform_data.reserve(max_draw_call_count * 16);
}

void Renderer::render(const Frame& frame) noexcept
{
	GLsync sync_obj = _sync_objects[_indirect_buffer.current_partition_index()];
	wait_for(sync_obj);
	glDeleteSync(sync_obj);
	_sync_objects[_indirect_buffer.current_partition_index()] = nullptr;

	// Gbuffer_pass
	_gbuffer_pass.begin(frame.projection_matrix(), frame.view_matrix());

	// batch_size, different vao id

	//GLuint curr_vao_id = Invalid::vao_id;
	//_model_matrix_uniform_data.clear();

	//for (const auto& rnd : frame.rnd_objects()) {
		//glBindVertexArray(rnd.vao_id);
		//_model_matrix_uniform_data.push_back(rnd.model_matrix);
	//}

	_gbuffer_pass.end();
	glBindTextureUnit(0, _gbuffer.tex_normal_smoothness().id());
	glBindTextureUnit(1, _gbuffer.tex_depth_map().id());
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