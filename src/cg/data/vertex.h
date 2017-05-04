#ifndef CG_DATA_VERTEX_H_
#define CG_DATA_VERTEX_H_

#include <cassert>
#include <ostream>
#include "cg/base/base.h"
#include "cg/base/math.h"


namespace cg {
namespace data {

// All possible combinations of vertex attributes.
// p	- position;
// n	- normal;
// tc	- texture coordinates;
// ts	- tangent space (tangent & bitangent or tangnet & handedness);
enum class vertex_attribs : unsigned char {
	p = 0,
	p_n,
	p_n_tc,
	p_tc,
	p_n_tc_ts
};

// Describes the order and byte offset of the specified vertex attributes.
// The relative order of the attributes is: position, normal, tex_coord, tangent_h.
template<vertex_attribs attribs>
struct Vertex_interleaved_format;

using Vertex_interleaved_format_all = Vertex_interleaved_format<vertex_attribs::p_n_tc_ts>;

template<>
struct Vertex_interleaved_format<vertex_attribs::p_n_tc_ts> {
	static constexpr vertex_attribs attribs = vertex_attribs::p_n_tc_ts;

	static constexpr size_t position_component_count = 3;
	static constexpr size_t position_byte_count = sizeof(float) * position_component_count;
	static constexpr size_t position_byte_offset = 0;

	static constexpr size_t normal_component_count = 3;
	static constexpr size_t normal_byte_count = sizeof(float) * normal_component_count;
	static constexpr size_t normal_byte_offset = position_byte_offset + position_byte_count;

	static constexpr size_t tex_coord_component_count = 2;
	static constexpr size_t tex_coord_byte_count = sizeof(float) * tex_coord_component_count;
	static constexpr size_t tex_coord_byte_offset = normal_byte_offset + normal_byte_count;

	static constexpr size_t tangent_space_component_count = 4;
	static constexpr size_t tangent_space_byte_count = sizeof(float) * tangent_space_component_count;
	static constexpr size_t tangent_space_byte_offset = tex_coord_byte_offset + tex_coord_byte_count;

	static constexpr size_t vertex_component_count = position_component_count
		+ normal_component_count + tex_coord_component_count + tangent_space_component_count;
	static constexpr size_t vertex_byte_count = position_byte_count
		+ normal_byte_count + tex_coord_byte_count + tangent_space_byte_count;
};

template<>
struct Vertex_interleaved_format<vertex_attribs::p> {
	static constexpr vertex_attribs attribs = vertex_attribs::p;

	static constexpr size_t position_component_count = Vertex_interleaved_format_all::position_component_count;
	static constexpr size_t position_byte_count = Vertex_interleaved_format_all::position_byte_count;
	static constexpr size_t position_byte_offset = 0;

	static constexpr size_t vertex_component_count = position_component_count;
	static constexpr size_t vertex_byte_count = position_byte_count;
};

template<>
struct Vertex_interleaved_format<vertex_attribs::p_n> {
	static constexpr vertex_attribs attribs = vertex_attribs::p_n;

	static constexpr size_t position_component_count = Vertex_interleaved_format_all::position_component_count;
	static constexpr size_t position_byte_count = Vertex_interleaved_format_all::position_byte_count;
	static constexpr size_t position_byte_offset = 0;

	static constexpr size_t normal_component_count = Vertex_interleaved_format_all::normal_component_count;
	static constexpr size_t normal_byte_count = Vertex_interleaved_format_all::normal_byte_count;
	static constexpr size_t normal_byte_offset = position_byte_offset + position_byte_count;

	static constexpr size_t vertex_component_count = position_component_count
		+ normal_component_count;
	static constexpr size_t vertex_byte_count = position_byte_count
		+ normal_byte_count;
};

template<>
struct Vertex_interleaved_format<vertex_attribs::p_n_tc> {
	static constexpr vertex_attribs attribs = vertex_attribs::p_n_tc;

	static constexpr size_t position_component_count = Vertex_interleaved_format_all::position_component_count;
	static constexpr size_t position_byte_count = Vertex_interleaved_format_all::position_byte_count;
	static constexpr size_t position_byte_offset = 0;

	static constexpr size_t normal_component_count = Vertex_interleaved_format_all::normal_component_count;
	static constexpr size_t normal_byte_count = Vertex_interleaved_format_all::normal_byte_count;
	static constexpr size_t normal_byte_offset = position_byte_offset + position_byte_count;

	static constexpr size_t tex_coord_component_count = Vertex_interleaved_format_all::tex_coord_component_count;
	static constexpr size_t tex_coord_byte_count = Vertex_interleaved_format_all::tex_coord_byte_count;
	static constexpr size_t tex_coord_byte_offset = normal_byte_offset + normal_byte_count;

	static constexpr size_t vertex_component_count = position_component_count
		+ normal_component_count + tex_coord_component_count;
	static constexpr size_t vertex_byte_count = position_byte_count
		+ normal_byte_count + tex_coord_byte_count;
};

template<>
struct Vertex_interleaved_format<vertex_attribs::p_tc> {
	static constexpr vertex_attribs attribs = vertex_attribs::p_tc;

	static constexpr size_t position_component_count = Vertex_interleaved_format_all::position_component_count;
	static constexpr size_t position_byte_count = Vertex_interleaved_format_all::position_byte_count;
	static constexpr size_t position_byte_offset = 0;

	static constexpr size_t tex_coord_component_count = Vertex_interleaved_format_all::tex_coord_component_count;
	static constexpr size_t tex_coord_byte_count = Vertex_interleaved_format_all::tex_coord_byte_count;
	static constexpr size_t tex_coord_byte_offset = position_byte_offset + position_byte_count;

	static constexpr size_t vertex_component_count = position_component_count
		+ tex_coord_component_count;
	static constexpr size_t vertex_byte_count = position_byte_count
		+ tex_coord_byte_count;
};

struct Vertex_interleaved_format_desc final {
	Vertex_interleaved_format_desc() noexcept = default;

	Vertex_interleaved_format_desc(vertex_attribs attribs) noexcept;

	vertex_attribs attribs = vertex_attribs::p;
	size_t position_component_count = 0;
	size_t position_byte_count = 0;
	size_t position_byte_offset = 0;
	size_t normal_component_count = 0;
	size_t normal_byte_count = 0;
	size_t normal_byte_offset = 0;
	size_t tex_coord_component_count = 0;
	size_t tex_coord_byte_count = 0;
	size_t tex_coord_byte_offset = 0;
	size_t tangent_space_component_count = 0;
	size_t tangent_space_byte_count = 0;
	size_t tangent_space_byte_offset = 0;
	size_t vertex_component_count = 0;
	size_t vertex_byte_count = 0;
};

std::ostream& operator<<(std::ostream& out, const vertex_attribs& attribs);

std::wostream& operator<<(std::wostream& out, const vertex_attribs& attribs);

// Computes tangent and bitangent vectors for a triangle that is specified by 3 position, tex_coord attribures.
// Returned vectors are not normalized.
std::pair<float3, float3> compute_tangent_bitangent(
	const float3& pos0, const float2& tc0,
	const float3& pos1, const float2& tc1,
	const float3& pos2, const float2& tc2) noexcept;

// Computes orthogonalized tangent and bitangent's handedness.
// All vectors have to be normalized.
// Returns: vector of 4 floats, xyz stands for the tangent & w stands for the handedness value.
float4 compute_tangent_handedness(const float3& tangent,
	const float3& bitangent, const float3& normal) noexcept;

constexpr bool has_normal(vertex_attribs attribs) noexcept
{
	return !(attribs == vertex_attribs::p || attribs == vertex_attribs::p_tc);
}

constexpr bool has_tangent_space(vertex_attribs attribs) noexcept
{
	return attribs == vertex_attribs::p_n_tc_ts;
}

constexpr bool has_tex_coord(vertex_attribs attribs) noexcept
{
	return !(attribs == vertex_attribs::p || attribs == vertex_attribs::p_n);
}

bool is_superset_of(vertex_attribs superset, vertex_attribs subset) noexcept;

} // namespace data
} // namespace cg

#endif // CG_DATA_VERTEX_H_
