#ifndef CG_DATA_MESH_H_
#define CG_DATA_MESH_H_

#include <cassert>
#include <type_traits>


namespace cg {
namespace data {

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
