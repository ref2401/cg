#ifndef TECHNIQUE_FUR_SIMULATION_FUR_SIMULATION_OPENGL_H_
#define TECHNIQUE_FUR_SIMULATION_FUR_SIMULATION_OPENGL_H_

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
		return _render_vao_id;
	}

	GLsizei vertex_count() const noexcept
	{
		return _vertex_count;
	}

	const Texture_buffer<Buffer_gpu>& tbo_position_buffer() const noexcept
	{
		return _tbo_position_buffer;
	}

	const Texture_buffer<Buffer_gpu>& tbo_simulation_buffer() const noexcept
	{
		return (_read_from_physics_0) ? _tbo_simulation_buffer_0 : _tbo_simulation_buffer_1;
	}


	void end_frame() const noexcept { _read_from_physics_0 = !_read_from_physics_0; }

private:

	void init_physics_simulation_0_vao(const Model_geometry_layout& layout);

	void init_physics_simulation_1_vao(const Model_geometry_layout& layout);

	std::vector<cg::data::Model_mesh_info> _meshes;
	Texture_buffer<Buffer_gpu> _tbo_position_buffer;
	Texture_buffer<Buffer_gpu> _tbo_simulation_buffer_0;
	Texture_buffer<Buffer_gpu> _tbo_simulation_buffer_1;
	Buffer_gpu _model_attribs_buffer;
	Buffer_gpu _index_buffer;
	GLuint _blank_vao_id = Invalid::vao_id;
	GLuint _physics_0_vao_id = Invalid::vao_id;
	GLuint _physics_1_vao_id = Invalid::vao_id;
	GLuint _render_vao_id = Invalid::vao_id;
	size_t _vertex_count = 0;
	mutable bool _read_from_physics_0 = true;
};

class Physics_simulation_pass final {
public:

	Physics_simulation_pass();

	Physics_simulation_pass(const Physics_simulation_pass&) = delete;

	Physics_simulation_pass(Physics_simulation_pass&&) = delete;


	Physics_simulation_pass& operator=(const Physics_simulation_pass&) = delete;

	Physics_simulation_pass& operator=(Physics_simulation_pass&&) = delete;


	void begin(const cg::float3& graviy_ms, float strand_length) noexcept;

	void end() noexcept;

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


	void perform(const Geometry_buffers& geometry_buffers, const cg::mat4& pvm_matrix) noexcept;

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

	void update(float dt) override;

private:

	void update_projection_matrix();

	// camera
	cg::mat4 _projection_matrix;
	cg::Viewpoint _curr_viewpoint;
	cg::Viewpoint _prev_viewpoint;
	cg::float2 _view_roll_angles;
	cg::float2 _prev_mouse_pos_ndc;
	// model
	cg::mat4 _model_matrix;
	Geometry_buffers _geometry_buffers;
	// render
	Physics_simulation_pass _physics_pass;
	Strand_debug_pass _strand_debug_pass;
};

} // fur_simulation

#endif // TECHNIQUE_FUR_SIMULATION_FUR_SIMULATION_OPENGL_H_
