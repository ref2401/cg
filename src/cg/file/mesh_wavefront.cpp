#include "cg/file/file.h"

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
using cg::data::Vertex_old;
using cg::data::Vertex_attribs;
using cg::data::compute_tangent_h;
using cg::file::By_line_iterator;


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

// ----- Wf_mesh_data -----

// The class is used as in-memory storage of a wavefront (.obj) file.
// The file's content is read to an Wf_mesh_data object and then the object constructs a mesh.
class Wf_mesh_data final {
public:

	Wf_mesh_data();


	void clear()
	{
		positions.clear();
		normals.clear();
		tex_coords.clear();
	}

	bool has_normals() const noexcept
	{
		return (normals.size() > 0u);
	}

	bool has_tex_coords() const noexcept
	{
		return (tex_coords.size() > 0u);
	}


	std::vector<float3> positions;
	std::vector<float3> normals;
	std::vector<float2> tex_coords;
};

Wf_mesh_data::Wf_mesh_data()
{
	positions.reserve(100);
	normals.reserve(100);
	tex_coords.reserve(100);
}

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
// constructs vertices and put them into Interleaved_mesh_data obejct.
void parse_faces_p(By_line_iterator& it, Wf_mesh_data& mesh_data, Interleaved_mesh_data& imd)
{
	long long position_count = mesh_data.positions.size();

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
		vertices[0].position = mesh_data.positions[static_cast<size_t>(p0)];
		vertices[1].position = mesh_data.positions[static_cast<size_t>(p1)];
		vertices[2].position = mesh_data.positions[static_cast<size_t>(p2)];
		imd.push_back_vertices(vertices[0], vertices[1], vertices[2]);
		imd.push_back_indices(index_counter, index_counter + 1, index_counter + 2);
		index_counter += 3;
	}
}

// Parses wavefront face definitions which contain vertex position//normal (example: f 1//1 2//1 3//1)
// constructs vertices and put them into Interleaved_mesh_data obejct.
void parse_face_pn(By_line_iterator& it, Wf_mesh_data& mesh_data, Interleaved_mesh_data& imd)
{
	long long position_count = mesh_data.positions.size();
	long long normal_count = mesh_data.normals.size();

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
		vertices[0].position = mesh_data.positions[static_cast<size_t>(p0)];
		vertices[1].position = mesh_data.positions[static_cast<size_t>(p1)];
		vertices[2].position = mesh_data.positions[static_cast<size_t>(p2)];
		vertices[0].normal = mesh_data.normals[static_cast<size_t>(n0)];
		vertices[1].normal = mesh_data.normals[static_cast<size_t>(n1)];
		vertices[2].normal = mesh_data.normals[static_cast<size_t>(n2)];
		imd.push_back_vertices(vertices[0], vertices[1], vertices[2]);
		imd.push_back_indices(index_counter, index_counter + 1, index_counter + 2);
		index_counter += 3;
	}
}

// Parses wavefront face definitions which contain vertex position/tex_coord/normal (example: f 1/1/1 2/2/1 3/3/1)
// constructs vertices and put them into Interleaved_mesh_data obejct.
void parse_face_pntc(By_line_iterator& it, Wf_mesh_data& mesh_data, Interleaved_mesh_data& imd, bool calc_tangent_h)
{
	long long position_count = mesh_data.positions.size();
	long long normal_count = mesh_data.normals.size();
	long long tex_coord_count = mesh_data.tex_coords.size();

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
		vertices[0].position = mesh_data.positions[static_cast<size_t>(p0)];
		vertices[1].position = mesh_data.positions[static_cast<size_t>(p1)];
		vertices[2].position = mesh_data.positions[static_cast<size_t>(p2)];
		vertices[0].normal = mesh_data.normals[static_cast<size_t>(n0)];
		vertices[1].normal = mesh_data.normals[static_cast<size_t>(n1)];
		vertices[2].normal = mesh_data.normals[static_cast<size_t>(n2)];
		vertices[0].tex_coord = mesh_data.tex_coords[static_cast<size_t>(tc0)];
		vertices[1].tex_coord = mesh_data.tex_coords[static_cast<size_t>(tc1)];
		vertices[2].tex_coord = mesh_data.tex_coords[static_cast<size_t>(tc2)];
		
		if (calc_tangent_h) {
			float4 tangent_h = compute_tangent_h(vertices[0], vertices[1], vertices[2]);
			vertices[0].tangent_h = vertices[1].tangent_h = vertices[2].tangent_h = tangent_h;
			
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
void parse_face_ptc(By_line_iterator& it, Wf_mesh_data& mesh_data, Interleaved_mesh_data& imd)
{
	long long position_count = mesh_data.positions.size();
	long long tex_coord_count = mesh_data.tex_coords.size();

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
		vertices[0].position = mesh_data.positions[static_cast<size_t>(p0)];
		vertices[1].position = mesh_data.positions[static_cast<size_t>(p1)];
		vertices[2].position = mesh_data.positions[static_cast<size_t>(p2)];
		vertices[0].tex_coord = mesh_data.tex_coords[static_cast<size_t>(tc0)];
		vertices[1].tex_coord = mesh_data.tex_coords[static_cast<size_t>(tc1)];
		vertices[2].tex_coord = mesh_data.tex_coords[static_cast<size_t>(tc2)];
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

	return float3(x, y, z);
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
Interleaved_mesh_data load_mesh_wavefront(By_line_iterator it, Vertex_attribs attribs,
	size_t approx_vertex_count = 0, size_t approx_index_cont = 0)
{
	assert(attribs != Vertex_attribs::none);

	static thread_local Wf_mesh_data mesh_data;
	mesh_data.clear();

	// read mesh data from file
	for (; it != By_line_iterator::end; ++it) {
		auto& line = *it;

		auto line_type = parse_line_type(line);
		if (line_type == Wf_line_type::other) continue;
		if (line_type == Wf_line_type::face) break;

		switch (line_type) {
			case Wf_line_type::position: {
				float3 p = parse_position(line);
				mesh_data.positions.push_back(p);
				break;
			}

			case Wf_line_type::normal: {
				float3 n = parse_normal(line);
				mesh_data.normals.push_back(n);
				break;
			}

			case Wf_line_type::tex_coord: {
				float2 tc = parse_tex_coord(line);
				mesh_data.tex_coords.push_back(tc);
				break;
			}

		} // switch
	}

	// validate mesh data
	ENFORCE(mesh_data.positions.size() > 0u, "Invalid mesh file. Expected position values.");
	if (has_normal(attribs)) 
		ENFORCE(mesh_data.has_normals(), "Invalid mesh file. Expected normal values.");
	if (has_tex_coord(attribs))
		ENFORCE(mesh_data.has_tex_coords(), "Invalid mesh file. Expected tex coord values.");

	// pack data
	size_t init_vertex_count = (approx_vertex_count) ? approx_vertex_count : mesh_data.positions.size();
	size_t init_index_cout = (approx_index_cont) ? approx_index_cont : mesh_data.positions.size();
	cg::data::Interleaved_mesh_data imd(attribs, init_vertex_count, init_index_cout);

	auto n = mesh_data.has_normals();
	auto tc = mesh_data.has_tex_coords();
	if (mesh_data.has_normals() && mesh_data.has_tex_coords()) {
		parse_face_pntc(it, mesh_data, imd, has_tangent_space(attribs));
	}
	else if (mesh_data.has_normals()) {
		parse_face_pn(it, mesh_data, imd);
	}
	else if (mesh_data.has_tex_coords()) {
		parse_face_ptc(it, mesh_data, imd);
	}
	else {
		parse_faces_p(it, mesh_data, imd);
	}

	return imd;
}

} // namespace

namespace cg {
namespace file {

cg::data::Interleaved_mesh_data load_mesh_wavefront(const std::string& filename, 
	cg::data::Vertex_attribs attribs, size_t approx_vertex_count, size_t approx_index_cont)
{
	By_line_iterator it(filename);
	return ::load_mesh_wavefront(std::move(it), attribs, approx_vertex_count, approx_index_cont);
}

cg::data::Interleaved_mesh_data load_mesh_wavefront(const char* filename, 
	cg::data::Vertex_attribs attribs, size_t approx_vertex_count, size_t approx_index_cont)
{
	By_line_iterator it(filename);
	return ::load_mesh_wavefront(std::move(it), attribs, approx_vertex_count, approx_index_cont);
}

} // namespace file
} // namespace cg
