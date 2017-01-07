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

private:

	void init_physics_simulation_vao();

	std::vector<cg::data::Model_mesh_info> _meshes;
	Buffer_gpu _position_buffer;
	Buffer_gpu _simulation_buffer_0;
	Buffer_gpu _simulation_buffer_1;
	Buffer_gpu _model_attribs_buffer;
	Buffer_gpu _index_buffer;
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
	Geometry_buffers _geometry_buffers;
};

} // fur_simulation

#endif // TECHNIQUE_FUR_SIMULATION_FUR_SIMULATION_OPENGL_H_
