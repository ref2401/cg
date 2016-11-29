#ifndef LEARN_DX11_DISPLACEMENT_MAPPING_EXAMPLE_H_
#define LEARN_DX11_DISPLACEMENT_MAPPING_EXAMPLE_H_

#include "cg/math/math.h"
#include "learn_dx11/base/app.h"


namespace learn_dx11 {
namespace mesh_rnd {

class Displacement_mapping_example final : public Example {
public:

	Displacement_mapping_example(Render_context& rnd_ctx);

	Displacement_mapping_example(const Displacement_mapping_example&) = delete;

	Displacement_mapping_example(Displacement_mapping_example&&) noexcept = delete;


	Displacement_mapping_example& operator=(const Displacement_mapping_example&) = delete;

	Displacement_mapping_example& operator=(Displacement_mapping_example&&) noexcept = delete;


	void on_viewport_resize(const cg::uint2& viewport_size) override;

	void render() override;

	void update(float dt_milliseconds) override {}

private:

	void init_shaders();

	void setup_pipeline_state();

	void update_projection_matrix(float wh_aspect_ratio);

	Hlsl_shader_set _shader_set;

	cg::mat4 _projection_matrix;
	cg::mat4 _view_matrix;
	cg::mat4 _model_matrix;
};

} // namespace mesh_rnd
} // namespace learn_dx11

#endif // LEARN_DX11_DISPLACEMENT_MAPPING_EXAMPLE_H_

