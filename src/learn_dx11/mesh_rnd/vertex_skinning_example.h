#ifndef LEARN_DX11_MESH_RND_VERTEX_SKINNING_EXAMPLE_H_
#define LEARN_DX11_MESH_RND_VERTEX_SKINNING_EXAMPLE_H_

#include "cg/math/math.h"
#include "learn_dx11/base/app.h"
#include "learn_dx11/base/dx11.h"


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

	void update() override;

private:
};

} // namespace mesh_rnd
} // namespace learn_dx11

#endif // LEARN_DX11_MESH_RND_VERTEX_SKINNING_EXAMPLE_H_

