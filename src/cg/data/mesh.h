#ifndef CG_DATA_MESH_H_
#define CG_DATA_MESH_H_

#include <cassert>
#include <ostream>
#include <type_traits>
#include "cg/base/base.h"
#include "cg/math/math.h"


namespace cg {
namespace data {


struct Vertex {
	Vertex() noexcept = default;

	explicit Vertex(float3 position) noexcept;

	Vertex(float3 position, float3 normal) noexcept;

	Vertex(float3 position, float2 tex_coord) noexcept;

	Vertex(float3 position, float3 normal, float2 tex_coord) noexcept;

	Vertex(float3 position, float3 normal, float2 tex_coord, float4 tangent_h) noexcept;


	float3 position;
	float3 normal;
	float2 tex_coord;
	float4 tangent_h;
};

bool operator==(const Vertex& l, const Vertex& r) noexcept;

bool operator!=(const Vertex& l, const Vertex& r) noexcept;

std::ostream& operator<<(std::ostream& out, const Vertex& v);

std::wostream& operator<<(std::wostream& out, const Vertex& v);


enum class Vertex_attribs : unsigned char {
	position = 1,
	tex_coord = 2,
	normal = 4,
	tangent_h = 8,

	mesh_position = position,
	mesh_textured = position | tex_coord,
	mesh_tangent_h = position | tex_coord | normal | tangent_h
};

constexpr Vertex_attribs operator|(Vertex_attribs l, Vertex_attribs r);

constexpr Vertex_attribs operator&(Vertex_attribs l, Vertex_attribs r);

// Checks whether attribs contains Vertex_attribs::position.
constexpr bool has_position(Vertex_attribs attribs);

// Checks whether attribs contains Vertex_attribs::tex_coord.
constexpr bool has_tex_coord(Vertex_attribs attribs);

// Checks whether attribs contains Vertex_attribs::normal.
constexpr bool has_normal(Vertex_attribs attribs);

// Checks whether attribs contains Vertex_attribs::tangent_h.
constexpr bool has_tangent_h(Vertex_attribs attribs);

// Returns true if superset has all the vertex attributes that subset has, otherwise false.
constexpr bool is_superset_of(Vertex_attribs superset, Vertex_attribs subset);


// Computes tangent and handedness of bitangent for a trinagnel 
// that is specified by the 3 given vertices.
// Assumes that all the normals are equal.
// Returns: vector of 4 floats, xyz stands for the tangent & w stands for the handedness value.
float4 compute_tanget_h(const Vertex& v0, const Vertex& v1, const Vertex& v2);


// ----- Vertex -----

inline Vertex::Vertex(float3 position) noexcept
	: Vertex(position, float3::zero, float2::zero, float4::zero)
{}

inline Vertex::Vertex(float3 position, float3 normal) noexcept
	: Vertex(position, normal, float2::zero, float4::zero)
{}

inline Vertex::Vertex(float3 position, float2 tex_coord) noexcept
	: Vertex(position, float3::zero, tex_coord, float4::zero)
{}

inline Vertex::Vertex(float3 position, float3 normal, float2 tex_coord) noexcept
	: Vertex(position, normal, tex_coord, float4::zero)
{}

inline Vertex::Vertex(float3 position, float3 normal, float2 tex_coord, float4 tangent_h) noexcept
	: position(position), normal(normal), tex_coord(tex_coord), tangent_h(tangent_h)
{}

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

// ----- Vertex_attribs -----

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

constexpr bool has_position(Vertex_attribs attribs)
{
	return (attribs & Vertex_attribs::position) == Vertex_attribs::position;
}

constexpr bool has_tex_coord(Vertex_attribs attribs)
{
	return (attribs & Vertex_attribs::tex_coord) == Vertex_attribs::tex_coord;
}

constexpr bool has_normal(Vertex_attribs attribs)
{
	return (attribs & Vertex_attribs::normal) == Vertex_attribs::normal;
}

constexpr bool has_tangent_h(Vertex_attribs attribs)
{
	return (attribs & Vertex_attribs::tangent_h) == Vertex_attribs::tangent_h;
}

constexpr bool is_superset_of(Vertex_attribs superset, Vertex_attribs subset)
{
	return (superset & subset) == subset;
}


} // data
} // namespace cg

#endif // CG_DATA_MESH_H_
