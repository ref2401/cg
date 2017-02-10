#ifndef TECHNIQUE_DEFERRED_LIGHTING_FRAME_H_
#define TECHNIQUE_DEFERRED_LIGHTING_FRAME_H_

#include <array>
#include "cg/math/math.h"
#include "cg/rnd/opengl/opengl.h"
#include "technique/deferred_lighting/render_pass_shader.h"
#include "technique/deferred_lighting/vertex_spec.h"


namespace deferred_lighting {

// Partitioned_buffer is a buffer decorator that implements double/triple/whatever_factor buffering technique.
// Buffering technique is supposed to solve CPU-GPU synchronization problem.
// Partitioned_buffer creates an internal object of type Buffer_type 
// which is partitioned into subbufer_count equal chunks. 
// At every time Partitioned_buffer writes only into the specific(active/current) chunk of the internal buffer.
class Buffer_partitioned final {
public:

	// Creates object of Buffer_type which size is byte_count * subbuffer_count bytes.
	Buffer_partitioned(size_t partition_count, size_t partition_byte_count) noexcept
		: _partition_count(partition_count), _partition_byte_count(partition_byte_count),
		_buffer(partition_count * partition_byte_count)
	{}

	Buffer_partitioned(const Buffer_partitioned&) = delete;

	Buffer_partitioned(Buffer_partitioned&& bp) noexcept;

	~Buffer_partitioned() noexcept = default;


	Buffer_partitioned& operator=(const Buffer_partitioned&) = delete;

	Buffer_partitioned& operator=(Buffer_partitioned&& bp) noexcept;


	// The index of partition which is currently used as target for writes.
	// The number lies in range [0, partition_count).
	size_t current_partition_index() const noexcept
	{
		return _partition_offset / _partition_byte_count;
	}

	GLuint id() const noexcept
	{
		return _buffer.id();
	}

	// Makes current the next buffer partition. If the end of the internal buffer has been reached
	// then the first partition (index: 0) will be current.
	void move_next_partition() noexcept
	{
		size_t next_index = (current_partition_index() + 1) % _partition_count;
		_partition_offset = next_index * _partition_byte_count;
	}

	// How many bytes are occupies by each partition of the internal buffer.
	size_t partition_byte_count() const noexcept
	{
		return _partition_byte_count;
	}

	// The number of partitions of the internal buffer.
	size_t partition_count() const noexcept
	{
		return _partition_count;
	}

	// Writes a sequence of objects of type T into the current partiotion of the internal buffer.
	// Params:
	// -	rel_offset: Byte offset from the beginig of the current partition.
	// -	ptr: Pointer to the first object of type T in the sequence.
	// -	count: How many objects are in the sequence pointed by ptr.
	//		If count is 0 the method returns immediately.
	// Returns:	rel_offset + sizeof(T) * count.
	//			Byte offset into the current partition that can be used for subsequent writes.	
	template<typename T>
	size_t write(size_t rel_offset, const T* ptr, size_t count = 1) noexcept;

private:
	size_t _partition_count = 0;
	size_t _partition_byte_count = 0;
	size_t _partition_offset = 0;
	cg::rnd::opengl::Buffer_persistent_map _buffer;
};

template<typename T>
size_t Buffer_partitioned::write(size_t rel_offset, const T* ptr, size_t count) noexcept
{
	static_assert(std::is_trivially_copy_assignable<T>::value, "T must satisfy is_trivially_copy_assignable.");
	if (count == 0) return rel_offset;

	const size_t ret_offset = rel_offset + sizeof(T) * count;
	assert(ret_offset <= _partition_byte_count);

	cg::rnd::opengl::write<T>(_buffer, _partition_offset + rel_offset, ptr, count);
	return ret_offset;
}

// Directional_light respresents extremely distant light sources like sun.
// The light of sucht light source travels in a single direction that is the same throughout the scene.
// All the orientation vectors are considered to be in the world space.
struct Directional_light final {

	Directional_light() noexcept = default;

	Directional_light(const cg::float3& position, const cg::float3& target,
		const cg::float3& rgb, float intensity, float ambient_intensity) noexcept;
	

	// Projection matrix of the light that is used in shadow mapping.
	cg::mat4 projection_matrix;

	// Position of the light. 
	// The value is required in shadow mapping computations.
	cg::float3 position = cg::float3::unit_z;

	// The point at which light's direction points to.
	// The value is required in shadow mapping computations.
	cg::float3 target = cg::float3::zero;

	// The color of the light.
	cg::float3 rgb = cg::float3::unit_xyz;

	// The value represents factor of the light's irradiance.
	float intensity = 1.f;

	// The value represents factor of the light's ambient irradiance.
	float ambient_intensity = 1.f;
};

struct Material_instance final {
	Material_instance() noexcept = default;

	Material_instance(float smoothness,
		GLuint tex_diffuse_rgb_id,
		GLuint tex_normal_map_id,
		GLuint tex_specular_intensity_id) noexcept;

	~Material_instance() noexcept = default;


	float smoothness = 0.f;
	GLuint tex_diffuse_rgb_id;
	GLuint tex_normal_map_id;
	GLuint tex_specular_intensity_id;
};

struct Renderable final {

	Renderable(const DE_cmd& cmd, const cg::mat4& model_matrix, const Material_instance& material) noexcept;

	DE_cmd cmd;
	cg::mat4 model_matrix;
	Material_instance material;
};

// ...
// Implementation notes: Frame temporary contains an vao id and assumes 
// that all the DE_cmd objects added to it refere to the vao id.
class Frame final {
public:

	Frame(size_t max_renderable_count);

	Frame(const Frame& frame) = delete;

	Frame(Frame&& frame) = delete;


	// How many batches are in the frame.
	size_t batch_count() const noexcept
	{
		if (_renderable_count == 0) return 0;

		size_t partial_batch = (_renderable_count % _batch_size > 0) ? 1 : 0;
		return _renderable_count / _batch_size + partial_batch;
	}

	// How many draw indirect commands can be performed in one multi-draw indirect call.
	size_t batch_size() const noexcept
	{
		return _batch_size;
	}

	void begin_rendering() noexcept;

	void end_rendering() noexcept;


	// Returns result of projection_matrix * view_matrix
	cg::mat4 projection_view_matrix() const noexcept
	{
		return projection_matrix * view_matrix;
	}

	void push_back_renderable(const Renderable& rnd);

	void reset(const Static_vertex_spec& vertex_spec) noexcept;


	// frame packet stuff:

	GLuint vao_id() const noexcept
	{
		return _vao_id;
	}

	size_t renderable_count() const noexcept
	{
		return _renderable_count;
	}

	const std::vector<float>& uniform_array_model_matrix() const noexcept
	{
		return _uniform_array_model_matrix;
	}

	const std::vector<float>& uniform_array_smoothness() const noexcept
	{
		return _uniform_array_smoothness;
	}

	const std::vector<GLuint>& uniform_array_tex_diffuse_rgb() const noexcept
	{
		return _uniform_array_tex_diffuse_rgb;
	}

	const std::vector<GLuint>& uniform_array_tex_normal_map() const noexcept
	{
		return _uniform_array_tex_normal_map;
	}

	const std::vector<GLuint>& uniform_array_tex_specular_intensity() const noexcept
	{
		return _uniform_array_tex_specular_intensity;
	}


	Directional_light_params directional_light;

	// Viewpoint(camera) far plane coordinaces.
	// left-bottom, right-bottom, right-top, left-top.
	// All the coordinates are in the view space.
	std::array<cg::float3, 4> far_plane_coords;

	// Viewpoint(camera) projection matrix
	cg::mat4 projection_matrix = cg::mat4::identity;

	// Viewpoint(camera) view matrix.
	cg::mat4 view_matrix = cg::mat4::identity;

private:
	// The method adds additional bindings to the specified vao object hence makeing it sutable for rendering.
	// Side effect: before method returns it resets vertex array binding to zero.
	void prepare_vao(GLuint vao_id, GLuint draw_index_binding_index) noexcept;

	const size_t _max_renderable_count;
	
	// indirect rendering gears
	// batch_size equals to 13 because the 'material_pass.pixel.glsl' shader imposes the most severe restriction.
	const size_t _batch_size = 13;
	std::array<GLsync, 3> _sync_objects;
	Buffer_partitioned _draw_indirect_buffer;
	cg::rnd::opengl::Buffer_immut _draw_index_buffer;  // simulates gl_DrawID
	size_t _offset_draw_indirect = 0;

	// future Frame_packet stuff:
	GLuint _vao_id = cg::rnd::opengl::Blank::vao_id;
	size_t _renderable_count;
	std::vector<float> _uniform_array_model_matrix;
	std::vector<float> _uniform_array_smoothness;
	std::vector<GLuint> _uniform_array_tex_diffuse_rgb;
	std::vector<GLuint> _uniform_array_tex_normal_map;
	std::vector<GLuint> _uniform_array_tex_specular_intensity;
};

} // namespace deferred_lighting

#endif //TECHNIQUE_DEFERRED_LIGHTING_FRAME_H_