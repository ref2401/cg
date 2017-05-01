#ifndef LEARN_DX11_TESS_TERRAIN_GRID_MODEL_H_
#define LEARN_DX11_TESS_TERRAIN_GRID_MODEL_H_

#include <vector>
#include "cg/base/container.h"


namespace learn_dx11 {
namespace tess {

// each vertex has position(float3) and tex_coord(float2)
// 12 indices for each patch. 
// 4 stands for the cell itselft (bottom left, bottom right, top left, top right)
// 2 - left cell (bottom left, top left)
// 2 - bottom cell (bottom left, bottom right)
// 2 - right cell (bottom right, top right)
// 2 - top cell (top left, top right)
class Terrain_grid_model final {
public:

	// each vertex has position(float3) and tex_coord(float2) -> 5 components per vertex.
	static constexpr size_t vertex_component_count = 5;


	Terrain_grid_model(int64_t z_cell_count, int64_t x_cell_count);


	size_t patch_count() const noexcept
	{
		return _patch_count;
	}

	size_t index_count() const noexcept
	{
		return _index_buffer.size();
	}

	const float* vertex_buffer_data() const noexcept
	{
		return _vertex_buffer.data();
	}

	const uint32_t* index_buffer_data() const noexcept
	{
		return _index_buffer.data();
	}

	size_t vertex_buffer_byte_count() const noexcept
	{
		return cg::byte_count(_vertex_buffer);
	}

	size_t index_buffer_byte_count() const noexcept
	{
		return cg::byte_count(_index_buffer);
	}

private:

	static constexpr size_t patch_index_count = 12;

	std::vector<float> _vertex_buffer;
	std::vector<uint32_t> _index_buffer;
	const size_t _patch_count = 0;
};

} // namespace tess
} // namespace learn_dx11

#endif // LEARN_DX11_TESS_TERRAIN_GRID_MODEL_H_
