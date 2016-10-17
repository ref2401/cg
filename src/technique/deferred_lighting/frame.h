#ifndef TECHNIQUE_DEFERRED_LIGHTING_FRAME_H_
#define TECHNIQUE_DEFERRED_LIGHTING_FRAME_H_

#include <array>
#include "cg/math/math.h"
#include "cg/rnd/opengl/opengl.h"
#include "technique/deferred_lighting/render_pass_shader.h"


namespace deferred_lighting {

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

	Renderable(const cg::rnd::opengl::DE_cmd& cmd, const cg::mat4& model_matrix, const Material_instance& material) noexcept;

	cg::rnd::opengl::DE_cmd cmd;
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

	void reset(const cg::rnd::opengl::Static_vertex_spec& vertex_spec) noexcept;	


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
	cg::rnd::opengl::Partitioned_buffer<cg::rnd::opengl::Persistent_buffer> _draw_indirect_buffer;
	cg::rnd::opengl::Static_buffer _draw_index_buffer;  // simulates gl_DrawID
	size_t _offset_draw_indirect = 0;

	// future Frame_packet stuff:
	GLuint _vao_id = cg::rnd::opengl::Invalid::vao_id;
	size_t _renderable_count;
	std::vector<float> _uniform_array_model_matrix;
	std::vector<float> _uniform_array_smoothness;
	std::vector<GLuint> _uniform_array_tex_diffuse_rgb;
	std::vector<GLuint> _uniform_array_tex_normal_map;
	std::vector<GLuint> _uniform_array_tex_specular_intensity;
};

} // namespace deferred_lighting

#endif //TECHNIQUE_DEFERRED_LIGHTING_FRAME_H_