#ifndef CG_DATA_MESH_H_
#define CG_DATA_MESH_H_

#include <cassert>
#include <ostream>
#include <type_traits>
#include "cg/base/base.h"
#include "cg/math/math.h"


namespace cg {
namespace data {

enum class Vertex_attribs : unsigned char {
	position = 1,
	normal = 2,
	tex_coord = 4,
	tangent_h = 8,

	mesh_position = position,
	mesh_textured = position | tex_coord,
	mesh_tangent_h = position | tex_coord | normal | tangent_h
};

constexpr bool has_normal(Vertex_attribs attribs); // see defs & comments below
constexpr bool has_position(Vertex_attribs attribs);
constexpr bool has_tangent_h(Vertex_attribs attribs);
constexpr bool has_tex_coord(Vertex_attribs attribs);


// Describes the order and offset (byte & component) of every vertex attribute.
// The default order of the attributes is: position, normal, tex_coord, tangent_h.
// All the offsets are calculated accordint to the defaul order.
//
// Each attribute has at least 1 component of some type (offen the type is float).
// Example:
// Interleaved_vertex_format(Vertex_attribs::position | Vertex_attribs::tex_coord)
// position has 3 components: x, y, z; tex_coord has 2 components: u, v.
// total component count = 3 + 2 = 5;
// total byte count = 5 * sizeof(float).
// position's offset:	0 components, 0 bytes (position is first in the default order).
// tex_coord's offset:	3 components, 3 * sizeof(float) bytes (tex_coord goes right after the position).
struct Interleaved_vertex_format {
	static constexpr size_t component_count_normal = 3;
	static constexpr size_t component_count_position = 3;
	static constexpr size_t component_count_tangent_h = 4;
	static constexpr size_t component_count_tex_coord = 2;


	explicit Interleaved_vertex_format(Vertex_attribs attribs) noexcept 
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
		if (has_tangent_h(attribs)) counter += component_count_tangent_h;

		return counter;
	}

	// How many components are there before normal data starts.
	size_t component_offset_normal() const noexcept
	{
		size_t prior_cmpt_count = has_position(attribs) 
			? (Interleaved_vertex_format::component_count_position) 
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
			? (Interleaved_vertex_format::component_count_tex_coord) 
			: 0u;
		return component_offset_tex_coord() + prior_cmpt_count;
	}

	// How many components are there before tex_coord data starts.
	size_t component_offset_tex_coord() const noexcept
	{
		size_t prior_cmpt_count = has_normal(attribs) 
			? (Interleaved_vertex_format::component_count_normal) 
			: 0u;
		return component_offset_normal() + prior_cmpt_count;
	}


	Vertex_attribs attribs;
};

struct Vertex {
	Vertex() noexcept = default;

	explicit Vertex(float3 position) noexcept
		: Vertex(position, float3::zero, float2::zero, float4::zero)
	{}

	Vertex(float3 position, float3 normal) noexcept
		: Vertex(position, normal, float2::zero, float4::zero)
	{}

	Vertex(float3 position, float2 tex_coord) noexcept
		: Vertex(position, float3::zero, tex_coord, float4::zero)
	{}

	Vertex(float3 position, float3 normal, float2 tex_coord) noexcept
		: Vertex(position, normal, tex_coord, float4::zero)
	{}

	Vertex(float3 position, float3 normal, float2 tex_coord, float4 tangent_h) noexcept
		: position(position), normal(normal), tex_coord(tex_coord), tangent_h(tangent_h)
	{}


	float3 position;
	float3 normal;
	float2 tex_coord;
	float4 tangent_h;
};


constexpr Vertex_attribs operator|(Vertex_attribs l, Vertex_attribs r)
{
	using Val_t = std::underlying_type<Vertex_attribs>::type;
	return static_cast<Vertex_attribs>(static_cast<Val_t>(l) | static_cast<Val_t>(r));
}

constexpr Vertex_attribs operator&(Vertex_attribs l, Vertex_attribs r)
{
	using Val_t = std::underlying_type<Vertex_attribs>::type;
	return static_cast<Vertex_attribs>(static_cast<Val_t>(l) & static_cast<Val_t>(r));
}

inline bool operator==(const Interleaved_vertex_format& l, const Interleaved_vertex_format& r) noexcept
{
	return (l.attribs == r.attribs);
}

inline bool operator!=(const Interleaved_vertex_format& l, const Interleaved_vertex_format& r) noexcept
{
	return (l.attribs == r.attribs);
}

inline bool operator==(const Vertex& l, const Vertex& r) noexcept
{
	return (l.position == r.position)
		&& (l.normal == r.normal)
		&& (l.tex_coord == r.tex_coord)
		&& (l.tangent_h == r.tangent_h);
}

inline bool operator!=(const Vertex& l, const Vertex& r) noexcept
{
	return !(l == r);
}

inline std::ostream& operator<<(std::ostream& out, const Vertex_attribs& attrib)
{
	switch (attrib) {
		case Vertex_attribs::position:			out << "Vertex_attribs::position"; break;
		case Vertex_attribs::normal:			out << "Vertex_attribs::normal"; break;
		case Vertex_attribs::tex_coord:			out << "Vertex_attribs::tex_coord"; break;
		case Vertex_attribs::tangent_h:			out << "Vertex_attribs::tangent_h"; break;
			// case Vertex_attribs::mesh_position: is skipped 
		case Vertex_attribs::mesh_textured:		out << "Vertex_attribs::mesh_textured"; break;
		case Vertex_attribs::mesh_tangent_h:	out << "Vertex_attribs::mesh_tangent_h"; break;
	}

	return out;
}

inline std::wostream& operator<<(std::wostream& out, const Vertex_attribs& attrib)
{
	switch (attrib) {
		case Vertex_attribs::position:			out << "Vertex_attribs::position"; break;
		case Vertex_attribs::normal:			out << "Vertex_attribs::normal"; break;
		case Vertex_attribs::tex_coord:			out << "Vertex_attribs::tex_coord"; break;
		case Vertex_attribs::tangent_h:			out << "Vertex_attribs::tangent_h"; break;
			// case Vertex_attribs::mesh_position: is skipped 
		case Vertex_attribs::mesh_textured:		out << "Vertex_attribs::mesh_textured"; break;
		case Vertex_attribs::mesh_tangent_h:	out << "Vertex_attribs::mesh_tangent_h"; break;
	}

	return out;
}

inline std::ostream& operator<<(std::ostream& out, const Interleaved_vertex_format& fmt)
{
	out << fmt.attribs;
	return out;
}

inline std::wostream& operator<<(std::wostream& out, const Interleaved_vertex_format& fmt)
{
	out << fmt.attribs;
	return out;
}

inline std::ostream& operator<<(std::ostream& out, const Vertex& v)
{
	out << "Vertex(p: " << v.position
		<< "; n: " << v.normal
		<< "; tc: " << v.tex_coord
		<< "; tan_h: " << v.tangent_h << ")";
	return out;
}

inline std::wostream& operator<<(std::wostream& out, const Vertex& v)
{
	out << "Vertex(p: " << v.position
		<< "; n: " << v.normal
		<< "; tc: " << v.tex_coord
		<< "; tan_h: " << v.tangent_h << ")";
	return out;
}

// Computes tangent and handedness of bitangent for a trinaglel that is specified by the 3 given vertices.
// Assumes that all the normals are equal. Tangent_h components of each vertex are ignored.
// Returns: vector of 4 floats, xyz stands for the tangent & w stands for the handedness value.
float4 compute_tangent_h(const Vertex& v0, const Vertex& v1, const Vertex& v2);

// Checks whether attribs contains Vertex_attribs::normal.
constexpr bool has_normal(Vertex_attribs attribs)
{
	return (attribs & Vertex_attribs::normal) == Vertex_attribs::normal;
}

// Checks whether attribs contains Vertex_attribs::position.
constexpr bool has_position(Vertex_attribs attribs)
{
	return (attribs & Vertex_attribs::position) == Vertex_attribs::position;
}

// Checks whether attribs contains Vertex_attribs::tangent_h.
constexpr bool has_tangent_h(Vertex_attribs attribs)
{
	return (attribs & Vertex_attribs::tangent_h) == Vertex_attribs::tangent_h;
}

// Checks whether attribs contains Vertex_attribs::tex_coord.
constexpr bool has_tex_coord(Vertex_attribs attribs)
{
	return (attribs & Vertex_attribs::tex_coord) == Vertex_attribs::tex_coord;
}

// Returns true if superset has all the vertex attributes that subset has, otherwise false.
constexpr bool is_superset_of(Vertex_attribs superset, Vertex_attribs subset)
{
	return (superset & subset) == subset;
}


} // data
} // namespace cg

#endif // CG_DATA_MESH_H_
