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

// ----- Frame -----

Frame::Frame(GLuint vao_id)
:	_vao_id(vao_id)
{
	_renderable_objects.reserve(16);
}

Frame::Frame(Frame&& frame) noexcept
	: _renderable_objects(std::move(frame._renderable_objects))
{}

Frame& Frame::operator=(Frame&& frame) noexcept
{
	_renderable_objects = std::move(frame._renderable_objects);
	return *this;
}

void Frame::clear() noexcept
{
	_renderable_objects.clear();
	_projection_matrix = mat4::identity;
	_view_matrix = mat4::identity;
}

void Frame::push_back_renderable(const DE_cmd& cmd, const mat4& model_matrix, GLuint tex_diffuse_rgb_id)
{
	assert(cmd.vao_id() == _vao_id);
	assert(tex_diffuse_rgb_id != Invalid::texture_id);

	_renderable_objects.emplace_back(cmd, model_matrix, 
		static_cast<mat3>(model_matrix), tex_diffuse_rgb_id);
}

// ----- Renderer -----

Renderer::Renderer(uint2 viewport_size, const Shader_program_source_code& src) : 
	max_texture_unit_count(get_max_texture_unit_count()),
	_vertex_attrib_layout(0, 1, 2, 3),
	_indirect_buffer(3, max_texture_unit_count * sizeof(DE_indirect_params)),
	_draw_index_buffer(make_draw_index_buffer(max_texture_unit_count)),
	_texture_units(max_texture_unit_count),
	_prog("test-shader", src),
	_u_pv_matrix(_prog.get_uniform_location("u_projection_view_matrix")),
	_u_model_matrix_array(_prog.get_uniform_location("u_model_matrix_array")),
	_u_normal_matrix_array(_prog.get_uniform_location("u_normal_matrix_array")),
	_u_tex_diffuse_rgb_array(_prog.get_uniform_location("u_tex_diffuse_rgb_array")),
	_sampler(Sampler_config())
{
	std::iota(_texture_units.begin(), _texture_units.end(), 0);
	resize_viewport(viewport_size);
}

void Renderer::render(const Frame& frame) noexcept
{
	assert(frame.renderable_objects().size() <= 512); // use uniform blocks for renderable list that is greater than 512.

	GLsync sync_obj = _sync_objects[_indirect_buffer.current_partition_index()];
	wait_for(sync_obj);
	glDeleteSync(sync_obj);
	_sync_objects[_indirect_buffer.current_partition_index()] = nullptr;

	static constexpr float clear_color[4] = { 0.5f, 0.5f, 0.55f, 1.f };
	static constexpr float clear_depth = 1.f;
	glClearBufferfv(GL_COLOR, 0, clear_color);
	glEnable(GL_DEPTH_TEST);
	glClearBufferfv(GL_DEPTH, 0, &clear_depth);

	// material
	glUseProgram(_prog.id());
	set_uniform(_u_pv_matrix, frame.projection_matrix() * frame.view_matrix());

	// vertex format
	glBindVertexArray(frame.vao_id());

	// renderable
	const size_t draw_call_count = frame.renderable_objects().size();
	assert(draw_call_count <= max_texture_unit_count);
	assert(draw_call_count <= Renderer::max_draw_call_count);
	size_t offset_indirect = 0;
	size_t offset_draw_index = 0;
	_model_matrices.clear();
	_normal_matrices.clear();
	_model_matrices.reserve(draw_call_count);
	_normal_matrices.reserve(draw_call_count);

	for (size_t i = 0; i < draw_call_count; ++i) {
		const auto& rnd_object = frame.renderable_objects()[i];

		// put model & normal matrices into uniform array buffer.
		_model_matrices.push_back(rnd_object.model_matrix);
		_normal_matrices.push_back(rnd_object.normal_matrix);

		// put diffuse textures into uniform array buffer.
		glBindSampler(i, _sampler.id());
		glBindTextureUnit(i, rnd_object.tex_diffuse_rgb_id);

		// put cmd into the indirect buffer
		auto params = rnd_object.cmd.get_indirect_params();
		params.base_instance = i; // base index is required to calculate an index to draw_index_buffer
		offset_indirect = _indirect_buffer.write(offset_indirect, &params);
	}

	set_uniform_array(_u_model_matrix_array, _model_matrices.data(), _model_matrices.size());
	set_uniform_array(_u_normal_matrix_array, _normal_matrices.data(), _normal_matrices.size());
	set_uniform_array(_u_tex_diffuse_rgb_array, _texture_units.data(), _texture_units.size());
	glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, 0, draw_call_count, 0);

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
	glViewport(0, 0, size.width, size.height);
}

} // namespace deferred_lighting