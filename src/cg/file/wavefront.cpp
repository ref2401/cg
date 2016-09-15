#include "cg/file/file.h"

#include <cstdint>
#include <utility>
#include <vector>
#include "cg/math/math.h"


namespace {

using cg::float2;
using cg::float3;
using cg::float4;
using cg::enforce;
using cg::data::Vertex;
using cg::data::Vertex_attribs;
using cg::data::compute_tangent_h;
using cg::file::By_line_iterator;
using cg::file::File_exception;


// Wf_face reprsents a triangle face. Each vertex of the triangle can have 
// posions, normals & tex_coords. Wf_face itself contains indices 
// which can be used to obtain positions, normals and tex_coords from the Wf_mesh_data.
// Example:
// Wf_face face;
// Wf_mesh_data data;
// data.positions[face.p0] returns the first position value of the face.
struct Wf_face {
	// Position indices.
	uint32_t p0, p1, p2;
	
	// Normal indices.
	uint32_t n0, n1, n2;

	// Texture coordinates indices.
	uint32_t tc0, tc1, tc2;
};

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

// The class is used as in-memory storage of a wavefront (.obj) file.
// The file's content is read to an Wf_mesh_data object and then the object constructs a mesh.
struct Wf_mesh_data {
	std::vector<float3> positions;
	std::vector<float3> normals;
	std::vector<float2> tex_coords;
	std::vector<Wf_face> faces;

	bool has_normals() const noexcept
	{
		return (normals.size() > 0u);
	}

	bool has_tex_coords() const noexcept
	{
		return (tex_coords.size() > 0u);
	}
};

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

// Parses Wf_face struct value from the given line.
// has_normals & has_tex_coords are used to determine the right face format.
Wf_face parse_face(const std::string& line, bool has_normals, bool has_tex_coords)
{
	// possible face formats:
	// 1. position/tex_coord/normal:	f 1/1/1 2/2/1 3/3/1
	// 2. position//normal:				f 1//1 2//1 3//1
	// 3. position/tex_coord:			f 1/1 2/2 3/3
	// 4. position:						f 1 2 3

	// A valid vertex index starts from 1 and matches the corresponding 
	// vertex elements of a previously defined vertex list.

	Wf_face face;

	if (has_normals && has_tex_coords) {
		int count = sscanf(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d",
			&face.p0, &face.tc0, &face.n0, &face.p1, &face.tc1, &face.n1, &face.p2, &face.tc2, &face.n2);
		enforce<File_exception>(count == 9u, "Invalid face format. Position tex_coord and normal indices were expected.");
	}
	else if (has_normals) {
		int count = sscanf(line.c_str(), "f %d//%d %d//%d %d//%d",
			&face.p0, &face.n0, &face.p1, &face.n1, &face.p2, &face.n2);
		enforce<File_exception>(count == 6u, "Invalid face format. Position and normal indices were expected.");
	}
	else if (has_tex_coords) {
		int count = sscanf(line.c_str(), "f %d/%d %d/%d %d/%d",
			&face.p0, &face.tc0, &face.p1, &face.tc1, &face.p2, &face.tc2);
		enforce<File_exception>(count == 6u, "Invalid face format. Position and tex_coord indices were expected.");
	}
	else {
		int count = sscanf(line.c_str(), "f %d %d %d", &face.p0, &face.p1, &face.p2);
		enforce<File_exception>(count == 3u, "Invalid face format. Only position indices were expected.");
	}

	return face;
}

// Parses a vector of 3 floats that represents 3D normal vector.
float3 parse_normal(const std::string& line)
{
	float x, y, z;
	int count = std::sscanf(line.c_str(), "vn %f %f %f", &x, &y, &z);
	enforce<File_exception>(count == 3, "Unexpected number of normal components.");

	return float3(x, y, z);
}

// Parses a vector of 3 floats that represents 3D position.
float3 parse_position(const std::string& line)
{
	float x, y, z;
	int count = std::sscanf(line.c_str(), "v %f %f %f", &x, &y, &z);
	enforce<File_exception>(count == 3, "Unexpected number of position components.");

	return float3(x, y, z);
}

// Parses a vector of 2 floats that represents texure coordinates.
float2 parse_tex_coord(const std::string& line)
{
	float x, y;
	int count = std::sscanf(line.c_str(), "vt %f %f", &x, &y);
	enforce<File_exception>(count == 2, "Unexpected number of tex_coord components.");

	return float2(x, y);
}

#pragma warning(pop)

// Loads, parses and constructs a mesh object using the specified file iterator.
void load_mesh_wavefront(By_line_iterator it)
{
	Wf_mesh_data mesh_data;

	for (; it != By_line_iterator::end; ++it) {
		auto& line = *it;

		auto line_type = parse_line_type(line);
		if (line_type == Wf_line_type::other) continue;

		switch (line_type) {
		case Wf_line_type::face: {
			Wf_face f = parse_face(line, mesh_data.has_normals(), mesh_data.has_tex_coords());
			mesh_data.faces.push_back(f);
			break;
		}

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

	//bool b = mesh_data.has_normals();
}

} // namespace

namespace cg {
namespace file {

void load_mesh_wavefront(const std::string& filename)
{
	By_line_iterator it(filename);
	::load_mesh_wavefront(std::move(it));
}

void load_mesh_wavefront(const char* filename)
{
	By_line_iterator it(filename);
	::load_mesh_wavefront(std::move(it));
}

} // namespace file
} // namespace cg
