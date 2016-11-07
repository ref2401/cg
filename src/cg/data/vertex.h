#ifndef CG_DATA_VERTEX_H_
#define CG_DATA_VERTEX_H_

#include <cassert>
#include <ostream>
#include "cg/base/base.h"
#include "cg/math/math.h"


namespace cg {
namespace data {


enum class Vertex_attribs : unsigned char {
	none = 0,
	position = 1,
	normal = 2,
	tex_coord = 4,
	tangent_h = 8,

	mesh_position = position,
	mesh_textured = position | tex_coord,
	mesh_tangent_space = position | tex_coord | normal | tangent_h
};

constexpr bool has_normal(Vertex_attribs attribs); // see defs & comments below
constexpr bool has_position(Vertex_attribs attribs);
constexpr bool has_tangent_space(Vertex_attribs attribs);
constexpr bool has_tex_coord(Vertex_attribs attribs);

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

	Vertex_old(float3 position, float3 normal, float2 tex_coord, float4 tangent_h) noexcept
		: position(position), normal(normal), tex_coord(tex_coord), tangent_h(tangent_h)
	{}


	float3 position = float3::zero;
	float3 normal = float3::zero;
	float2 tex_coord = float2::zero;
	float4 tangent_h = float4::zero;
};

// Vertex that contains position, normal and tex_coord attributes.
struct Vertex final {
	Vertex() noexcept = default;

	explicit Vertex(const float3& position) noexcept :
		position(position)
	{}

	Vertex(const float3& position, const float3& normal) noexcept :
		position(position), normal(normal)
	{}

	Vertex(const float3& position, const float2& tex_coord) noexcept :
		position(position), tex_coord(tex_coord)
	{}

	Vertex(const float3& position, const float3& normal, const float2& tex_coord) noexcept :
		position(position), normal(normal), tex_coord(tex_coord)
	{}


	float3 position;
	float3 normal;
	float2 tex_coord;
};

// Vertex that contains position, normal, tex_coord, tangent & bitangent attributes.
struct Vertex_ts final {
	Vertex_ts() noexcept = default;

	explicit Vertex_ts(const float3& position) noexcept :
		position(position)
	{}

	Vertex_ts(const float3& position, const float3& normal) noexcept :
		position(position), normal(normal)
	{}

	Vertex_ts(const float3& position, const float2& tex_coord) noexcept :
		position(position), tex_coord(tex_coord)
	{}

	Vertex_ts(const float3& position, const float3& normal, const float2& tex_coord) noexcept :
		position(position), normal(normal), tex_coord(tex_coord)
	{}

	Vertex_ts(const float3& position, const float3& normal, const float2& tex_coord, 
		const float3& tangent, const float3& bitangent) noexcept :
		position(position), normal(normal),
		tangent(tangent), bitangent(bitangent), tex_coord(tex_coord)
	{}


	float3 position = float3::zero;
	float3 normal = float3::zero;
	float3 tangent = float3::zero;
	float3 bitangent = float3::zero;
	float2 tex_coord = float2::zero;
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

inline Vertex_attribs& operator|=(Vertex_attribs& l, Vertex_attribs r) noexcept
{
	l = l | r;
	return l;
}

inline Vertex_attribs& operator&=(Vertex_attribs& l, Vertex_attribs r) noexcept
{
	l = l & r;
	return l;
}

inline bool operator==(const Vertex_old& l, const Vertex_old& r) noexcept
{
	return (l.position == r.position)
		&& (l.normal == r.normal)
		&& (l.tex_coord == r.tex_coord)
		&& (l.tangent_h == r.tangent_h);
}

inline bool operator!=(const Vertex_old& l, const Vertex_old& r) noexcept
{
	return !(l == r);
}

inline bool operator==(const Vertex& l, const Vertex& r) noexcept
{
	return (l.position == r.position)
		&& (l.normal == r.normal)
		&& (l.tex_coord == r.tex_coord);
}

inline bool operator!=(const Vertex& l, const Vertex& r) noexcept
{
	return !(l == r);
}

inline bool operator==(const Vertex_ts& l, const Vertex_ts& r) noexcept
{
	return (l.position == r.position)
		&& (l.normal == r.normal)
		&& (l.tex_coord == r.tex_coord)
		&& (l.tangent == r.tangent)
		&& (l.bitangent == r.bitangent);
}

inline bool operator!=(const Vertex_ts& l, const Vertex_ts& r) noexcept
{
	return !(l == r);
}

std::ostream& operator<<(std::ostream& out, const Vertex_attribs& attribs);

std::wostream& operator<<(std::wostream& out, const Vertex_attribs& attribs);

std::ostream& operator<<(std::ostream& out, const Vertex_old& v);

std::wostream& operator<<(std::wostream& out, const Vertex_old& v);

std::ostream& operator<<(std::ostream& out, const Vertex& v);

std::wostream& operator<<(std::wostream& out, const Vertex& v);

std::ostream& operator<<(std::ostream& out, const Vertex_ts& v);

std::wostream& operator<<(std::wostream& out, const Vertex_ts& v);

// Calculates hash value for the given vertex.
size_t get_hash(const Vertex& v) noexcept;

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
constexpr bool has_tangent_space(Vertex_attribs attribs)
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

} // namespace data
} // namespace cg


#endif // CG_DATA_VERTEX_H_
