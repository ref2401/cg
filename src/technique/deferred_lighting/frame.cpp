#include "technique/deferred_lighting/frame.h"

#include <cassert>
#include <algorithm>
#include <numeric>

using namespace cg;
using namespace cg::rnd::opengl;


namespace {

Buffer_gpu make_draw_index_buffer(size_t draw_call_count)
{
	std::vector<GLuint> draw_indices(draw_call_count);
	std::iota(draw_indices.begin(), draw_indices.end(), 0);
	return Buffer_gpu(draw_indices);
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

Renderable::Renderable(const DE_cmd& cmd, const mat4& model_matrix, const Material_instance& material) noexcept :
	cmd(cmd),
	model_matrix(model_matrix),
	material(material)
{}

// ----- Material_instance -----

Material_instance::Material_instance(float smoothness,
	GLuint tex_diffuse_rgb_id,
	GLuint tex_normal_map_id,
	GLuint tex_specular_intensity_id) noexcept :
	smoothness(smoothness),
	tex_diffuse_rgb_id(tex_diffuse_rgb_id),
	tex_normal_map_id(tex_normal_map_id),
	tex_specular_intensity_id(tex_specular_intensity_id)
{
	assert(this->smoothness >= 0.0f);
	assert(this->tex_diffuse_rgb_id != Blank::texture_id);
	assert(this->tex_normal_map_id != Blank::texture_id);
	assert(this->tex_specular_intensity_id != Blank::texture_id);
}

// ----- Frame -----

Frame::Frame(size_t max_renderable_count) :
	_max_renderable_count(max_renderable_count),
	_draw_indirect_buffer(3, max_renderable_count * sizeof(DE_indirect_params)),
	_draw_index_buffer(make_draw_index_buffer(_batch_size))
{
	const size_t initial_capacity = 16;
	_uniform_array_model_matrix.reserve(initial_capacity * 16);
	_uniform_array_smoothness.reserve(initial_capacity);
	_uniform_array_tex_diffuse_rgb.reserve(initial_capacity);
	_uniform_array_tex_normal_map.reserve(initial_capacity);
	_uniform_array_tex_specular_intensity.reserve(initial_capacity);
}

void Frame::begin_rendering() noexcept
{
	assert(_renderable_count > 0);
	assert(_uniform_array_model_matrix.size() / 16 == _renderable_count);
	assert(_uniform_array_smoothness.size() == _renderable_count);
	assert(_uniform_array_tex_diffuse_rgb.size() == _renderable_count);
	assert(_uniform_array_tex_normal_map.size() == _renderable_count);
	assert(_uniform_array_tex_specular_intensity.size() == _renderable_count);
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
	to_array_column_major_order(rnd.model_matrix, model_matrix_arr);
	_uniform_array_model_matrix.insert(_uniform_array_model_matrix.end(),
		std::begin(model_matrix_arr), std::end(model_matrix_arr));

	// material
	// smoothness			->	_uniform_array_smoothness
	// tex_diffuse_rgb_id	->	_uniform_array_tex_diffuse_rgb
	// tex_normal_map_id	->	_uniform_array_tex_normal_map
	// tex_specular_rgb_id	->	_uniform_array_tex_specular_rgb
	_uniform_array_smoothness.push_back(rnd.material.smoothness); 
	_uniform_array_tex_diffuse_rgb.push_back(rnd.material.tex_diffuse_rgb_id);
	_uniform_array_tex_normal_map.push_back(rnd.material.tex_normal_map_id);
	_uniform_array_tex_specular_intensity.push_back(rnd.material.tex_specular_intensity_id);

	++_renderable_count;
}

void Frame::prepare_vao(GLuint vao_id, GLuint draw_index_binding_index) noexcept
{
	assert(vao_id != Blank::vao_id);

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

	glBindVertexArray(Blank::vao_id);
}

void Frame::reset(const Static_vertex_spec& vertex_spec) noexcept
{
	// sync with gpu
	GLsync sync_obj = _sync_objects[_draw_indirect_buffer.current_partition_index()];
	wait_for(sync_obj);
	glDeleteSync(sync_obj);
	_sync_objects[_draw_indirect_buffer.current_partition_index()] = nullptr;

	projection_matrix = mat4::identity;
	view_matrix = mat4::identity;

	// prepare frame_packet
	prepare_vao(vertex_spec.vao_id(), vertex_spec.vertex_buffer_binding_index() + 1);
	_vao_id = vertex_spec.vao_id();
	_renderable_count = 0;
	_offset_draw_indirect = 0;
	_uniform_array_model_matrix.clear();
	_uniform_array_smoothness.clear();
	_uniform_array_tex_diffuse_rgb.clear();
	_uniform_array_tex_normal_map.clear();
	_uniform_array_tex_specular_intensity.clear();
}

} // namespace deferred_lighting