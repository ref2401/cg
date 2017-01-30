#ifndef TECHNIQUE_DEFERRED_LIGHTING_DEFERRED_LIGHTING_H_
#define TECHNIQUE_DEFERRED_LIGHTING_DEFERRED_LIGHTING_H_

#include <array>
#include <vector>
#include "cg/math/math.h"
#include "cg/sys/app.h"
#include "technique/deferred_lighting/frame.h"
#include "technique/deferred_lighting/render.h"
#include "technique/deferred_lighting/rnd/opengl.h"


namespace deferred_lighting {

struct Material final {
	Material() noexcept = default;

	Material(float smoothness,
		rnd::Texture_2d_immut tex_diffuse_rgb,
		rnd::Texture_2d_immut tex_normal_map,
		rnd::Texture_2d_immut tex_specular_intensity) noexcept;

	~Material() noexcept = default;


	float smoothness = 0.f;
	rnd::Texture_2d_immut tex_diffuse_rgb;
	rnd::Texture_2d_immut tex_normal_map;
	rnd::Texture_2d_immut tex_specular_intensity;
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

	Material_instance teapot_material() const noexcept
	{
		return  get_material_instance(_teapot_material);
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
		const rnd::Texture_2d_immut& tex_specular_intensity_override) const noexcept
	{
		return Material_instance(material.smoothness,
			material.tex_diffuse_rgb.id(),
			material.tex_normal_map.id(),
			tex_specular_intensity_override.id());
	}

	Material _default_material;
	Material _brick_wall_material;
	Material _chess_board_material;
	Material _teapot_material;
	Material _wooden_box_material;
};

class Deferred_lighting final : public cg::sys::Example {
public:

	explicit Deferred_lighting(const cg::sys::App_context& app_ctx);

	~Deferred_lighting() noexcept override = default;


	void on_mouse_move() override;

	void on_window_resize() override;

	void render(float interpolation_factor) override;

	void update(float dt) override;

private:

	void begin_render(float interpolation_factor);

	void end_render();

	void init_geometry();

	void init_renderables();

	void update_viewpoint_projection();

	// scene
	Directional_light _dir_light;
	std::vector<Renderable> _rednerable_objects;
	// scene viewpoint (camera)
	cg::mat4 _projection_matrix;
	cg::Viewpoint _curr_viewpoint;
	cg::Viewpoint _prev_viewpoint;
	std::array<cg::float3, 4> _far_plane_coords; // left-bottom, right-bottom, right-top, left-top
	// renderer stuff
	Renderer _renderer;
	Frame _frame;
	// scene data
	Static_vertex_spec_builder _vs_builder;
	Static_vertex_spec _vertex_spec0;
	DE_cmd _cmd_cube;
	DE_cmd _cmd_rect_2x2_repeat;
	DE_cmd _cmd_teapot;
	Material_library _material_library;
	// viewpoint mouse rotation stuff
	cg::float2 _view_roll_angles;
	cg::float2 _prev_mouse_pos_ndc;
};

} // namespace deferred_lighting

#endif // TECHNIQUE_DEFERRED_LIGHTING_DEFERRED_LIGHTING_H_