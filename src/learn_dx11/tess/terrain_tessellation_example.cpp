#include "learn_dx11/tess/terrain_tessellation_example.h"

#include <cassert>
#include <iostream>
#include <iterator>
#include <vector>

using namespace cg;


namespace {

size_t bottom_left_index(size_t row, size_t column, size_t row_count, size_t column_count)
{
	assert(row_count > 0);
	assert(column_count > 0);

	size_t r = cg::clamp(row + 1, size_t(0), row_count);
	size_t c = cg::clamp(column, size_t(0), column_count);

	return r * (row_count + 1) + c;
}

size_t bottom_right_index(size_t row, size_t column, size_t row_count, size_t column_count)
{
	assert(row_count > 0);
	assert(column_count > 0);

	size_t r = cg::clamp(row + 1, size_t(0), row_count);
	size_t c = cg::clamp(column + 1, size_t(0), column_count);

	return r * (row_count + 1) + c;
}

size_t top_left_index(size_t row, size_t column, size_t row_count, size_t column_count)
{
	assert(row_count > 0);
	assert(column_count > 0);

	size_t r = cg::clamp(row, size_t(0), row_count);
	size_t c = cg::clamp(column, size_t(0), column_count);

	return r * (row_count + 1) + c;
}

size_t top_right_index(size_t row, size_t column, size_t row_count, size_t column_count)
{
	assert(row_count > 0);
	assert(column_count > 0);

	size_t r = cg::clamp(row, size_t(0), row_count);
	size_t c = cg::clamp(column + 1, size_t(0), column_count);

	return r * (row_count + 1) + c;
}

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

	void init_indices(const uint2& cell_count);

	
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
}

void Terrain_model::init_vertex_attribs(const uint2& cell_count)
{
	// positions are in range [(-0.5f, 0.0f, -0.5f), (0.5f, 0.0f, 0.5f)]
	// tex_coords are in range [(0.0, 0.0), (1.0, 1.0)]

	const size_t horz_coord_count = cell_count.width + 1;
	const size_t depth_coord_count = cell_count.height + 1;

	for (size_t z = 0; z < depth_coord_count; ++z) {
		const float z_coord = float(z) / cell_count.height;
		
		for (size_t x = 0; x < horz_coord_count; ++x) {
			const float x_coord = float(x) / cell_count.width;
			const float vert[Terrain_model::vertex_component_count] = {
				x_coord - 0.5f, 0.0f, z_coord - 0.5f,	// position
				x_coord, z_coord						// tex_coord
			}; 

			_vertex_attribs.insert(_vertex_attribs.cend(), vert, vert + Terrain_model::vertex_component_count);
		}
	}


	for (size_t r = 0; r < cell_count.height; ++r) {
		for (size_t c = 0; c < cell_count.width; ++c) {

			std::cout << "[" << r << ", " << c << "] ";

			std::cout << bottom_left_index(r, c, cell_count.width, cell_count.height) << " "
				<< bottom_right_index(r, c, horz_coord_count, depth_coord_count) << " "
				<< top_right_index(r, c, horz_coord_count, depth_coord_count) << " "
				<< top_left_index(r, c, horz_coord_count, depth_coord_count) << " ";

		}

		std::cout << std::endl;
	}
}

} // namespace


namespace learn_dx11 {
namespace tess {

// ----- Terrain_tessellation_example -----

Terrain_tessellation_example::Terrain_tessellation_example(Render_context& rnd_ctx) 
	: Example(rnd_ctx)
{
	Terrain_model terrain_model(uint2(3, 2));
}

void Terrain_tessellation_example::on_viewport_resize(const uint2& viewport_size)
{
}

void Terrain_tessellation_example::render()
{
}

} // namespace tess
} // namespace learn_dx11
