#ifndef CG_DATA_MESH_H_
#define CG_DATA_MESH_H_

#include <cassert>
#include <cstdint>
#include <iterator>
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

// Interleaved_mesh_data_old is used to pack and store mesh data that is goint to be fed to the GPU.
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

// Interleaved_mesh_data is used to store mesh data that is goint to be fed to the GPU.
// Each instnace of the Interleaved_mesh_data class contains data of a single mesh.
template<Vertex_attribs attribs>
class Interleaved_mesh_data final {
public:

	static_assert(attribs != Vertex_attribs::none, "Vertex_attribs parameter may not be none.");

	using Format = Vertex_interleaved_format<attribs>;
	using Vertex_data_array = float[Format::vertex_component_count];


	Interleaved_mesh_data(size_t vertex_count, size_t index_count);

	Interleaved_mesh_data(const Interleaved_mesh_data<attribs>&) = delete;

	Interleaved_mesh_data(Interleaved_mesh_data<attribs>&& mesh_data) noexcept;


	Interleaved_mesh_data<attribs>& operator=(const Interleaved_mesh_data<attribs>&) = delete;

	Interleaved_mesh_data<attribs>& operator=(Interleaved_mesh_data<attribs>&& mesh_data) noexcept;


	// Returns a buffer that contains all the components specified by the format,
	// for every vertex beloning to the mesh.
	const std::vector<float>& vertex_data() const noexcept
	{
		return _vertex_data;
	}

	// Returns the index buffer
	const std::vector<uint32_t>& index_data() const noexcept
	{
		return _index_data;
	}

	// How many vertices are stored in this mesh data.
	size_t vertex_count() const noexcept
	{
		return _vertex_data.size() / Format::vertex_component_count;
	}

	// How many indices are in this mesh data.
	size_t index_count() const noexcept
	{
		return _index_data.size();
	}


	void push_back_vertex(const Vertex_data_array& arr);

	void push_back_index(uint32_t index);

	template<typename Container>
	void push_back_indices(const Container& container);

	template<size_t count>
	void push_back_indices(const uint32_t (&indices)[count]);

	template<typename Input_iterator>
	void push_back_indices(Input_iterator b, Input_iterator e);

private:
	std::vector<float> _vertex_data;
	std::vector<uint32_t> _index_data;
};

template<Vertex_attribs attribs>
Interleaved_mesh_data<attribs>::Interleaved_mesh_data(size_t vertex_count, size_t index_count)
{
	_vertex_data.reserve(Format::vertex_component_count * vertex_count);
	_index_data.reserve(index_count);
}

template<Vertex_attribs attribs>
Interleaved_mesh_data<attribs>::Interleaved_mesh_data(Interleaved_mesh_data<attribs>&& mesh_data) noexcept :
	_vertex_data(std::move(mesh_data._vertex_data)),
	_index_data(std::move(mesh_data._index_data))
{}

template<Vertex_attribs attribs>
Interleaved_mesh_data<attribs>& Interleaved_mesh_data<attribs>::operator=(Interleaved_mesh_data<attribs>&& mesh_data) noexcept
{
	if (this == &mesh_data) return *this;

	_vertex_data = std::move(mesh_data._vertex_data);
	_index_data = std::move(mesh_data._index_data);

	return *this;
}

template<Vertex_attribs attribs>
void Interleaved_mesh_data<attribs>::push_back_vertex(const Vertex_data_array& arr)
{
	_vertex_data.insert(_vertex_data.end(), std::cbegin(arr), std::cend(arr));
}

template<Vertex_attribs attribs>
void Interleaved_mesh_data<attribs>::push_back_index(uint32_t index)
{
	_index_data.push_back(index);
}

template<Vertex_attribs attribs>
template<typename Container>
void Interleaved_mesh_data<attribs>::push_back_indices(const Container& container)
{
	_index_data.insert(_index_data.end(), container.cbegin(), container.cend());
}

template<Vertex_attribs attribs>
template<size_t count>
void Interleaved_mesh_data<attribs>::push_back_indices(const uint32_t (&indices)[count])
{
	_index_data.insert(_index_data.end(), std::cbegin(indices), std::cend(indices));
}

template<Vertex_attribs attribs>
template<typename Input_iterator>
void Interleaved_mesh_data<attribs>::push_back_indices(Input_iterator b, Input_iterator e)
{
	_index_data.insert(_index_data.end(), b, e);
}

// Mesh_builder composes a mesh from the given vertices. Shared vertices are merged, 
// tangent & bitangent attribs for shared vertices are always accumulated to make them smooth.
class Mesh_builder final {
public:

	Mesh_builder(size_t vertex_count, size_t index_count);


	// Returns vertices of the mesh being constructed.
	const std::vector<Vertex_ts>& vertices() const noexcept
	{
		return _vertices;
	}

	// Returns indices of the mesh being constructed.
	const std::vector<uint32_t>& indices() const noexcept
	{
		return _indices;
	}

	// Total number of vertices in the mesh.
	size_t vertex_count() const noexcept
	{
		return _vertices.size();
	}

	// Total number of indices in the mesh.
	size_t index_count() const noexcept
	{
		return _indices.size();
	}

	// Removes all the vertices & indices from builder.
	void clear() noexcept;

	// Returns an object that contains indices and vertex data stored in interleaved format.
	template<Vertex_attribs attribs>
	Interleaved_mesh_data<attribs> mesh_data() const;

	void push_back_vertex(const Vertex_ts& vertex);

	void push_back_triangle(const Vertex_ts& v0, const Vertex_ts& v1, const Vertex_ts& v2);

private:
	std::unordered_map<Vertex, uint32_t> _shared_vertices;
	std::vector<Vertex_ts> _vertices;
	std::vector<uint32_t> _indices;
};

template<Vertex_attribs attribs>
Interleaved_mesh_data<attribs> Mesh_builder::mesh_data() const
{
	Interleaved_mesh_data<attribs> md(vertex_count(), index_count());
	Interleaved_mesh_data<attribs>::Vertex_data_array vert_arr;

	for (const auto& v : _vertices) {
		to_array<attribs>(v, vert_arr);
		md.push_back_vertex(vert_arr);
	}

	md.push_back_indices(_indices);

	return md;
}


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
