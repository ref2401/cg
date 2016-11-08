#ifndef CG_DATA_MESH_H_
#define CG_DATA_MESH_H_

#include <cassert>
#include <cstdint>
#include <ostream>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>
#include "cg/base/base.h"
#include "cg/math/math.h"
#include "cg/data/vertex.h"


namespace cg {
namespace data {


// Describes the order and offset (byte & component) of every vertex attribute.
// The relative order of the attributes is: position, normal, tex_coord, tangent_h.
// All the offsets are calculated accordint to the defaul order.
//
// Each attribute has at least 1 component of some type (offen the type is float).
// Example:
// Interleaved_vertex_format(Vertex_attribs::position | Vertex_attribs::tex_coord)
// position has 3 components: x, y, z; tex_coord has 2 components: u, v.
// total component count = 3 + 2 = 5;
// total byte count = 5 * sizeof(float).
// position's offset:	0 components, 0 bytes (position is first in the relative order).
// tex_coord's offset:	3 components, 3 * sizeof(float) bytes (tex_coord goes right after the position).
struct Interleaved_vertex_format_old final {
	static constexpr size_t component_count_normal = 3;
	static constexpr size_t component_count_position = 3;
	static constexpr size_t component_count_tangent_h = 4;
	static constexpr size_t component_count_tex_coord = 2;


	Interleaved_vertex_format_old() noexcept = default;

	explicit Interleaved_vertex_format_old(Vertex_attribs attribs) noexcept
		: attribs(attribs)
	{}

	// Total size in bytes of the vertex.
	size_t byte_count() const
	{
		return component_count() * sizeof(float);
	}

	// How many bytes are there before normal data starts.
	size_t byte_offset_normal() const
	{
		return component_offset_normal() * sizeof(float);
	}

	// How many bytes are there before position data starts.
	size_t byte_offset_position() const
	{
		return component_offset_position() * sizeof(float);
	}

	// How many bytes are there before tangent_h data starts.
	size_t byte_offset_tangent_h() const
	{
		return component_offset_tangent_h() * sizeof(float);
	}

	// How many bytes are there before tex_coord data starts.
	size_t byte_offset_tex_coord() const
	{
		return component_offset_tex_coord() * sizeof(float);
	}

	// Total number of components in the vertex.
	size_t component_count() const noexcept
	{
		size_t counter = 0;
		if (has_position(attribs)) counter += component_count_position;
		if (has_normal(attribs)) counter += component_count_normal;
		if (has_tex_coord(attribs)) counter += component_count_tex_coord;
		if (has_tangent_space(attribs)) counter += component_count_tangent_h;

		return counter;
	}

	// How many components are there before normal data starts.
	size_t component_offset_normal() const noexcept
	{
		size_t prior_cmpt_count = has_position(attribs)
			? (Interleaved_vertex_format_old::component_count_position)
			: 0u;
		return component_offset_position() + prior_cmpt_count;
	}

	// How many components are there before position data starts.
	size_t component_offset_position() const noexcept
	{
		return 0u;
	}

	// How many components are there before tangent_h data starts.
	size_t component_offset_tangent_h() const noexcept
	{
		size_t prior_cmpt_count = has_tex_coord(attribs)
			? (Interleaved_vertex_format_old::component_count_tex_coord)
			: 0u;
		return component_offset_tex_coord() + prior_cmpt_count;
	}

	// How many components are there before tex_coord data starts.
	size_t component_offset_tex_coord() const noexcept
	{
		size_t prior_cmpt_count = has_normal(attribs)
			? (Interleaved_vertex_format_old::component_count_normal)
			: 0u;
		return component_offset_normal() + prior_cmpt_count;
	}


	Vertex_attribs attribs = Vertex_attribs::none;
};

// Interleaved_mesh_data is used to pack and store mesh data that is goint to be fed to the GPU.
// Implementation details: does not support base vertex counter 
// which means do not put more than one mesh into an Interleaved_mesh_data instance.
class Interleaved_mesh_data_old final {
public:

	Interleaved_mesh_data_old() noexcept = default;

	explicit Interleaved_mesh_data_old(Vertex_attribs attribs)
		: Interleaved_mesh_data_old(attribs, 0, 0)
	{}

	Interleaved_mesh_data_old(Vertex_attribs attribs, size_t vertex_count, size_t index_count);
	


	Vertex_attribs attribs() const
	{
		return _format.attribs;
	}

	// Returns interleaved attributes buffer.
	const std::vector<float>& data() const
	{
		return _data;
	}

	// Represents format of this mesh data. Use format property to set offsets & strides.
	Interleaved_vertex_format_old format() const
	{
		return _format;
	}

	// How many indices are in this mesh data.
	size_t index_count() const noexcept
	{
		return _indices.size();
	}

	// Indices
	const std::vector<uint32_t>& indices() const
	{
		return _indices;
	}

	void push_back_index(uint32_t i);

	void push_back_indices(uint32_t i0, uint32_t i1, uint32_t i2);

	// Packs all the v vertex attribus that are specified in format.
	// Preserves the relative order of the attributes: positon, normal, tex_coord, tangent_h.
	void push_back_vertex(const Vertex_old& v);

	void push_back_vertices(const Vertex_old& v0, const Vertex_old& v1, const Vertex_old& v2);

	// How many vertices are in this mesh data.
	size_t vertex_count() const noexcept
	{
		return _data.size() / _format.component_count();
	}

private:
	std::vector<float> _data;
	std::vector<uint32_t> _indices;
	Interleaved_vertex_format_old _format;
};

//template<Vertex_attribs attribs>
//class Interleaved_mesh_data final {
//public:
//
//private:
//
//	std::vector<float> _vertex_data;
//	std::vector<uint32_t> _index_data;
//};

// Mesh_builder composes a mesh from the given vertices. Shared vertices are merged, 
// tangent & bitangent attribs for shared vertices are always accumulated to make them smooth.
class Mesh_builder final {
public:

	Mesh_builder(size_t vertex_count, size_t index_count);

	
	// Removes all the vertices & indices from builder.
	void clear() noexcept;

	// Total number of indices in the mesh.
	size_t index_count() const noexcept
	{
		return _indices.size();
	}

	// Returns indices of the mesh being constructed.
	const std::vector<uint32_t>& indices() const noexcept
	{
		return _indices;
	}

	void push_back_vertex(const Vertex_ts& vertex);

	void push_back_triangle(const Vertex_ts& v0, const Vertex_ts& v1, const Vertex_ts& v2);

	// Total number of vertices in the mesh.
	size_t vertex_count() const noexcept
	{
		return _vertices.size();
	}

	// Returns vertices of the mesh being constructed.
	const std::vector<Vertex_ts>& vertices() const noexcept
	{
		return _vertices;
	}

private:
	std::unordered_map<Vertex, uint32_t> _shared_vertices;
	std::vector<Vertex_ts> _vertices;
	std::vector<uint32_t> _indices;
};


inline bool operator==(const Interleaved_vertex_format_old& l, const Interleaved_vertex_format_old& r) noexcept
{
	return (l.attribs == r.attribs);
}

inline bool operator!=(const Interleaved_vertex_format_old& l, const Interleaved_vertex_format_old& r) noexcept
{
	return (l.attribs == r.attribs);
}

std::ostream& operator<<(std::ostream& out, const Interleaved_vertex_format_old& fmt);

std::wostream& operator<<(std::wostream& out, const Interleaved_vertex_format_old& fmt);


} // data
} // namespace cg

#endif // CG_DATA_MESH_H_
