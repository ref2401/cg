#include "learn_dx11/tess/terrain_grid_model.h"

#include <cassert>
#include "cg/math/math.h"


namespace {

class Vertex_index_helper final {
public:

	Vertex_index_helper(size_t row_count, size_t column_count) noexcept
		: _row_count(int64_t(row_count)), _column_count(int64_t(column_count))
	{
		assert(row_count > 0);
		assert(column_count > 0);
	}


	uint32_t bottom_left_index(int64_t row, int64_t column) noexcept
	{
		int64_t r = cg::clamp(row, int64_t(0), _row_count);
		int64_t c = cg::clamp(column, int64_t(0), _column_count);

		return uint32_t(r * (_column_count + 1) + c);
	}

	uint32_t bottom_right_index(int64_t row, int64_t column) noexcept
	{
		int64_t r = cg::clamp(row, int64_t(0), _row_count);
		int64_t c = cg::clamp(column + 1, int64_t(0), _column_count);

		return uint32_t(r * (_column_count + 1) + c);
	}

	uint32_t top_left_index(int64_t row, int64_t column) noexcept
	{
		int64_t r = cg::clamp(row + 1, int64_t(0), _row_count);
		int64_t c = cg::clamp(column, int64_t(0), _column_count);

		return uint32_t(r * (_column_count + 1) + c);
	}

	uint32_t top_right_index(int64_t row, int64_t column) noexcept
	{
		int64_t r = cg::clamp(row + 1, int64_t(0), _row_count);
		int64_t c = cg::clamp(column + 1, int64_t(0), _column_count);

		return uint32_t(r * (_column_count + 1) + c);
	}

private:

	int64_t _row_count = 0;
	int64_t _column_count = 0;
};

} // namespace


namespace learn_dx11 {
namespace tess {

Terrain_grid_model::Terrain_grid_model(size_t z_cell_count, size_t x_cell_count)
	: _patch_count(z_cell_count * x_cell_count)
{
	assert(z_cell_count > 0);
	assert(x_cell_count > 0);

	const size_t expected_vertex_attrib_count = (z_cell_count + 1) * (x_cell_count + 1) 
		* Terrain_grid_model::vertex_component_count;
	const size_t expected_index_count = z_cell_count * x_cell_count 
		* Terrain_grid_model::patch_index_count;

	_vertex_buffer.reserve(expected_vertex_attrib_count);
	_index_buffer.reserve(expected_index_count);

	// ----- vertices -----
	// positions are in range [(-0.5f, 0.0f, -0.5f), (0.5f, 0.0f, 0.5f)]
	// tex_coords are in range [(0.0, 0.0), (1.0, 1.0)]
	// bottom-left position of the grid is (-0.5f, 0.0f, 0.5f) tex_coords [0.0f, 0.0f]
	// top-right position of the grid is (0.5f, 0.0f, -0.5f) tex_coords [1.0f, 1.0f]
	const size_t z_coord_count = z_cell_count + 1;
	const size_t x_coord_count = x_cell_count + 1;

	for (size_t z = z_coord_count; z > 0; --z) {
		const float z_coord = float(z - 1) / z_cell_count;

		for (size_t x = 0; x < x_coord_count; ++x) {
			const float x_coord = float(x) / x_cell_count;
			const float vert[Terrain_grid_model::vertex_component_count] = {
				x_coord - 0.5f, 0.0f, z_coord - 0.5f,	// position
				x_coord, z_coord						// tex_coord
			};

			_vertex_buffer.insert(_vertex_buffer.cend(), vert, vert + Terrain_grid_model::vertex_component_count);
		}
	}

	// ----- patch indices -----
	Vertex_index_helper index_helper(z_cell_count, x_cell_count);

	for (int64_t r = 0; r < z_cell_count; ++r) {
		for (int64_t c = 0; c < x_cell_count; ++c) {

			//std::cout << std::endl << "<" << r << ", " << c << ">" << std::endl << std::endl;

			uint32_t patch_indices[patch_index_count];
			patch_indices[0] = index_helper.bottom_left_index(r, c);
			patch_indices[1] = index_helper.bottom_right_index(r, c);
			patch_indices[2] = index_helper.top_left_index(r, c);
			patch_indices[3] = index_helper.top_right_index(r, c);
			// left cell: 
			patch_indices[4] = index_helper.bottom_left_index(r, c - 1);
			patch_indices[5] = index_helper.top_left_index(r, c - 1);
			// bottom cell:
			patch_indices[6] = index_helper.bottom_left_index(r - 1, c);
			patch_indices[7] = index_helper.bottom_right_index(r - 1, c);
			// right cell:
			patch_indices[8] = index_helper.bottom_right_index(r, c + 1);
			patch_indices[9] = index_helper.top_right_index(r, c + 1);
			// top cell:
			patch_indices[10] = index_helper.top_left_index(r + 1, c);
			patch_indices[11] = index_helper.top_right_index(r + 1, c);

			_index_buffer.insert(_index_buffer.cend(), patch_indices, patch_indices + patch_index_count);
		}
	}

	assert(_vertex_buffer.size() == expected_vertex_attrib_count);
	assert(_index_buffer.size() == expected_index_count);
}

} // namespace tess
} // namespace learn_dx11
