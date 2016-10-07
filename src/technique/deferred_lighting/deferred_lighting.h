#ifndef TECHNIQUE_DEFERRED_LIGHTING_DEFERRED_LIGHTING_H_
#define TECHNIQUE_DEFERRED_LIGHTING_DEFERRED_LIGHTING_H_

#include <vector>
#include "cg/math/math.h"
#include "cg/opengl/opengl.h"
#include "cg/sys/app.h"
#include "technique/deferred_lighting/frame.h"
#include "technique/deferred_lighting/render.h"


namespace deferred_lighting {

class Material final {
public:

	Material() noexcept = default;

	Material(float smoothness, cg::opengl::Texture_2d_immut tex_normal_map) noexcept;

	~Material() noexcept = default;


	float smoothness = 0.f;
	cg::opengl::Texture_2d_immut tex_normal_map;
};

class Deferred_lighting final : public cg::sys::Game {
public:

	Deferred_lighting(cg::sys::Application_context_i& ctx);

	~Deferred_lighting() noexcept override = default;


	void begin_render(float blend_factor) override;

	void end_render() override;

	void on_mouse_click() override {}

	void on_mouse_move() override;

	void on_window_resize() override;

	void render() override;

	void update(float dt) override;

private:

	// scene data
	cg::opengl::Static_vertex_spec_builder _vs_builder;
	cg::opengl::Static_vertex_spec _vertex_spec0;
	Material _material;
	cg::opengl::Texture_2d_immut _tex_default_normal_map;
	// scene
	std::vector<Renderable> _rednerable_objects;
	cg::mat4 _projection_matrix;
	cg::Viewpoint _curr_viewpoint;
	cg::Viewpoint _prev_viewpoint;
	Directional_light _dir_light;
	// renderer stuff
	Renderer _renderer;
	Frame _frame;
	//
	cg::float2 _view_roll_angles;
	cg::float2 _prev_mouse_pos_ndc;
};

} // namespace deferred_lighting

#endif // TECHNIQUE_DEFERRED_LIGHTING_DEFERRED_LIGHTING_H_