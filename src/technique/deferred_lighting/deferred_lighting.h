#ifndef TECHNIQUE_DEFERRED_LIGHTING_DEFERRED_LIGHTING_H_
#define TECHNIQUE_DEFERRED_LIGHTING_DEFERRED_LIGHTING_H_

#include <vector>
#include "cg/math/math.h"
#include "cg/rnd/opengl/opengl.h"
#include "cg/sys/app.h"
#include "technique/deferred_lighting/frame.h"
#include "technique/deferred_lighting/render.h"


namespace deferred_lighting {

struct Material final {
	Material() noexcept = default;

	Material(float smoothness,
		cg::rnd::opengl::Texture_2d_immut tex_diffuse_rgb,
		cg::rnd::opengl::Texture_2d_immut tex_normal_map,
		cg::rnd::opengl::Texture_2d_immut tex_specular_intensity) noexcept;

	~Material() noexcept = default;


	float smoothness = 0.f;
	cg::rnd::opengl::Texture_2d_immut tex_diffuse_rgb;
	cg::rnd::opengl::Texture_2d_immut tex_normal_map;
	cg::rnd::opengl::Texture_2d_immut tex_specular_intensity;
};

// Provides a predefined set of Material_instance objects.
// Manages texture objects that define various materials.
class Material_library final {
public:

	Material_library();

	Material_library(const Material_library&) = delete;

	Material_library(Material_library&&) noexcept = delete;

	~Material_library() noexcept = default;


	Material_instance brick_wall_material() const noexcept
	{
		return get_material_instance(_brick_wall_material);
	}

	Material_instance chess_board_material() const noexcept
	{
		return get_material_instance(_chess_board_material);
	}

	Material_instance default_material() const noexcept
	{
		return get_material_instance(_default_material);
	}

	Material_instance wooden_box_material() const noexcept
	{
		return get_material_instance(_wooden_box_material);
	}

private:

	// Constructs Material_instance object from the specified material objecj.
	Material_instance get_material_instance(const Material& material) const noexcept
	{
		return Material_instance(material.smoothness,
			material.tex_diffuse_rgb.id(),
			material.tex_normal_map.id(),
			material.tex_specular_intensity.id());
	}

	// Constructs Material_instance object from the specified material objecj.
	// tex_specular_intensity is used instead of material.tex_specular_intensity.
	Material_instance get_material_instance_specular_intensity(const Material& material,
		const cg::rnd::opengl::Texture_2d_immut& tex_specular_intensity_override) const noexcept
	{
		return Material_instance(material.smoothness,
			material.tex_diffuse_rgb.id(),
			material.tex_normal_map.id(),
			tex_specular_intensity_override.id());
	}

	Material _default_material;
	Material _brick_wall_material;
	Material _chess_board_material;
	Material _wooden_box_material;
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

	void init_geometry();

	void init_renderables();

	// scene
	std::vector<Renderable> _rednerable_objects;
	cg::mat4 _projection_matrix;
	cg::Viewpoint _curr_viewpoint;
	cg::Viewpoint _prev_viewpoint;
	Directional_light _dir_light;
	// renderer stuff
	Renderer _renderer;
	Frame _frame;
	// scene data
	cg::rnd::opengl::Static_vertex_spec_builder _vs_builder;
	cg::rnd::opengl::Static_vertex_spec _vertex_spec0;
	cg::rnd::opengl::DE_cmd _cmd_cube;
	cg::rnd::opengl::DE_cmd _cmd_rect_2x2_repeat;
	cg::rnd::opengl::DE_cmd _cmd_teapot_base;
	Material_library _material_library;
	// viewpoint mouse rotation stuff
	cg::float2 _view_roll_angles;
	cg::float2 _prev_mouse_pos_ndc;
};

} // namespace deferred_lighting

#endif // TECHNIQUE_DEFERRED_LIGHTING_DEFERRED_LIGHTING_H_