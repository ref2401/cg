#ifndef CG_DATA_MESH_OLD_H_
#define CG_DATA_MESH_OLD_H_

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
#include "cg/data/file.h"


namespace cg {
namespace data {

enum class Vertex_attribs_old : unsigned char {
	none = 0,
	position = 1,
	normal = 2,
	tex_coord = 4,
	tangent_space = 8,

	vertex_p = position,
	vertex_p_n = position | normal,
	vertex_p_n_tc = position | normal | tex_coord,
	vertex_p_tc = position | tex_coord,
	vertex_ts = position | normal | tex_coord | tangent_space
};

constexpr bool has_normal(Vertex_attribs_old attribs); // see defs & comments below
constexpr bool has_position(Vertex_attribs_old attribs);
constexpr bool has_tangent_space(Vertex_attribs_old attribs);
constexpr bool has_tex_coord(Vertex_attribs_old attribs);

struct Vertex_old {
	Vertex_old() noexcept = default;

	explicit Vertex_old(float3 position) noexcept
		: Vertex_old(position, float3::zero, float2::zero, float4::zero)
	{}

	Vertex_old(float3 position, float3 normal) noexcept
		: Vertex_old(position, normal, float2::zero, float4::zero)
	{}

	Vertex_old(float3 position, float2 tex_coord) noexcept
		: Vertex_old(position, float3::zero, tex_coord, float4::zero)
	{}

	Vertex_old(float3 position, float3 normal, float2 tex_coord) noexcept
		: Vertex_old(position, normal, tex_coord, float4::zero)
	{}

	Vertex_old(float3 position, float3 normal, float2 tex_coord, float4 tangent_space) noexcept
		: position(position), normal(normal), tex_coord(tex_coord), tangent_space(tangent_space)
	{}


	float3 position = float3::zero;
	float3 normal = float3::zero;
	float2 tex_coord = float2::zero;
	float4 tangent_space = float4::zero;
};

// Describes the order and offset (byte & component) of every vertex attribute.
// The relative order of the attributes is: position, normal, tex_coord, tangent_h.
// All the offsets are calculated accordint to the defaul order.
//
// Each attribute has at least 1 component of some type (offen the type is float).
// Example:
// Interleaved_vertex_format(Vertex_attribs_old::position | Vertex_attribs_old::tex_coord)
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

	explicit Interleaved_vertex_format_old(Vertex_attribs_old attribs) noexcept
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


	Vertex_attribs_old attribs = Vertex_attribs_old::none;
};

// Interleaved_mesh_data_old is used to pack and store mesh data that is goint to be fed to the GPU.
// Implementation details: does not support base vertex counter 
// which means do not put more than one mesh into an Interleaved_mesh_data instance.
class Interleaved_mesh_data_old final {
public:

	Interleaved_mesh_data_old() noexcept = default;

	explicit Interleaved_mesh_data_old(Vertex_attribs_old attribs)
		: Interleaved_mesh_data_old(attribs, 0, 0)
	{}

	Interleaved_mesh_data_old(Vertex_attribs_old attribs, size_t vertex_count, size_t index_count);
	


	Vertex_attribs_old attribs() const
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


constexpr Vertex_attribs_old operator|(Vertex_attribs_old l, Vertex_attribs_old r)
{
	using Val_t = std::underlying_type<Vertex_attribs_old>::type;
	return static_cast<Vertex_attribs_old>(static_cast<Val_t>(l) | static_cast<Val_t>(r));
}

constexpr Vertex_attribs_old operator&(Vertex_attribs_old l, Vertex_attribs_old r)
{
	using Val_t = std::underlying_type<Vertex_attribs_old>::type;
	return static_cast<Vertex_attribs_old>(static_cast<Val_t>(l) & static_cast<Val_t>(r));
}

inline Vertex_attribs_old& operator|=(Vertex_attribs_old& l, Vertex_attribs_old r) noexcept
{
	l = l | r;
	return l;
}

inline Vertex_attribs_old& operator&=(Vertex_attribs_old& l, Vertex_attribs_old r) noexcept
{
	l = l & r;
	return l;
}

inline bool operator==(const Interleaved_vertex_format_old& l, const Interleaved_vertex_format_old& r) noexcept
{
	return (l.attribs == r.attribs);
}

inline bool operator!=(const Interleaved_vertex_format_old& l, const Interleaved_vertex_format_old& r) noexcept
{
	return (l.attribs == r.attribs);
}

inline bool operator==(const Vertex_old& l, const Vertex_old& r) noexcept
{
	return (l.position == r.position)
		&& (l.normal == r.normal)
		&& (l.tex_coord == r.tex_coord)
		&& (l.tangent_space == r.tangent_space);
}

inline bool operator!=(const Vertex_old& l, const Vertex_old& r) noexcept
{
	return !(l == r);
}

std::ostream& operator<<(std::ostream& out, const Interleaved_vertex_format_old& fmt);

std::wostream& operator<<(std::wostream& out, const Interleaved_vertex_format_old& fmt);

std::ostream& operator<<(std::ostream& out, const Vertex_attribs_old& attribs);

std::wostream& operator<<(std::wostream& out, const Vertex_attribs_old& attribs);

std::ostream& operator<<(std::ostream& out, const Vertex_old& v);

std::wostream& operator<<(std::wostream& out, const Vertex_old& v);

// Computes tangent and handedness of bitangent for a triangle that is specified by the 3 given vertices.
// Assumes that all the normals are equal. Tangent_h components of each vertex are ignored.
// Returns: vector of 4 floats, xyz stands for the tangent & w stands for the handedness value.
float4 compute_tangent_h(const Vertex_old& v0, const Vertex_old& v1, const Vertex_old& v2);

// Checks whether attribs contains Vertex_attribs_old::normal.
constexpr bool has_normal(Vertex_attribs_old attribs)
{
	return (attribs & Vertex_attribs_old::normal) == Vertex_attribs_old::normal;
}

// Checks whether attribs contains Vertex_attribs_old::position.
constexpr bool has_position(Vertex_attribs_old attribs)
{
	return (attribs & Vertex_attribs_old::position) == Vertex_attribs_old::position;
}

// Checks whether attribs contains Vertex_attribs_old::tangent_h.
constexpr bool has_tangent_space(Vertex_attribs_old attribs)
{
	return (attribs & Vertex_attribs_old::tangent_space) == Vertex_attribs_old::tangent_space;
}

// Checks whether attribs contains Vertex_attribs_old::tex_coord.
constexpr bool has_tex_coord(Vertex_attribs_old attribs)
{
	return (attribs & Vertex_attribs_old::tex_coord) == Vertex_attribs_old::tex_coord;
}

// Returns true if superset has all the vertex attributes that subset has, otherwise false.
constexpr bool is_superset_of(Vertex_attribs_old superset, Vertex_attribs_old subset)
{
	return (superset & subset) == subset;
}

Interleaved_mesh_data_old load_mesh_wavefront_old(const std::string& filename,
	Vertex_attribs_old attribs, size_t approx_vertex_count = 0, size_t approx_index_count = 0);

Interleaved_mesh_data_old load_mesh_wavefront_old(const char * filename,
	Vertex_attribs_old attribs, size_t approx_vertex_count = 0, size_t approx_index_count = 0);

} // data
} // namespace cg

#endif // CG_DATA_MESH_OLD_H_