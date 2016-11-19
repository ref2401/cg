#ifndef LEARN_DX11_MESH_RND_STATIC_MESH_EXAMPLE_H_
#define LEARN_DX11_MESH_RND_STATIC_MESH_EXAMPLE_H_

#include "cg/math/math.h"
#include "learn_dx11/base/app.h"
#include "learn_dx11/base/dx11.h"


namespace learn_dx11 {
namespace mesh_rnd {


class Static_mesh_example final : public Example {
public:

	Static_mesh_example(Render_context& rnd_ctx);

	Static_mesh_example(const Static_mesh_example&) = delete;

	Static_mesh_example(Static_mesh_example&&) noexcept = delete;


	Static_mesh_example& operator=(const Static_mesh_example&) = delete;

	Static_mesh_example& operator=(Static_mesh_example&&) noexcept = delete;

	void on_viewport_resize(const cg::uint2& viewport_size) override;

	void render() override;

	void update() override;

private:

	void init_cbuffers();

	void init_geometry();

	void init_shaders();

	// Calculates projection & view matrices and writes values to the _scene_cbuffer.
	void setup_projection_view_matrices(float wh_aspect_ratio);

	// Sets up the pipeline after initialization is complete.
	void setup_pipeline_state();

	Hlsl_shader_set _shader_set;
	Com_ptr<ID3D11Buffer> _vertex_buffer;
	Com_ptr<ID3D11InputLayout> _input_layout;
	Com_ptr<ID3D11Buffer> _index_buffer;
	// scene
	size_t _model_index_count;
	cg::mat4 _projection_matrix;
	cg::mat4 _view_matrix;
	cg::mat4 _model_matrix;
	Com_ptr<ID3D11Buffer> _scene_cbuffer;
	Com_ptr<ID3D11Buffer> _model_cbuffer;
};

} // mesh_rnd
} // learn_dx11

#endif // LEARN_DX11_MESH_RND_STATIC_MESH_EXAMPLE_H_
