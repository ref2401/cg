#include "cg/file/file_mesh.h"

#include <cstdint>
#include <array>
#include <limits>
#include <utility>
#include <vector>
#include "cg/math/math.h"


namespace {

using cg::float2;
using cg::float3;
using cg::float4;
using cg::data::Interleaved_mesh_data;
using cg::data::Interleaved_mesh_data_old;
using cg::data::Mesh_builder;
using cg::data::Vertex_old;
using cg::data::Vertex_attribs;
using cg::data::Vertex_ts;
using cg::data::compute_tangent_bitangent;
using cg::data::compute_tangent_h;
using cg::file::By_line_iterator;
using cg::file::internal::Wf_mesh_data;


// Describes the type of wavefront data that is serialized as one line of characters.
// Different parse funcs are used according to the line type.
enum class Wf_line_type : unsigned char {
	// Includes position, normal and tex_coord indices
	face,

	// Vector of 3 floats
	normal,

	// The content of the line negligible. 
	other,

	// Vector of 3 floats
	position,

	// Vector of 2 floats
	tex_coord
};

// ----- funcs -----

// Wavefront spec: If an index is positive then it refers to the offset in that vertex list, starting at 1.
// In this case normalized index is wavefront index - 1.
// If an index is negative then it relatively refers to the end of the vertex list, -1 referring to the last element.
// In this case normalized index is max_count - wavefront index.
inline void normalize_wf_indices(long long max_count, long long& i0, long long& i1, long long& i2) noexcept
{
	i0 = (i0 > 0) ? (i0 - 1) : (max_count + i0);
	i1 = (i1 > 0) ? (i1 - 1) : (max_count + i1);
	i2 = (i2 > 0) ? (i2 - 1) : (max_count + i2);

	assert(0 <= i0 && i0 <= std::numeric_limits<size_t>::max());
	assert(0 <= i1 && i1 <= std::numeric_limits<size_t>::max());
	assert(0 <= i2 && i2 <= std::numeric_limits<size_t>::max());
}

// Returns the type of wavefront data in the given line.
Wf_line_type parse_line_type(const std::string& line)
{
	// possible cases: f, v, vn, vt

	if (line[0] == 'f') return Wf_line_type::face;
	if (line[0] != 'v' || line.size() == 1) return Wf_line_type::other;

	switch (line[1]) {
	case ' ': return Wf_line_type::position;
	case 'n': return Wf_line_type::normal;
	case 't': return Wf_line_type::tex_coord;
	default: return Wf_line_type::other;
	}
}

#pragma warning(push)
#pragma warning(disable:4996)

// Parses wavefront face definitions which contain only vertex position (example: f 1 2 3)
// Constructs vertices and put them into the Mesh_builder obejct.
void parse_faces_p(By_line_iterator& it, Wf_mesh_data& wf_mesh_data, Mesh_builder& mesh_builder)
{
	long long position_count = wf_mesh_data.positions.size();

	for (; it != By_line_iterator::end; ++it) {
		auto& line = *it;
		auto line_type = parse_line_type(line);
		if (line_type != Wf_line_type::face) break;

		long long p0, p1, p2;
		int count = sscanf(line.c_str(), "f %lld %lld %lld", &p0, &p1, &p2);
		ENFORCE(count == 3u, "Invalid face format. Only position indices were expected.");
		normalize_wf_indices(position_count, p0, p1, p2);

		Vertex_ts vertices[3];
		vertices[0].position = wf_mesh_data.positions[static_cast<size_t>(p0)];
		vertices[1].position = wf_mesh_data.positions[static_cast<size_t>(p1)];
		vertices[2].position = wf_mesh_data.positions[static_cast<size_t>(p2)];
		mesh_builder.push_back_triangle(vertices[0], vertices[1], vertices[2]);
	}
}

// Parses wavefront face definitions which contain vertex position//normal (example: f 1//1 2//1 3//1)
// Constructs vertices and put them into the Mesh_builder obejct.
void parse_face_pn(By_line_iterator& it, Wf_mesh_data& wf_mesh_data, Mesh_builder& mesh_builder)
{
	long long position_count = wf_mesh_data.positions.size();
	long long normal_count = wf_mesh_data.normals.size();

	for (; it != By_line_iterator::end; ++it) {
		auto& line = *it;
		auto line_type = parse_line_type(line);
		if (line_type != Wf_line_type::face) break;

		long long p0, p1, p2, n0, n1, n2;
		int count = sscanf(line.c_str(), "f %lld//%lld %lld//%lld %lld//%lld", &p0, &n0, &p1, &n1, &p2, &n2);
		ENFORCE(count == 6u, "Invalid face format. Position and normal indices were expected.");
		normalize_wf_indices(position_count, p0, p1, p2);
		normalize_wf_indices(normal_count, n0, n1, n2);

		Vertex_ts vertices[3];
		vertices[0].position = wf_mesh_data.positions[static_cast<size_t>(p0)];
		vertices[1].position = wf_mesh_data.positions[static_cast<size_t>(p1)];
		vertices[2].position = wf_mesh_data.positions[static_cast<size_t>(p2)];
		vertices[0].normal = wf_mesh_data.normals[static_cast<size_t>(n0)];
		vertices[1].normal = wf_mesh_data.normals[static_cast<size_t>(n1)];
		vertices[2].normal = wf_mesh_data.normals[static_cast<size_t>(n2)];
		mesh_builder.push_back_triangle(vertices[0], vertices[1], vertices[2]);
	}
}

// Parses wavefront face definitions which contain vertex position/tex_coord (example: f 1/1 2/2 3/3)
// Constructs vertices and put them into the Mesh_builder obejct.
void parse_face_ptc(By_line_iterator& it, Wf_mesh_data& wf_mesh_data, Mesh_builder& mesh_builder)
{
	long long position_count = wf_mesh_data.positions.size();
	long long tex_coord_count = wf_mesh_data.tex_coords.size();

	for (; it != By_line_iterator::end; ++it) {
		auto& line = *it;
		auto line_type = parse_line_type(line);
		if (line_type != Wf_line_type::face) break;

		long long p0, p1, p2, tc0, tc1, tc2;
		int count = sscanf(line.c_str(), "f %lld/%lld %lld/%lld %lld/%lld", &p0, &tc0, &p1, &tc1, &p2, &tc2);
		ENFORCE(count == 6u, "Invalid face format. Position and tex_coord indices were expected.");
		normalize_wf_indices(position_count, p0, p1, p2);
		normalize_wf_indices(tex_coord_count, tc0, tc1, tc2);

		Vertex_ts vertices[3];
		vertices[0].position = wf_mesh_data.positions[static_cast<size_t>(p0)];
		vertices[1].position = wf_mesh_data.positions[static_cast<size_t>(p1)];
		vertices[2].position = wf_mesh_data.positions[static_cast<size_t>(p2)];
		vertices[0].tex_coord = wf_mesh_data.tex_coords[static_cast<size_t>(tc0)];
		vertices[1].tex_coord = wf_mesh_data.tex_coords[static_cast<size_t>(tc1)];
		vertices[2].tex_coord = wf_mesh_data.tex_coords[static_cast<size_t>(tc2)];
		mesh_builder.push_back_triangle(vertices[0], vertices[1], vertices[2]);
	}
}

// Parses wavefront face definitions which contain vertex position/tex_coord/normal (example: f 1/1/1 2/2/1 3/3/1)
// Constructs vertices and put them into the Mesh_builder obejct.
void parse_face_pntc(By_line_iterator& it, Wf_mesh_data& wf_mesh_data, Mesh_builder& mesh_builder)
{
	long long position_count = wf_mesh_data.positions.size();
	long long normal_count = wf_mesh_data.normals.size();
	long long tex_coord_count = wf_mesh_data.tex_coords.size();

	uint32_t index_counter = 0;
	for (; it != By_line_iterator::end; ++it) {
		auto& line = *it;
		auto line_type = parse_line_type(line);
		if (line_type != Wf_line_type::face) break;

		long long p0, p1, p2, n0, n1, n2, tc0, tc1, tc2;
		int count = sscanf(line.c_str(), "f %lld/%lld/%lld %lld/%lld/%lld %lld/%lld/%lld",
			&p0, &tc0, &n0, &p1, &tc1, &n1, &p2, &tc2, &n2);
		ENFORCE(count == 9u, "Invalid face format. Position tex_coord and normal indices were expected.");
		normalize_wf_indices(position_count, p0, p1, p2);
		normalize_wf_indices(normal_count, n0, n1, n2);
		normalize_wf_indices(tex_coord_count, tc0, tc1, tc2);

		Vertex_ts vertices[3];
		vertices[0].position = wf_mesh_data.positions[static_cast<size_t>(p0)];
		vertices[1].position = wf_mesh_data.positions[static_cast<size_t>(p1)];
		vertices[2].position = wf_mesh_data.positions[static_cast<size_t>(p2)];
		vertices[0].normal = wf_mesh_data.normals[static_cast<size_t>(n0)];
		vertices[1].normal = wf_mesh_data.normals[static_cast<size_t>(n1)];
		vertices[2].normal = wf_mesh_data.normals[static_cast<size_t>(n2)];
		vertices[0].tex_coord = wf_mesh_data.tex_coords[static_cast<size_t>(tc0)];
		vertices[1].tex_coord = wf_mesh_data.tex_coords[static_cast<size_t>(tc1)];
		vertices[2].tex_coord = wf_mesh_data.tex_coords[static_cast<size_t>(tc2)];
		mesh_builder.push_back_triangle(vertices[0], vertices[1], vertices[2]);
	}
}

// Parses wavefront face definitions which contain vertex position/tex_coord/normal (example: f 1/1/1 2/2/1 3/3/1).
// Constructs vertices and put them into the Mesh_builder obejct.
// Also the function computes tangent & bitangent vectors for each vertex.
void parse_face_pntc_ts(By_line_iterator& it, Wf_mesh_data& wf_mesh_data, Mesh_builder& mesh_builder)
{
	long long position_count = wf_mesh_data.positions.size();
	long long normal_count = wf_mesh_data.normals.size();
	long long tex_coord_count = wf_mesh_data.tex_coords.size();

	for (; it != By_line_iterator::end; ++it) {
		auto& line = *it;
		auto line_type = parse_line_type(line);
		if (line_type != Wf_line_type::face) break;

		long long p0, p1, p2, n0, n1, n2, tc0, tc1, tc2;
		int count = sscanf(line.c_str(), "f %lld/%lld/%lld %lld/%lld/%lld %lld/%lld/%lld",
			&p0, &tc0, &n0, &p1, &tc1, &n1, &p2, &tc2, &n2);
		ENFORCE(count == 9u, "Invalid face format. Position tex_coord and normal indices were expected.");
		normalize_wf_indices(position_count, p0, p1, p2);
		normalize_wf_indices(normal_count, n0, n1, n2);
		normalize_wf_indices(tex_coord_count, tc0, tc1, tc2);

		Vertex_ts vertices[3];
		vertices[0].position = wf_mesh_data.positions[static_cast<size_t>(p0)];
		vertices[1].position = wf_mesh_data.positions[static_cast<size_t>(p1)];
		vertices[2].position = wf_mesh_data.positions[static_cast<size_t>(p2)];
		vertices[0].normal = wf_mesh_data.normals[static_cast<size_t>(n0)];
		vertices[1].normal = wf_mesh_data.normals[static_cast<size_t>(n1)];
		vertices[2].normal = wf_mesh_data.normals[static_cast<size_t>(n2)];
		vertices[0].tex_coord = wf_mesh_data.tex_coords[static_cast<size_t>(tc0)];
		vertices[1].tex_coord = wf_mesh_data.tex_coords[static_cast<size_t>(tc1)];
		vertices[2].tex_coord = wf_mesh_data.tex_coords[static_cast<size_t>(tc2)];

		auto tan_bitan = compute_tangent_bitangent(
			vertices[0].position, vertices[0].tex_coord,
			vertices[1].position, vertices[1].tex_coord, 
			vertices[2].position, vertices[2].tex_coord);
		vertices[0].tangent = vertices[1].tangent = vertices[2].tangent = tan_bitan.first;
		vertices[0].bitangent = vertices[1].bitangent = vertices[2].bitangent = tan_bitan.second;

		mesh_builder.push_back_triangle(vertices[0], vertices[1], vertices[2]);
	}
}

// Parses wavefront face definitions which contain only vertex position (example: f 1 2 3)
// constructs vertices and put them into Interleaved_mesh_data obejct.
void parse_faces_p_old(By_line_iterator& it, Wf_mesh_data& wf_mesh_data, Interleaved_mesh_data_old& imd)
{
	long long position_count = wf_mesh_data.positions.size();

	uint32_t index_counter = 0;
	for (; it != By_line_iterator::end; ++it) {
		auto& line = *it;
		auto line_type = parse_line_type(line);
		if (line_type != Wf_line_type::face) break;

		long long p0, p1, p2;
		int count = sscanf(line.c_str(), "f %lld %lld %lld", &p0, &p1, &p2);
		ENFORCE(count == 3u, "Invalid face format. Only position indices were expected.");
		normalize_wf_indices(position_count, p0, p1, p2);

		std::array<Vertex_old, 3> vertices;
		vertices[0].position = wf_mesh_data.positions[static_cast<size_t>(p0)];
		vertices[1].position = wf_mesh_data.positions[static_cast<size_t>(p1)];
		vertices[2].position = wf_mesh_data.positions[static_cast<size_t>(p2)];
		imd.push_back_vertices(vertices[0], vertices[1], vertices[2]);
		imd.push_back_indices(index_counter, index_counter + 1, index_counter + 2);
		index_counter += 3;
	}
}

// Parses wavefront face definitions which contain vertex position//normal (example: f 1//1 2//1 3//1)
// constructs vertices and put them into Interleaved_mesh_data obejct.
void parse_face_pn_old(By_line_iterator& it, Wf_mesh_data& wf_mesh_data, Interleaved_mesh_data_old& imd)
{
	long long position_count = wf_mesh_data.positions.size();
	long long normal_count = wf_mesh_data.normals.size();

	uint32_t index_counter = 0;
	for (; it != By_line_iterator::end; ++it) {
		auto& line = *it;
		auto line_type = parse_line_type(line);
		if (line_type != Wf_line_type::face) break;

		long long p0, p1, p2, n0, n1, n2;
		int count = sscanf(line.c_str(), "f %lld//%lld %lld//%lld %lld//%lld", &p0, &n0, &p1, &n1, &p2, &n2);
		ENFORCE(count == 6u, "Invalid face format. Position and normal indices were expected.");
		normalize_wf_indices(position_count, p0, p1, p2);
		normalize_wf_indices(normal_count, n0, n1, n2);

		std::array<Vertex_old, 3> vertices;
		vertices[0].position = wf_mesh_data.positions[static_cast<size_t>(p0)];
		vertices[1].position = wf_mesh_data.positions[static_cast<size_t>(p1)];
		vertices[2].position = wf_mesh_data.positions[static_cast<size_t>(p2)];
		vertices[0].normal = wf_mesh_data.normals[static_cast<size_t>(n0)];
		vertices[1].normal = wf_mesh_data.normals[static_cast<size_t>(n1)];
		vertices[2].normal = wf_mesh_data.normals[static_cast<size_t>(n2)];
		imd.push_back_vertices(vertices[0], vertices[1], vertices[2]);
		imd.push_back_indices(index_counter, index_counter + 1, index_counter + 2);
		index_counter += 3;
	}
}

// Parses wavefront face definitions which contain vertex position/tex_coord/normal (example: f 1/1/1 2/2/1 3/3/1)
// constructs vertices and put them into Interleaved_mesh_data obejct.
void parse_face_pntc_old(By_line_iterator& it, Wf_mesh_data& wf_mesh_data, Interleaved_mesh_data_old& imd, bool calc_tangent_h)
{
	long long position_count = wf_mesh_data.positions.size();
	long long normal_count = wf_mesh_data.normals.size();
	long long tex_coord_count = wf_mesh_data.tex_coords.size();

	uint32_t index_counter = 0;
	for (; it != By_line_iterator::end; ++it) {
		auto& line = *it;
		auto line_type = parse_line_type(line);
		if (line_type != Wf_line_type::face) break;

		long long p0, p1, p2, n0, n1, n2, tc0, tc1, tc2;
		int count = sscanf(line.c_str(), "f %lld/%lld/%lld %lld/%lld/%lld %lld/%lld/%lld", 
			&p0, &tc0, &n0, &p1, &tc1, &n1, &p2, &tc2, &n2);
		ENFORCE(count == 9u, "Invalid face format. Position tex_coord and normal indices were expected.");
		normalize_wf_indices(position_count, p0, p1, p2);
		normalize_wf_indices(normal_count, n0, n1, n2);
		normalize_wf_indices(tex_coord_count, tc0, tc1, tc2);

		std::array<Vertex_old, 3> vertices;
		vertices[0].position = wf_mesh_data.positions[static_cast<size_t>(p0)];
		vertices[1].position = wf_mesh_data.positions[static_cast<size_t>(p1)];
		vertices[2].position = wf_mesh_data.positions[static_cast<size_t>(p2)];
		vertices[0].normal = wf_mesh_data.normals[static_cast<size_t>(n0)];
		vertices[1].normal = wf_mesh_data.normals[static_cast<size_t>(n1)];
		vertices[2].normal = wf_mesh_data.normals[static_cast<size_t>(n2)];
		vertices[0].tex_coord = wf_mesh_data.tex_coords[static_cast<size_t>(tc0)];
		vertices[1].tex_coord = wf_mesh_data.tex_coords[static_cast<size_t>(tc1)];
		vertices[2].tex_coord = wf_mesh_data.tex_coords[static_cast<size_t>(tc2)];
		
		if (calc_tangent_h) {
			float4 tangent_space = compute_tangent_h(vertices[0], vertices[1], vertices[2]);
			vertices[0].tangent_space = vertices[1].tangent_space = vertices[2].tangent_space = tangent_space;
			
			//if (vertices[0].normal != vertices[1].normal || vertices[1].normal != vertices[2].normal)
			//{
			//	// TODO(ref2401): get rid of this ugly workaround if you remember it's here.
			//	float4 tangent_h1 = compute_tangent_h(vertices[1], vertices[2], vertices[0]);
			//	float4 tangent_h2= compute_tangent_h(vertices[2], vertices[0], vertices[1]);
			//	vertices[1].tangent_h = tangent_h1;
			//	vertices[2].tangent_h = tangent_h2;
			//}
		}

		imd.push_back_vertices(vertices[0], vertices[1], vertices[2]);
		imd.push_back_indices(index_counter, index_counter + 1, index_counter + 2);
		index_counter += 3;
	}
}

// Parses wavefront face definitions which contain vertex position/tex_coord (example: f 1/1 2/2 3/3)
// constructs vertices and put them into Interleaved_mesh_data obejct.
void parse_face_ptc_old(By_line_iterator& it, Wf_mesh_data& wf_mesh_data, Interleaved_mesh_data_old& imd)
{
	long long position_count = wf_mesh_data.positions.size();
	long long tex_coord_count = wf_mesh_data.tex_coords.size();

	uint32_t index_counter = 0;
	for (; it != By_line_iterator::end; ++it) {
		auto& line = *it;
		auto line_type = parse_line_type(line);
		if (line_type != Wf_line_type::face) break;

		long long p0, p1, p2, tc0, tc1, tc2;
		int count = sscanf(line.c_str(), "f %lld/%lld %lld/%lld %lld/%lld", &p0, &tc0, &p1, &tc1, &p2, &tc2);
		ENFORCE(count == 6u, "Invalid face format. Position and tex_coord indices were expected.");
		normalize_wf_indices(position_count, p0, p1, p2);
		normalize_wf_indices(tex_coord_count, tc0, tc1, tc2);

		std::array<Vertex_old, 3> vertices;
		vertices[0].position = wf_mesh_data.positions[static_cast<size_t>(p0)];
		vertices[1].position = wf_mesh_data.positions[static_cast<size_t>(p1)];
		vertices[2].position = wf_mesh_data.positions[static_cast<size_t>(p2)];
		vertices[0].tex_coord = wf_mesh_data.tex_coords[static_cast<size_t>(tc0)];
		vertices[1].tex_coord = wf_mesh_data.tex_coords[static_cast<size_t>(tc1)];
		vertices[2].tex_coord = wf_mesh_data.tex_coords[static_cast<size_t>(tc2)];
		imd.push_back_vertices(vertices[0], vertices[1], vertices[2]);
		imd.push_back_indices(index_counter, index_counter + 1, index_counter + 2);
		index_counter += 3;
	}
}

// Parses a vector of 3 floats that represents 3D normal vector.
float3 parse_normal(const std::string& line)
{
	float x, y, z;
	int count = std::sscanf(line.c_str(), "vn %f %f %f", &x, &y, &z);
	ENFORCE(count == 3, "Unexpected number of normal components.");

	return normalize(float3(x, y, z));
}

// Parses a vector of 3 floats that represents 3D position.
float3 parse_position(const std::string& line)
{
	float x, y, z;
	int count = std::sscanf(line.c_str(), "v %f %f %f", &x, &y, &z);
	ENFORCE(count == 3, "Unexpected number of position components.");

	return float3(x, y, z);
}

// Parses a vector of 2 floats that represents texure coordinates.
float2 parse_tex_coord(const std::string& line)
{
	float x, y;
	int count = std::sscanf(line.c_str(), "vt %f %f", &x, &y);
	ENFORCE(count == 2, "Unexpected number of tex_coord components.");

	return float2(x, y);
}

#pragma warning(pop)

// Loads, parses and constructs a mesh object using the specified file iterator.
Interleaved_mesh_data_old load_mesh_wavefront_old(By_line_iterator it, Vertex_attribs attribs,
	size_t approx_vertex_count = 0, size_t approx_index_count = 0)
{
	assert(attribs != Vertex_attribs::none);

	static thread_local Wf_mesh_data wf_mesh_data(256);
	wf_mesh_data.clear();
	wf_mesh_data.reserve(approx_vertex_count);

	// read mesh data from file
	for (; it != By_line_iterator::end; ++it) {
		auto& line = *it;

		auto line_type = parse_line_type(line);
		if (line_type == Wf_line_type::other) continue;
		if (line_type == Wf_line_type::face) break;

		switch (line_type) {
			case Wf_line_type::position: {
				float3 p = parse_position(line);
				wf_mesh_data.positions.push_back(p);
				break;
			}

			case Wf_line_type::normal: {
				float3 n = parse_normal(line);
				wf_mesh_data.normals.push_back(n);
				break;
			}

			case Wf_line_type::tex_coord: {
				float2 tc = parse_tex_coord(line);
				wf_mesh_data.tex_coords.push_back(tc);
				break;
			}

		} // switch
	}

	// validate mesh data
	ENFORCE(wf_mesh_data.positions.size() > 0u, "Invalid mesh file. Expected position values.");
	if (has_normal(attribs)) 
		ENFORCE(wf_mesh_data.has_normals(), "Invalid mesh file. Expected normal values.");
	if (has_tex_coord(attribs))
		ENFORCE(wf_mesh_data.has_tex_coords(), "Invalid mesh file. Expected tex coord values.");

	// pack data
	size_t init_vertex_count = (approx_vertex_count) ? approx_vertex_count : wf_mesh_data.positions.size();
	size_t init_index_cout = (approx_index_count) ? approx_index_count : wf_mesh_data.positions.size();
	cg::data::Interleaved_mesh_data_old imd(attribs, init_vertex_count, init_index_cout);


	if (wf_mesh_data.has_normals() && wf_mesh_data.has_tex_coords()) {
		parse_face_pntc_old(it, wf_mesh_data, imd, has_tangent_space(attribs));
	}
	else if (wf_mesh_data.has_normals()) {
		parse_face_pn_old(it, wf_mesh_data, imd);
	}
	else if (wf_mesh_data.has_tex_coords()) {
		parse_face_ptc_old(it, wf_mesh_data, imd);
	}
	else {
		parse_faces_p_old(it, wf_mesh_data, imd);
	}

	return imd;
}

} // namespace

namespace cg {
namespace file {

namespace internal {

// ----- Wf_mesh_data -----

Wf_mesh_data::Wf_mesh_data(size_t vertex_count)
{
	reserve(vertex_count);
}

void Wf_mesh_data::clear() noexcept
{
	positions.clear();
	normals.clear();
	tex_coords.clear();
}

void Wf_mesh_data::reserve(size_t vertex_count)
{
	if (vertex_count > 0) {
		positions.reserve(vertex_count);
		normals.reserve(vertex_count);
		tex_coords.reserve(vertex_count);
	}
}

// ----- funcs -----

void load_mesh_wavefront(By_line_iterator it, Vertex_attribs attribs, Wf_mesh_data& wf_mesh_data, Mesh_builder& mesh_builder)
{
	using cg::data::has_normal;
	using cg::data::has_position;
	using cg::data::has_tex_coord;

	// read mesh data from file
	for (; it != By_line_iterator::end; ++it) {
		auto& line = *it;

		auto line_type = parse_line_type(line);
		if (line_type == Wf_line_type::other) continue;
		if (line_type == Wf_line_type::face) break;

		switch (line_type) {
			case Wf_line_type::position: {
				float3 p = parse_position(line);
				wf_mesh_data.positions.push_back(p);
				break;
			}

			case Wf_line_type::normal: {
				float3 n = parse_normal(line);
				wf_mesh_data.normals.push_back(n);
				break;
			}

			case Wf_line_type::tex_coord: {
				float2 tc = parse_tex_coord(line);
				wf_mesh_data.tex_coords.push_back(tc);
				break;
			}

		} // switch
	}

	// validate mesh data
	ENFORCE(wf_mesh_data.positions.size() > 0u, "Invalid mesh file. Expected position values.");
	if (has_normal(attribs))
		ENFORCE(wf_mesh_data.has_normals(), "Invalid mesh file. Expected normal values.");
	if (has_tex_coord(attribs))
		ENFORCE(wf_mesh_data.has_tex_coords(), "Invalid mesh file. Expected tex coord values.");

	if (wf_mesh_data.has_normals() && wf_mesh_data.has_tex_coords()) {
		if (has_tangent_space(attribs)) {
			parse_face_pntc_ts(it, wf_mesh_data, mesh_builder);
		}
		else {
			parse_face_pntc(it, wf_mesh_data, mesh_builder);
		}
	}
	else if (wf_mesh_data.has_normals()) {
		parse_face_pn(it, wf_mesh_data, mesh_builder);
	}
	else if (wf_mesh_data.has_tex_coords()) {
		parse_face_ptc(it, wf_mesh_data, mesh_builder);
	}
	else {
		parse_faces_p(it, wf_mesh_data, mesh_builder);
	}
}

} // namespace internal


cg::data::Interleaved_mesh_data_old load_mesh_wavefront_old(const std::string& filename, 
	cg::data::Vertex_attribs attribs, size_t approx_vertex_count, size_t approx_index_count)
{
	By_line_iterator it(filename);
	return ::load_mesh_wavefront_old(std::move(it), attribs, approx_vertex_count, approx_index_count);
}

cg::data::Interleaved_mesh_data_old load_mesh_wavefront_old(const char* filename, 
	cg::data::Vertex_attribs attribs, size_t approx_vertex_count, size_t approx_index_count)
{
	By_line_iterator it(filename);
	return ::load_mesh_wavefront_old(std::move(it), attribs, approx_vertex_count, approx_index_count);
}

} // namespace file
} // namespace cg
