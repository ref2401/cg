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
	tangent_space = 8,

	vertex_p = position,
	vertex_p_n = position | normal,
	vertex_p_n_tc = position | normal | tex_coord,
	vertex_p_tc = position | tex_coord,
	vertex_ts = position | normal | tex_coord | tangent_space
};

constexpr bool has_normal(Vertex_attribs attribs); // see defs & comments below
constexpr bool has_position(Vertex_attribs attribs);
constexpr bool has_tangent_space(Vertex_attribs attribs);
constexpr bool has_tex_coord(Vertex_attribs attribs);

// Describes the order and byte offset of the specified vertex attributes.
// The relative order of the attributes is: position, normal, tex_coord, tangent_h.
// Only the following vertex attribute compositions are supported:
// - Vertex_attribs::none
// - Vertex_attribs::vertex_p
// - Vertex_attribs::vertex_p_n
// - Vertex_attribs::vertex_p_n_tc
// - Vertex_attribs::vertex_p_tc
// - Vertex_attribs::vertex_ts
template<Vertex_attribs attribs>
struct Vertex_interleaved_format;

template<>
struct Vertex_interleaved_format<Vertex_attribs::none> final {
	constexpr static Vertex_attribs attribs = Vertex_attribs::none;
	constexpr static size_t vertex_component_count = 0;
	constexpr static size_t vertex_byte_count = 0;
};

template<>
struct Vertex_interleaved_format<Vertex_attribs::vertex_p> final {
	constexpr static Vertex_attribs attribs = Vertex_attribs::vertex_p;
	
	constexpr static size_t position_component_count = 3;
	constexpr static size_t position_byte_offset = 0;

	constexpr static size_t vertex_component_count = position_component_count;
	constexpr static size_t vertex_byte_count = sizeof(float) * vertex_component_count;
};

template<>
struct Vertex_interleaved_format<Vertex_attribs::vertex_p_n> final {
	constexpr static Vertex_attribs attribs = Vertex_attribs::vertex_p_n;

	constexpr static size_t position_component_count = 3;
	constexpr static size_t position_byte_offset = 0;

	constexpr static size_t normal_component_count = 3;
	constexpr static size_t normal_byte_offset = sizeof(float) * position_component_count;

	constexpr static size_t vertex_component_count = position_component_count + normal_component_count;
	constexpr static size_t vertex_byte_count = sizeof(float) * vertex_component_count;
};

template<>
struct Vertex_interleaved_format<Vertex_attribs::vertex_p_n_tc> final {
	constexpr static Vertex_attribs attribs = Vertex_attribs::vertex_p_n_tc;
	
	constexpr static size_t position_component_count = 3;
	constexpr static size_t position_byte_offset = 0;

	constexpr static size_t normal_component_count = 3;
	constexpr static size_t normal_byte_offset = sizeof(float) * position_component_count;

	constexpr static size_t tex_coord_component_count = 2;
	constexpr static size_t tex_coord_byte_offset = sizeof(float)
		* (position_component_count + normal_component_count);

	constexpr static size_t vertex_component_count = position_component_count 
		+ normal_component_count + tex_coord_component_count;
	constexpr static size_t vertex_byte_count = sizeof(float) * vertex_component_count;
};

template<>
struct Vertex_interleaved_format<Vertex_attribs::vertex_p_tc> final {
	constexpr static Vertex_attribs attribs = Vertex_attribs::vertex_p_tc;

	constexpr static size_t position_component_count = 3;
	constexpr static size_t position_byte_offset = 0;

	constexpr static size_t tex_coord_component_count = 2;
	constexpr static size_t tex_coord_byte_offset = sizeof(float) * position_component_count;

	constexpr static size_t vertex_component_count = position_component_count + tex_coord_component_count;
	constexpr static size_t vertex_byte_count = sizeof(float) * vertex_component_count;
};

template<>
struct Vertex_interleaved_format<Vertex_attribs::vertex_ts> final {
	constexpr static Vertex_attribs attribs = Vertex_attribs::vertex_ts;

	constexpr static size_t position_component_count = 3;
	constexpr static size_t position_byte_offset = 0;

	constexpr static size_t normal_component_count = 3;
	constexpr static size_t normal_byte_offset = sizeof(float) * position_component_count;

	constexpr static size_t tex_coord_component_count = 2;
	constexpr static size_t tex_coord_byte_offset = sizeof(float)
		* (position_component_count + normal_component_count);

	constexpr static size_t tangent_space_component_count = 4;
	constexpr static size_t tangent_space_byte_offset = sizeof(float)
		* (position_component_count + normal_component_count + tex_coord_component_count);

	constexpr static size_t vertex_component_count = position_component_count 
		+ normal_component_count + tex_coord_component_count + tangent_space_component_count;
	constexpr static size_t vertex_byte_count = sizeof(float) * vertex_component_count;
};

using Vertex_interleaved_format_p = Vertex_interleaved_format<Vertex_attribs::vertex_p>;
using Vertex_interleaved_format_p_n = Vertex_interleaved_format<Vertex_attribs::vertex_p_n>;
using Vertex_interleaved_format_p_n_tc = Vertex_interleaved_format<Vertex_attribs::vertex_p_n_tc>;
using Vertex_interleaved_format_p_tc = Vertex_interleaved_format<Vertex_attribs::vertex_p_tc>;
using Vertex_interleaved_format_ts = Vertex_interleaved_format<Vertex_attribs::vertex_ts>;

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
		&& (l.tangent_space == r.tangent_space);
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

// Computes tangent and handedness of bitangent for a triangle that is specified by the 3 given vertices.
// Assumes that all the normals are equal. Tangent_h components of each vertex are ignored.
// Returns: vector of 4 floats, xyz stands for the tangent & w stands for the handedness value.
float4 compute_tangent_h(const Vertex_old& v0, const Vertex_old& v1, const Vertex_old& v2);

// Computes tangent and bitangent vectors for a triangle that is specified by 3 position, tex_coord attribures.
// Returned vectors are not normalized.
std::pair<cg::float3, cg::float3> compute_tangent_bitangent(
	const cg::float3& pos0, const cg::float2& tc0,
	const cg::float3& pos1, const cg::float2& tc1,
	const cg::float3& pos2, const cg::float2& tc2) noexcept;

// Computes orthogonalized tangent and bitangent's handedness.
// All vectors have to be normalized.
// Returns: vector of 4 floats, xyz stands for the tangent & w stands for the handedness value.
cg::float4 compute_tangent_handedness(const cg::float3& tangent,
	const cg::float3& bitangent, const cg::float3& normal) noexcept;

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
	return (attribs & Vertex_attribs::tangent_space) == Vertex_attribs::tangent_space;
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

// Puts the given vertex into a float array pointed to by arr.
// The given array must have at least 12 elements.
// Components: position.xyz, normal.xyz, tex_coord.uv, tangent_h.xyzw.
// Tangent & handendess are compute by the function itself.
void to_array(const Vertex_ts& v, float* arr) noexcept;

// Puts position attibute of the given vertex into a float array pointed to by arr.
// The given array must have at least 3 elements. 
inline void to_array_p(const Vertex_ts& v, float* arr) noexcept
{
	arr[0] = v.position.x;
	arr[1] = v.position.y;
	arr[2] = v.position.z;
}

// Puts position & normal attibutes of the given vertex into a float array pointed to by arr.
// The given array must have at least 6 elements. 
inline void to_array_p_n(const Vertex_ts& v, float* arr) noexcept
{
	arr[0] = v.position.x;
	arr[1] = v.position.y;
	arr[2] = v.position.z;
	arr[3] = v.normal.x;
	arr[4] = v.normal.y;
	arr[5] = v.normal.z;
}

// Puts position, normal & tex_coord attibutes of the given vertex into a float array pointed to by arr.
// The given array must have at least 8 elements. 
inline void to_array_p_n_tc(const Vertex_ts& v, float* arr) noexcept
{
	arr[0] = v.position.x;
	arr[1] = v.position.y;
	arr[2] = v.position.z;
	arr[3] = v.normal.x;
	arr[4] = v.normal.y;
	arr[5] = v.normal.z;
	arr[6] = v.tex_coord.u;
	arr[7] = v.tex_coord.v;
}

// Puts position & tex_coord attibutes of the given vertex into a float array pointed to by arr.
// The given array must have at least 5 elements. 
inline void to_array_p_tc(const Vertex_ts& v, float* arr) noexcept
{
	arr[0] = v.position.x;
	arr[1] = v.position.y;
	arr[2] = v.position.z;
	arr[3] = v.tex_coord.u;
	arr[4] = v.tex_coord.v;
}

} // namespace data
} // namespace cg

namespace std {

template<> 
struct hash<cg::data::Vertex> {
	size_t operator()(const cg::data::Vertex& v) const noexcept
	{
		return cg::data::get_hash(v);
	}
};

} // namespace std

#endif // CG_DATA_VERTEX_H_
