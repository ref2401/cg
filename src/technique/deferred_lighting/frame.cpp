#include "technique/deferred_lighting/frame.h"

#include <cassert>
#include <algorithm>
#include <numeric>

using namespace cg;
using namespace cg::opengl;


namespace {

size_t get_batch_size() noexcept
{
	// max_texture_unit_count
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

// ----- Directional_light -----

Directional_light::Directional_light(const float3& position, const float3& target,
	const float3& rgb, float intensity, float ambient_intensity) noexcept :
	position(position),
	target(target),
	rgb(rgb),
	intensity(intensity),
	ambient_intensity(ambient_intensity)
{}

// ----- Renderable -----

Renderable::Renderable(const DE_cmd& cmd, const mat4& model_matrix, const Material& material) noexcept :
	cmd(cmd),
	model_matrix(model_matrix),
	smoothness(material.smoothness),
	tex_diffuse_rgb_id(material.tex_diffuse_rgb.id()),
	tex_normal_map_id(material.tex_normal_map.id()),
	tex_specular_rgb_id(material.tex_specular_rgb.id())
{
	assert(tex_diffuse_rgb_id != Invalid::texture_id);
	assert(tex_normal_map_id != Invalid::texture_id);
	assert(tex_specular_rgb_id != Invalid::texture_id);
}

// ----- Material -----

Material::Material(float smoothness,
	cg::opengl::Texture_2d_immut tex_diffuse_rgb,
	cg::opengl::Texture_2d_immut tex_normal_map,
	cg::opengl::Texture_2d_immut tex_specular_rgb) noexcept :
	smoothness(smoothness),
	tex_diffuse_rgb(std::move(tex_diffuse_rgb)),
	tex_normal_map(std::move(tex_normal_map)),
	tex_specular_rgb(std::move(tex_specular_rgb))
{}

// ----- Frame -----

Frame::Frame(size_t max_renderable_count) :
	_max_renderable_count(max_renderable_count),
	_batch_size(get_batch_size()),
	_draw_indirect_buffer(3, max_renderable_count * sizeof(DE_indirect_params)),
	_draw_index_buffer(make_draw_index_buffer(_batch_size))
{
	const size_t initial_capacity = 16;
	_uniform_array_model_matrix.reserve(initial_capacity * 16);
	_uniform_array_smoothness.reserve(initial_capacity);
	_uniform_array_tex_diffuse_rgb.reserve(initial_capacity);
	_uniform_array_tex_normal_map.reserve(initial_capacity);
	_uniform_array_tex_specular_rgb.reserve(initial_capacity);
}

void Frame::begin_rendering() noexcept
{
	assert(_renderable_count > 0);
	assert(_uniform_array_model_matrix.size() / 16 == _renderable_count);
	assert(_uniform_array_smoothness.size() == _renderable_count);
	assert(_uniform_array_tex_diffuse_rgb.size() == _renderable_count);
	assert(_uniform_array_tex_normal_map.size() == _renderable_count);
	assert(_uniform_array_tex_specular_rgb.size() == _renderable_count);
}

void Frame::end_rendering() noexcept
{
	_sync_objects[_draw_indirect_buffer.current_partition_index()] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
	_draw_indirect_buffer.move_next_partition();
}

void Frame::push_back_renderable(const Renderable& rnd)
{
	assert(rnd.cmd.vao_id() == _vao_id);
	assert(_renderable_count < _max_renderable_count);

	// indirect params -> _draw_indirect_buffer
	auto params = rnd.cmd.get_indirect_params();
	params.base_instance = _renderable_count % _batch_size; // base index is required to calculate an index to draw_index_buffer
	_offset_draw_indirect = _draw_indirect_buffer.write(_offset_draw_indirect, &params);

	// model matrix -> _uniform_arr_model_matrix
	float model_matrix_arr[16];
	put_in_column_major_order(rnd.model_matrix, model_matrix_arr);
	_uniform_array_model_matrix.insert(_uniform_array_model_matrix.end(),
		std::begin(model_matrix_arr), std::end(model_matrix_arr));

	// material
	_uniform_array_smoothness.push_back(rnd.smoothness); // smoothness -> _uniform_array_smoothness
	_uniform_array_tex_diffuse_rgb.push_back(rnd.tex_diffuse_rgb_id); // tex_diffuse_rgb_id -> _uniform_array_tex_diffuse_rgb
	_uniform_array_tex_normal_map.push_back(rnd.tex_normal_map_id); // tex_normal_map_id -> _uniform_array_tex_normal_map
	_uniform_array_tex_specular_rgb.push_back(rnd.tex_specular_rgb_id); // tex_specular_rgb_id -> _uniform_array_tex_specular_rgb

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
	_uniform_array_model_matrix.clear();
	_uniform_array_smoothness.clear();
	_uniform_array_tex_diffuse_rgb.clear();
	_uniform_array_tex_normal_map.clear();
	_uniform_array_tex_specular_rgb.clear();
}

} // namespace deferred_lighting