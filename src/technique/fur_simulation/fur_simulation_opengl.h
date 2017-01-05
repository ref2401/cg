#ifndef TECHNIQUE_FUR_SIMULATION_FUR_SIMULATION_OPENGL_H_
#define TECHNIQUE_FUR_SIMULATION_FUR_SIMULATION_OPENGL_H_

#include <vector>
#include "cg/data/model.h"
#include "cg/rnd/opengl/opengl.h"
#include "cg/sys/app.h"
#include "technique/fur_simulation/fur_model.h"
#include "technique/fur_simulation/glsl_program.h"

using namespace cg;
using namespace cg::rnd::opengl;


namespace fur_simulation {

class Fur_simulation_opengl_example final : public cg::sys::Example {
public:
	
	explicit Fur_simulation_opengl_example(const cg::sys::App_context& app_ctx);


	void on_keyboard() override;

	void on_mouse_move() override;

	void on_window_resize() override;

	void render(float interpolation_factor) override;

	void update(float dt) override;

private:

	void init_materials();

	void init_model();

	void update_projection_matrix();

	Opaque_model_program _glsl_opaque_model;
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
	size_t _model_index_count;
	mat4 _model_matrix;
	// fur rendering
	Material* _curr_material = nullptr;
	Material _cat_material;
	Material _curly_red_material;
	Material _hair_material;
	Material _sheep_material;
	float3 _light_dir_ws; // dir to light
	Texture_2d_immut _tex_fur_mask;
};

} // fur_simulation

#endif // TECHNIQUE_FUR_SIMULATION_FUR_SIMULATION_OPENGL_H_
