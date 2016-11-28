#ifndef LEARN_DX11_MESH_RND_VERTEX_SKINNING_EXAMPLE_H_
#define LEARN_DX11_MESH_RND_VERTEX_SKINNING_EXAMPLE_H_

#include <memory>
#include "cg/math/math.h"
#include "learn_dx11/base/app.h"
#include "learn_dx11/base/dx11.h"
#include "learn_dx11/base/bob_lamp_model.h"


namespace learn_dx11 {
namespace mesh_rnd {

class Vertex_skinning_example final : public Example {
public:
	Vertex_skinning_example(Render_context& rnd_ctx);

	Vertex_skinning_example(const Vertex_skinning_example&) = delete;

	Vertex_skinning_example(Vertex_skinning_example&&) noexcept = delete;


	Vertex_skinning_example& operator=(const Vertex_skinning_example&) = delete;

	Vertex_skinning_example& operator=(Vertex_skinning_example&&) noexcept = delete;


	void on_viewport_resize(const cg::uint2& viewport_size) override;

	void render() override;

	void update(float dt_milliseconds) override;

private:

	void init_cbuffers();

	void init_geometry();

	void init_shaders();

	void setup_pipeline_state();

	void update_projection_matrix(float wh_aspect_ratio);

	void setup_projection_view_matrices();

	Hlsl_shader_set _shader_set;
	Com_ptr<ID3D11Buffer> _scene_cbuffer;
	Com_ptr<ID3D11Buffer> _model_cbuffer;
	Com_ptr<ID3D11Buffer> _model_bone_matrices_buffer;
	Com_ptr<ID3D11ShaderResourceView> _model_bone_matrices_buffer_srv;
	Com_ptr<ID3D11Buffer> _vertex_buffer;
	Com_ptr<ID3D11Buffer> _index_buffer;
	Com_ptr<ID3D11InputLayout> _input_layout;

	// scene
	std::vector<Draw_indexed_params> _draw_params;
	std::unique_ptr<Model_animation> _model_animation;
	cg::mat4 _projection_matrix;
	cg::mat4 _view_matrix;
	cg::mat4 _model_matrix;
};

} // namespace mesh_rnd
} // namespace learn_dx11

#endif // LEARN_DX11_MESH_RND_VERTEX_SKINNING_EXAMPLE_H_

