#include "technique/deferred_lighting/frame.h"

#include <cassert>
#include <algorithm>
#include <numeric>

using cg::float3;
using cg::mat3;
using cg::mat4;
using cg::put_in_column_major_order;
using cg::opengl::DE_indirect_params;
using cg::opengl::DE_cmd;
using cg::opengl::Invalid;
using cg::opengl::Static_buffer;
using cg::opengl::Static_vertex_spec;
using cg::opengl::wait_for;


namespace {

size_t get_batch_size() noexcept
{
	// const size_t _batch_size = 62; // see gbuffer_pass.vertex.glsl
	return 2;
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

Frame::Frame(size_t max_renderable_count) :
	_max_renderable_count(max_renderable_count),
	_batch_size(get_batch_size()),
	_draw_indirect_buffer(3, max_renderable_count * sizeof(DE_indirect_params)),
	_draw_index_buffer(make_draw_index_buffer(_batch_size))
{
	_uniform_arr_model_matrix.reserve(16 * 16);
}

void Frame::begin_rendering() noexcept
{
	assert(_renderable_count > 0);
	// _offset_indirect / sizeof(DE_indirect_params) == _offset_indirect
	// _uniform_arr_model_matrix.size() / 16 == _rnd_obj_count
	// _uniform_arr_smoothness.size() = _rnd_obj_count;
}

void Frame::end_rendering() noexcept
{
	_sync_objects[_draw_indirect_buffer.current_partition_index()] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
	_draw_indirect_buffer.move_next_partition();
}

void Frame::push_back_renderable(const DE_cmd& cmd, const mat4& model_matrix)
{
	assert(cmd.vao_id() == _vao_id);
	assert(_renderable_count < _max_renderable_count);

	// put indirect params into _draw_indirect_buffer
	auto params = cmd.get_indirect_params();
	params.base_instance = _renderable_count % _batch_size; // base index is required to calculate an index to draw_index_buffer
	_offset_draw_indirect = _draw_indirect_buffer.write(_offset_draw_indirect, &params);

	// put model matrix into _uniform_arr_model_matrix
	float model_matrix_arr[16];
	put_in_column_major_order(model_matrix, model_matrix_arr);
	_uniform_arr_model_matrix.insert(_uniform_arr_model_matrix.end(),
		std::begin(model_matrix_arr), std::end(model_matrix_arr));

	++_renderable_count;
}

void Frame::prepare_vao(GLuint vao_id, GLuint draw_index_binding_index) noexcept
{
	assert(vao_id != Invalid::vao_id);

	glBindVertexArray(vao_id);

	// bind indirect buffer & draw index buffer to vao
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, _draw_indirect_buffer.id());

	// draw_index attrib location == buffer binding index.
	static constexpr GLuint draw_index_attrib_location = 15;

	glVertexArrayVertexBuffer(vao_id, draw_index_binding_index, _draw_index_buffer.id(), 0, sizeof(GLuint));
	glVertexArrayAttribBinding(vao_id, draw_index_attrib_location, draw_index_binding_index);
	glVertexArrayAttribIFormat(vao_id, draw_index_attrib_location, 1, GL_UNSIGNED_INT, 0);
	glVertexArrayBindingDivisor(vao_id, draw_index_binding_index, 1);
	glEnableVertexArrayAttrib(vao_id, draw_index_attrib_location);

	glBindVertexArray(Invalid::vao_id);
}

void Frame::reset(const Static_vertex_spec& vertex_spec) noexcept
{
	// sync with gpu
	GLsync sync_obj = _sync_objects[_draw_indirect_buffer.current_partition_index()];
	wait_for(sync_obj);
	glDeleteSync(sync_obj);
	_sync_objects[_draw_indirect_buffer.current_partition_index()] = nullptr;

	_projection_matrix = mat4::identity;
	_view_matrix = mat4::identity;

	// prepare frame_packet
	prepare_vao(vertex_spec.vao_id(), vertex_spec.vertex_buffer_binding_index() + 1);
	_vao_id = vertex_spec.vao_id();
	_renderable_count = 0;
	_offset_draw_indirect = 0;
	_uniform_arr_model_matrix.clear();
}

} // namespace deferred_lighting