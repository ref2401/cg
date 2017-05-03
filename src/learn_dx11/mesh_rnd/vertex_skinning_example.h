#ifndef LEARN_DX11_MESH_RND_VERTEX_SKINNING_EXAMPLE_H_
#define LEARN_DX11_MESH_RND_VERTEX_SKINNING_EXAMPLE_H_

#include <memory>
#include "cg/base/math.h"
#include "learn_dx11/base/app.h"
#include "learn_dx11/base/dx11.h"
#include "learn_dx11/base/bob_lamp_model.h"


namespace learn_dx11 {
namespace mesh_rnd {

class Vertex_skinning_example final : public Example {
public:

	static constexpr bool init_depth_stencil_view = true;


	Vertex_skinning_example(Render_context& rnd_ctx);

	Vertex_skinning_example(const Vertex_skinning_example&) = delete;

	Vertex_skinning_example(Vertex_skinning_example&&) noexcept = delete;


	Vertex_skinning_example& operator=(const Vertex_skinning_example&) = delete;

	Vertex_skinning_example& operator=(Vertex_skinning_example&&) noexcept = delete;


	void on_viewport_resize(const uint2& viewport_size) override;

	void render() override;

	void update(float dt_milliseconds) override;

private:

	struct Draw_indexed_params final {
		com_ptr<ID3D11Texture2D> _tex_diffuse_rgb;
		com_ptr<ID3D11ShaderResourceView> _tex_diffuse_rgb_srv;
		size_t index_count = 0;
		size_t index_offset = 0;
		size_t base_vertex = 0;
	};

	void init_cbuffers();

	void init_draw_indexed_params(const std::vector<Mesh_draw_params>& mesh_draw_params);

	void init_geometry();

	void init_render_states();

	void init_shaders();

	void setup_pipeline_state();

	void update_projection_matrix(float wh_aspect_ratio);

	void update_projection_view_matrices();

	Hlsl_shader_set _shader_set;
	com_ptr<ID3D11Buffer> _scene_cbuffer;
	com_ptr<ID3D11Buffer> _model_cbuffer;
	com_ptr<ID3D11Buffer> _model_bone_matrices_buffer;
	com_ptr<ID3D11ShaderResourceView> _model_bone_matrices_buffer_srv;
	com_ptr<ID3D11Buffer> _vertex_buffer;
	com_ptr<ID3D11Buffer> _index_buffer;
	com_ptr<ID3D11InputLayout> _input_layout;
	com_ptr<ID3D11SamplerState> _linear_sampler_state;
	// renderer state
	com_ptr<ID3D11DepthStencilState> _depth_stencil_state;
	com_ptr<ID3D11RasterizerState> _rasterizer_state;
	// scene
	std::vector<Draw_indexed_params> _draw_indexed_params;
	std::unique_ptr<Model_animation> _model_animation;
	std::vector<float> _curr_bone_matrices_data;
	float4x4 _projection_matrix;
	float4x4 _view_matrix;
	float4x4 _model_matrix;
};

} // namespace mesh_rnd
} // namespace learn_dx11

#endif // LEARN_DX11_MESH_RND_VERTEX_SKINNING_EXAMPLE_H_

