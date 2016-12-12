#ifndef TECHNIQUE_FUR_SIMULATION_FUR_SIMULATION_OPENGL_H_
#define TECHNIQUE_FUR_SIMULATION_FUR_SIMULATION_OPENGL_H_

#include "cg/data/model.h"
#include "cg/rnd/opengl/opengl.h"
#include "cg/sys/app.h"

using namespace cg;
using namespace cg::rnd::opengl;


namespace fur_simulation {

class Fur_generation_program final {
public:

	Fur_generation_program();

	Fur_generation_program(const Fur_generation_program&) = delete;

	Fur_generation_program(Fur_generation_program&&) = delete;

	~Fur_generation_program() noexcept = default;


	void bind(const mat4& projection_view_matrix, const mat4& model_matrix) noexcept;

private:

	Glsl_program _program;
	GLint _g_projection_view_matrix_location = Invalid::uniform_location;
	GLint _g_model_matrix_location = Invalid::uniform_location;
};

class Fur_simulation_opengl_example final : public cg::sys::Example {
public:
	
	explicit Fur_simulation_opengl_example(const cg::sys::App_context& app_ctx);


	void on_mouse_move() override;

	void on_window_resize() override;

	void render(float interpolation_factor) override;

	void update(float dt) override;

private:

	void init_model();

	void update_projection_matrix();


	Fur_generation_program _glsl_fur_generation;
	// camera
	mat4 _projection_matrix;
	cg::Viewpoint _curr_viewpoint;
	cg::Viewpoint _prev_viewpoint;
	float2 _view_roll_angles;
	float2 _prev_mouse_pos_ndc;
	// model data
	GLuint _vao_id = Invalid::vao_id;
	Buffer_gpu _vertex_buffer;
	Buffer_gpu _index_buffer;
	cg::data::Model_mesh_info _draw_params;
	mat4 _model_matrix;
};

} // fur_simulation

#endif // TECHNIQUE_FUR_SIMULATION_FUR_SIMULATION_OPENGL_H_
