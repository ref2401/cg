#include "technique/fur_simulation/fur_simulation_opengl.h"

#include <cassert>
#include <utility>
#include "cg/data/image.h"
#include "cg/data/shader.h"


using namespace cg;
using cg::data::Image_2d;
using cg::data::Image_format;
using cg::sys::Key;


namespace fur_simulation {

// ----- Geometry_buffers -----

Geometry_buffers::Geometry_buffers(float strand_lenght, const char* geometry_filename)
{
	Model_geometry_data geometry_data(strand_lenght, geometry_filename);

	//_position_buffer = Buffer_gpu();
	_meshes = std::move(geometry_data.meshes);
}

void Geometry_buffers::init_physics_simulation_vao()
{

}

// ----- Fur_simulation_opengl_example -----

Fur_simulation_opengl_example::Fur_simulation_opengl_example(const cg::sys::App_context& app_ctx) :
	Example(app_ctx),
	_curr_viewpoint(float3(0, 0, 7), float3(0, 0, 0)),
	_prev_viewpoint(_curr_viewpoint),
	_geometry_buffers(0.3f, "../data/sphere-20x20.obj")
{
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ZERO, GL_ONE);
	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
}

void Fur_simulation_opengl_example::on_keyboard()
{
	/*if (_app_ctx.keyboard.is_down(Key::digit_1)) {
		_curr_material = &_cat_material;
	}
	else if(_app_ctx.keyboard.is_down(Key::digit_2)) {
		_curr_material = &_curly_red_material;
	}
	else if (_app_ctx.keyboard.is_down(Key::digit_3)) {
		_curr_material = &_hair_material;
	}
	else if (_app_ctx.keyboard.is_down(Key::digit_4)) {
		_curr_material = &_sheep_material;
	}*/
}

void Fur_simulation_opengl_example::on_mouse_move()
{
	if (!_app_ctx.mouse.middle_down()) return;

	float2 pos_ndc = _app_ctx.mouse.get_ndc_position(_app_ctx.window.viewport_size());
	float2 offset_ndc = pos_ndc - _prev_mouse_pos_ndc;
	if (offset_ndc == float2::zero) return;

	_prev_mouse_pos_ndc = pos_ndc;

	bool y_offset_satisfies = !approx_equal(offset_ndc.y, 0.f, 0.01f);
	bool x_offset_satisfies = !approx_equal(offset_ndc.x, 0.f, ((y_offset_satisfies) ? 0.01f : 0.001f));

	// mouse offset by x means rotation around OY (yaw)
	if (x_offset_satisfies) {
		_view_roll_angles.y += (offset_ndc.x > 0.f) ? -pi_64 : pi_64;
	}

	// mouse offset by x means rotation around OX (pitch)
	if (y_offset_satisfies) {
		_view_roll_angles.x += (offset_ndc.y > 0.f) ? pi_64 : -pi_64;
	}
}

void Fur_simulation_opengl_example::on_window_resize()
{
	auto vp_size = _app_ctx.window.viewport_size();
	glViewport(0, 0, vp_size.width, vp_size.height);
	update_projection_matrix();
}

void Fur_simulation_opengl_example::render(float interpolation_factor)
{
	float3 view_position = lerp(_prev_viewpoint.position, _curr_viewpoint.position, interpolation_factor);
	mat4 view_matrix = ::view_matrix(_prev_viewpoint, _curr_viewpoint, interpolation_factor);
	mat4 projection_view_matrix = _projection_matrix* view_matrix;

	const static float3 color = rgb(0x817c6e);
	glClearColor(color.r, color.g, color.b, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Fur_simulation_opengl_example::update(float dt)
{
	if (_view_roll_angles != float2::zero) {
		float dist = _curr_viewpoint.distance();
		float3 ox = cross(_curr_viewpoint.forward(), _curr_viewpoint.up);
		ox.y = 0.0f; // ox is always parallel the world's OX.
		ox = normalize(ox);

		if (!approx_equal(_view_roll_angles.y, 0.0f)) {
			quat q = from_axis_angle_rotation(float3::unit_y, _view_roll_angles.y);
			_curr_viewpoint.position = dist * normalize(rotate(q, _curr_viewpoint.position));

			ox = rotate(q, ox);
			ox.y = 0.0f;
			ox = normalize(ox);
		}

		if (!approx_equal(_view_roll_angles.x, 0.0f)) {
			quat q = from_axis_angle_rotation(ox, _view_roll_angles.x);
			_curr_viewpoint.position = dist * normalize(rotate(q, _curr_viewpoint.position));
		}

		_curr_viewpoint.up = normalize(cross(ox, _curr_viewpoint.forward()));
	}

	//static float rot_angle = 0;

	//if (!approx_equal(_view_roll_angles.y, 0.0f)) {
	//	rot_angle += _view_roll_angles.y;
	//	_model_matrix = rotation_matrix_oy<mat4>(rot_angle) * scale_matrix<mat4>(float3(2.0f));
	//}

	_view_roll_angles = float2::zero;
}

void Fur_simulation_opengl_example::update_projection_matrix()
{
	_projection_matrix = perspective_matrix_opengl(cg::pi_3,
		_app_ctx.window.viewport_size().aspect_ratio(), 0.1f, 1000.0f);
}

} // fur_simulation
