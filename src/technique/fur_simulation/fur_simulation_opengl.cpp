#include "technique/fur_simulation/fur_simulation_opengl.h"

#include "cg/data/image.h"
#include "cg/data/shader.h"


namespace fur_simulation {

// ----- Fur_generation_program -----

Fur_generation_program::Fur_generation_program()
{
	auto glsl_source = cg::data::load_glsl_program_data("../data/fur_simulation/fur_generation");
	_program = Glsl_program("fur-generation", glsl_source);
	_g_projection_view_matrix_location = _program.get_uniform_location("g_projection_view_matrix");
	_g_model_matrix_location = _program.get_uniform_location("g_model_matrix");
}

void Fur_generation_program::bind(const mat4& projection_view_matrix, const mat4& model_matrix) noexcept
{
	glUseProgram(_program.id());
	set_uniform(_g_projection_view_matrix_location, projection_view_matrix);
	set_uniform(_g_model_matrix_location, model_matrix);
}

// ----- Fur_simulation_opengl_example -----

Fur_simulation_opengl_example::Fur_simulation_opengl_example(const cg::sys::App_context& app_ctx) :
	Example(app_ctx)
{
	_view_matrix = view_matrix(float3(0, 0, 5), float3::zero);
	init_model();

	glEnable(GL_DEPTH_TEST);
}

void Fur_simulation_opengl_example::init_model()
{
	using Model_geometry_data_t = cg::data::Model_geometry_data<cg::data::Vertex_attribs::p_n>;

	_model_matrix = mat4::identity;
	auto model_data = cg::data::load_model<Model_geometry_data_t::Format::attribs>("../data/sphere-20x20.obj");
	_draw_params = model_data.meshes()[0];

	glCreateVertexArrays(1, &_vao_id);
	glBindVertexArray(_vao_id);

	// vertex buffer
	constexpr GLuint vb_binding_index = 0;

	_vertex_buffer = Buffer_gpu(model_data.vertex_data_byte_count(), model_data.vertex_data().data());
	glVertexArrayVertexBuffer(_vao_id, vb_binding_index, _vertex_buffer.id(), 
		0, Model_geometry_data_t::Format::vertex_byte_count);

	// position
	glEnableVertexArrayAttrib(_vao_id, 0);
	glVertexArrayAttribBinding(_vao_id, 0, vb_binding_index);
	glVertexArrayAttribFormat(_vao_id, 0, 3, GL_FLOAT, false, Model_geometry_data_t::Format::position_byte_offset);

	// normal
	glEnableVertexArrayAttrib(_vao_id, 1);
	glVertexArrayAttribBinding(_vao_id, 1, vb_binding_index);
	glVertexArrayAttribFormat(_vao_id, 1, 3, GL_FLOAT, false, Model_geometry_data_t::Format::normal_byte_offset);

	// index buffer
	_index_buffer = Buffer_gpu(model_data.index_data_byte_count(), model_data.index_data().data());
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _index_buffer.id());
}

void Fur_simulation_opengl_example::on_window_resize()
{
	auto vp_size = _app_ctx.window.viewport_size();
	glViewport(0, 0, vp_size.width, vp_size.height);
	update_projection_matrix();
}

void Fur_simulation_opengl_example::render(float interpolation_factor)
{
	glClearColor(0.2f, 0.3f, 0.2f, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	_glsl_fur_generation.bind(_projection_matrix * _view_matrix, _model_matrix);
	glDrawElements(GL_TRIANGLES, _draw_params.index_count, GL_UNSIGNED_INT, nullptr);
}

void Fur_simulation_opengl_example::update(float dt)
{
}

void Fur_simulation_opengl_example::update_projection_matrix()
{
	_projection_matrix = perspective_matrix_opengl(cg::pi_3,
		_app_ctx.window.viewport_size().aspect_ratio(), 0.1f, 50.0f);
}

} // fur_simulation
