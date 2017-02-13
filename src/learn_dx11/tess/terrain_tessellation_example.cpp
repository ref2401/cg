#include "learn_dx11/tess/terrain_tessellation_example.h"

#include <cassert>
#include <iostream>
#include <iterator>
#include <vector>

using namespace cg;


namespace {

class Terrain_model final {
public:

	Terrain_model(const uint2& cell_count);

	Terrain_model(const Terrain_model&) = delete;

	Terrain_model(Terrain_model&&) = delete;


	Terrain_model& operator=(const Terrain_model) = delete;

	Terrain_model& operator=(Terrain_model&&) = delete;


	const std::vector<float>& vertex_attribs() const noexcept
	{
		return _vertex_attribs;
	}

	const std::vector<uint32_t>& indices() const noexcept
	{
		return _indices;
	}

private:

	// each vertex has position(float3) and tex_coord(float2) -> 5 components per vertex.
	static constexpr size_t vertex_component_count = 5;

	void init_vertex_attribs(const uint2& cell_count);

	//void init_indices();

	
	std::vector<float> _vertex_attribs;
	std::vector<uint32_t> _indices;
	size_t _vertex_count = 0;
};

Terrain_model::Terrain_model(const uint2& cell_count)
	: _vertex_count((cell_count.x + 1) * (cell_count.y + 1))
{
	assert(greater_than(cell_count, 0));

	_vertex_attribs.reserve(_vertex_count * Terrain_model::vertex_component_count);
	_indices.resize(6 * cell_count.square());
	init_vertex_attribs(cell_count);
	//init_indices();
}

void Terrain_model::init_vertex_attribs(const uint2& cell_count)
{
	// positions are in range [(-0.5f, 0.0f, -0.5f), (0.5f, 0.0f, 0.5f)]
	// tex_coords are in range [(0.0, 0.0), (1.0, 1.0)]

	for (uint32_t r = 0; r < cell_count.x + 1; ++r) {
		const float x = float(r) / cell_count.x;

		for (uint32_t c = 0; c < cell_count.y + 1; ++c) {
			const float y = float(c) / cell_count.y;
			const float vert[Terrain_model::vertex_component_count] = {
				x - 0.5f, 0.0f, y - 0.5f,	// position
				x, y						// tex_coord
			};

			_vertex_attribs.insert(_vertex_attribs.cend(), vert, vert + Terrain_model::vertex_component_count);
		}
	}
}

} // namespace


namespace learn_dx11 {
namespace tess {

// ----- Terrain_tessellation_example -----

Terrain_tessellation_example::Terrain_tessellation_example(Render_context& rnd_ctx) 
	: Example(rnd_ctx)
{
	Terrain_model terrain_model(uint2(3, 3));
}

void Terrain_tessellation_example::on_viewport_resize(const uint2& viewport_size)
{
}

void Terrain_tessellation_example::render()
{
}

} // namespace tess
} // namespace learn_dx11
