#ifndef TECHNIQUE_FUR_SIMULATION_FUR_SIMULATION_OPENGL_H_
#define TECHNIQUE_FUR_SIMULATION_FUR_SIMULATION_OPENGL_H_

#include <memory>
#include <vector>
#include "cg/data/model.h"
#include "cg/rnd/opengl/opengl.h"
#include "cg/sys/app.h"
#include "technique/fur_simulation/fur_model.h"
#include "technique/fur_simulation/fur_glsl_program.h"

using namespace cg::rnd::opengl;


namespace fur_simulation {

class Geometry_buffers final {
public:

	Geometry_buffers(float strand_lenght, const char* geometry_filename);

	Geometry_buffers(const Geometry_buffers&) = delete;

	Geometry_buffers(Geometry_buffers&&) = delete;


	Geometry_buffers& operator=(const Geometry_buffers&) = delete;

	Geometry_buffers& operator=(Geometry_buffers&&) = delete;


	const std::vector<cg::data::Model_mesh_info>& meshes() const noexcept
	{
		return _meshes;
	}

	GLuint blank_vao_id() const noexcept
	{
		return _blank_vao_id;
	}

	GLuint physics_vao_id() const noexcept
	{
		return (_read_from_physics_0) ? _physics_0_vao_id : _physics_1_vao_id;
	}

	GLuint render_vao_id() const noexcept
	{
		return (_read_from_physics_0) ? _render_vao_0_id : _render_vao_1_id;
	}

	GLsizei vertex_count() const noexcept
	{
		return _vertex_count;
	}

	const Texture_buffer<Buffer_gpu>& tbo_debug_slot_buffer() const noexcept
	{
		return _tbo_debug_slot;
	}

	const Texture_buffer<Buffer_gpu>& tbo_position_buffer() const noexcept
	{
		return _tbo_position_buffer;
	}

	const Texture_buffer<Buffer_gpu>& tbo_physics_buffer() const noexcept
	{
		return (_read_from_physics_0) ? _tbo_physics_buffer_0 : _tbo_physics_buffer_1;
	}


	void swap_physics_source_dest_buffers() noexcept;

private:

	void init_physics_simulation_0_vao(const Model_geometry_layout& layout);

	void init_physics_simulation_1_vao(const Model_geometry_layout& layout);

	void init_render_0_vao(const Model_geometry_layout& layout);

	void init_render_1_vao(const Model_geometry_layout& layout);


	std::vector<cg::data::Model_mesh_info> _meshes;
	Texture_buffer<Buffer_gpu> _tbo_position_buffer;
	Texture_buffer<Buffer_gpu> _tbo_physics_buffer_0;
	Texture_buffer<Buffer_gpu> _tbo_physics_buffer_1;
	Texture_buffer<Buffer_gpu> _tbo_debug_slot;
	Buffer_immut _model_attribs_buffer;
	Buffer_immut _index_buffer;
	GLuint _blank_vao_id = Blank::vao_id;
	GLuint _physics_0_vao_id = Blank::vao_id;
	GLuint _physics_1_vao_id = Blank::vao_id;
	GLuint _render_vao_0_id = Blank::vao_id;
	GLuint _render_vao_1_id = Blank::vao_id;
	size_t _vertex_count = 0;
	mutable bool _read_from_physics_0 = true;
};

class Material final {
public:

	Material(const Texture_2d_i& tex_diffuse_rgb, const Texture_2d_i& tex_fur_mask,
		const Strand_properties& strand_props) noexcept
		: _tex_diffuse_rgb(tex_diffuse_rgb), _tex_fur_mask(tex_fur_mask), _strand_props(strand_props)
	{}

	Material(const Material&) = delete;

	Material(Material&&) = delete;


	Material& operator=(const Material) = delete;

	Material& operator=(Material&&) = delete;


	const Strand_properties& strand_props() const noexcept
	{
		return _strand_props;
	}

	const Texture_2d_i& tex_diffuse_rgb() const noexcept
	{
		return _tex_diffuse_rgb;
	}

	const Texture_2d_i& tex_fur_mask() const noexcept
	{
		return _tex_fur_mask;
	}

private:

	const Texture_2d_i& _tex_diffuse_rgb;
	const Texture_2d_i& _tex_fur_mask;
	Strand_properties _strand_props;
};

class Material_gallery final {
public:

	Material_gallery();

	Material_gallery(const Material_gallery&) = delete;

	Material_gallery(Material_gallery&&) = delete;


	Material_gallery& operator=(const Material_gallery&) = delete;

	Material_gallery& operator=(Material_gallery&&) = delete;


	const Material& bunny_material() const noexcept
	{
		return *_bunny_material;
	}

	const Material& cat_material() const noexcept
	{
		return *_cat_material;
	}

	const Material& curly_red_material() const noexcept
	{
		return *_curly_red_material;
	}

private:

	Texture_2d_immut _tex_fur_mask;
	Texture_2d_immut _tex_car_diffuse_rgb;
	Texture_2d_immut _tex_red_curl_material;
	Texture_2d_immut _tex_bunny_diffuse_rgb;
	std::unique_ptr<Material> _cat_material;
	std::unique_ptr<Material> _curly_red_material;
	std::unique_ptr<Material> _bunny_material;
};

class Fur_pass final {
public:

	Fur_pass() = default;

	Fur_pass(const Fur_pass&) = delete;

	Fur_pass(Fur_pass&&) = delete;


	Fur_pass& operator=(const Fur_pass&) = delete;

	Fur_pass& operator=(Fur_pass&&) = delete;


	void perform(const Geometry_buffers& geometry_buffers, const Material& material,
		const cg::mat4& pvm_matrix, const cg::mat4& model_matrix,
		const cg::float3& view_position_ws, const cg::float3& light_dir_ws) noexcept;

private:

	Fur_pass_program _program;
};

class Opaque_model_pass final {
public:

	Opaque_model_pass() = default;

	Opaque_model_pass(const Opaque_model_pass&) = delete;

	Opaque_model_pass(Opaque_model_pass&&) = delete;


	Opaque_model_pass& operator=(const Opaque_model_pass&) = delete;

	Opaque_model_pass& operator=(Opaque_model_pass&&) = delete;


	void perform(const Geometry_buffers& geometry_buffers, const Material& material,
		const cg::mat4& projection_view_matrix, const cg::mat4& model_matrix, 
		const cg::float3& dir_to_light_ws) noexcept;

private:

	Opaque_model_pass_program _program;
};

class Physics_simulation_pass final {
public:

	Physics_simulation_pass() = default;

	Physics_simulation_pass(const Physics_simulation_pass&) = delete;

	Physics_simulation_pass(Physics_simulation_pass&&) = delete;


	Physics_simulation_pass& operator=(const Physics_simulation_pass&) = delete;

	Physics_simulation_pass& operator=(Physics_simulation_pass&&) = delete;


	void perform(Geometry_buffers& geometry_buffers, const cg::float4& extern_accel_ms,
		const cg::float3& angular_velocity_ms, float strand_length, 
		const Strand_properties& strand_props) noexcept;

private:

	Physics_simulation_pass_program _program;
};

class Strand_debug_pass final {
public:

	Strand_debug_pass() = default;

	Strand_debug_pass(const Strand_debug_pass&) = delete;

	Strand_debug_pass(Strand_debug_pass&&) = delete;


	Strand_debug_pass& operator=(const Strand_debug_pass&) = delete;

	Strand_debug_pass& operator=(Strand_debug_pass&&) = delete;


	void perform(Geometry_buffers& geometry_buffers, const cg::mat4& pvm_matrix) noexcept;

private:

	Strand_debug_pass_program _program;
};

class Fur_simulation_opengl_example final : public cg::sys::Example {
public:
	
	explicit Fur_simulation_opengl_example(const cg::sys::App_context& app_ctx);


	void on_keyboard() override;

	void on_mouse_move() override;

	void on_window_resize() override;

	void render(float interpolation_factor) override;

	void update(float dt_msec) override;

private:

	struct Model_transform {
		Model_transform() noexcept = default;

		Model_transform(const cg::float3& position, const cg::float3& rotation_axis,
			const cg::float3& scale) noexcept
			: position(position), rotation_axis(rotation_axis), scale(scale)
		{}
			
		cg::float3 position;
		cg::float3 rotation_axis;
		float rotation_angle_total = 0.0f;
		float rotation_angle = 0.0f;
		cg::float3 scale;
	};

	void update_curr_viewpoint();

	void update_projection_matrix();

	// camera
	cg::mat4 _projection_matrix;
	cg::Viewpoint _curr_viewpoint;
	cg::Viewpoint _prev_viewpoint;
	cg::float2 _view_roll_angles;
	cg::float2 _prev_mouse_pos_ndc;
	// model
	Model_transform _model_transform;
	cg::float3 _movement_speed;
	cg::float3 _movement_acceleration;
	//cg::float3 _wind_acceleration;
	cg::mat4 _model_matrix;
	Material_gallery _material_gallery;
	const Material* _curr_material = nullptr;
	Geometry_buffers _geometry_buffers;
	cg::float3 _dir_to_light_ws;
	// render
	Fur_pass _fur_pass;
	Opaque_model_pass _opaque_model_pass;
	Physics_simulation_pass _physics_pass;
	Strand_debug_pass _strand_debug_pass;
};

} // fur_simulation

#endif // TECHNIQUE_FUR_SIMULATION_FUR_SIMULATION_OPENGL_H_
