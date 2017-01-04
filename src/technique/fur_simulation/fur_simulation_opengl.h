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

// ----- Fur_simulation_opengl_example_2 stuff -----

// Common data shared between passes.
class Gbuffer final {
public:

	Gbuffer(const uint2& viewport_size) noexcept;

	Gbuffer(const Gbuffer&) = delete;

	Gbuffer(Gbuffer&&) = delete;


	Gbuffer& operator=(const Gbuffer&) = delete;

	Gbuffer* operator=(Gbuffer&&) = delete;


	void resize(const uint2& viewport_size) noexcept;

	const Renderbuffer& depth_renderbuffer() const noexcept
	{
		return _depth_renderbuffer;
	}

	const Texture_2d_i& tex_fur() const noexcept
	{
		return _tex_fur;
	}

	const Texture_2d_i& tex_geometry() const noexcept
	{
		return _tex_geometry;
	}

	const Texture_2d_i& tex_strand_data() const noexcept
	{
		return _tex_strand_data;
	}

	uint2 viewport_size() const noexcept
	{
		return _viewport_size;
	}

private:

	uint2 _viewport_size;
	Texture_2d _tex_fur;
	Texture_2d _tex_geometry;
	Texture_2d _tex_strand_data;
	Renderbuffer _depth_renderbuffer;
};

class Geometry_pass final {
public:

	Geometry_pass(Gbuffer& fur_render_data);

	Geometry_pass(const Geometry_pass&) = delete;

	Geometry_pass(Geometry_pass&&) = delete;


	Geometry_pass& operator=(const Geometry_pass&) = delete;

	Geometry_pass& operator=(Geometry_pass&&) = delete;


	void begin(const mat4& projection_matrix, const mat4& view_matrix,
		const mat4& model_matrix) noexcept;

	void end() noexcept;

private:

	Gbuffer& _gbuffer;
	Geometry_pass_program _program;
	Framebuffer _fbo;

};

class Fur_spread_pass final {
public:

	Fur_spread_pass(Gbuffer& gbuffer);

	Fur_spread_pass(const Fur_spread_pass&) = delete;

	Fur_spread_pass(Fur_spread_pass&&) = delete;


	Fur_spread_pass& operator=(const Fur_spread_pass&) = delete;

	Fur_spread_pass& operator=(Fur_spread_pass&&) = delete;


	void perform() noexcept;

private:

	Gbuffer& _gbuffer;
	Framebuffer _fbo;
	Fur_spread_pass_program _program;
};

class Final_pass final {
public:

	Final_pass(Gbuffer& gbuffer);

	Final_pass(const Final_pass&) = delete;

	Final_pass(Final_pass&&) = delete;


	Final_pass& operator=(const Final_pass&) = delete;

	Final_pass& operator=(Final_pass&&) = delete;


	void perform() noexcept;

private:

	Gbuffer& _gbuffer;
	Framebuffer _fbo;
};

class Fur_simulation_opengl_example_2 final : public cg::sys::Example {
public:

	explicit Fur_simulation_opengl_example_2(const cg::sys::App_context& app_ctx);

	Fur_simulation_opengl_example_2(const Fur_simulation_opengl_example_2&) = delete;

	Fur_simulation_opengl_example_2(Fur_simulation_opengl_example_2&&) = delete;


	Fur_simulation_opengl_example_2& operator=(const Fur_simulation_opengl_example_2&) = delete;

	Fur_simulation_opengl_example_2& operator=(Fur_simulation_opengl_example_2&&) = delete;
	

	void on_mouse_move() override;

	void on_window_resize() override;

	void render(float interpolation_factor) override;

	void update(float dt) override;

private:

	void init_materials();

	void init_model();

	void update_projection_matrix();

	// camera
	mat4 _projection_matrix;
	cg::Viewpoint _curr_viewpoint;
	cg::Viewpoint _prev_viewpoint;
	float2 _view_roll_angles;
	float2 _prev_mouse_pos_ndc;
	// model
	GLuint _vao_id = Invalid::vao_id;
	Buffer_gpu _vertex_buffer;
	Buffer_gpu _index_buffer;
	size_t _model_index_count;
	mat4 _model_matrix;
	Texture_2d_immut _tex_diffuse_rgb;
	// render
	Gbuffer _gbuffer;
	Geometry_pass _geometry_pass;
	Fur_spread_pass _fur_spread_pass;
	Final_pass _final_pass;
};



} // fur_simulation

#endif // TECHNIQUE_FUR_SIMULATION_FUR_SIMULATION_OPENGL_H_
