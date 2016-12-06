#include "cg/data/mesh_old.h"

#include <array>
#include <limits>
#include "cg/math/math.h"


namespace {

using cg::float2;
using cg::float3;
using cg::float4;
using cg::data::Interleaved_mesh_data_old;
using cg::data::Vertex_old;
using cg::data::Vertex_attribs_old;
using cg::data::compute_tangent_h;
using cg::data::By_line_iterator;


// The class is used as in-memory storage of a wavefront (.obj) file.
// The file's content is read to an Wf_mesh_data object and then the object constructs a mesh.
class Wf_mesh_data final {
public:

	Wf_mesh_data(size_t vertex_count)
	{
		reserve(vertex_count);
	}


	void clear() noexcept
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

	void reserve(size_t vertex_count)
	{
		if (vertex_count > 0) {
			positions.reserve(vertex_count);
			normals.reserve(vertex_count);
			tex_coords.reserve(vertex_count);
		}
	}


	std::vector<float3> positions;
	std::vector<float3> normals;
	std::vector<float2> tex_coords;
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
Interleaved_mesh_data_old load_mesh_wavefront_old(By_line_iterator it, Vertex_attribs_old attribs,
	size_t approx_vertex_count = 0, size_t approx_index_count = 0)
{
	assert(attribs != Vertex_attribs_old::none);

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
namespace data {

// ----- Interleaved_mesh_data_old -----

Interleaved_mesh_data_old::Interleaved_mesh_data_old(Vertex_attribs_old attribs, size_t vertex_count, size_t index_count)
	: _format(attribs)
{
	assert(attribs != Vertex_attribs_old::none);
	_data.reserve(_format.component_count() * vertex_count);
	_indices.reserve(index_count);
}

void Interleaved_mesh_data_old::push_back_index(uint32_t i)
{
	_indices.push_back(i);
}

void Interleaved_mesh_data_old::push_back_indices(uint32_t i0, uint32_t i1, uint32_t i2)
{
	uint32_t index_buffer[3];
	index_buffer[0] = i0;
	index_buffer[1] = i1;
	index_buffer[2] = i2;
	_indices.insert(_indices.end(), index_buffer, index_buffer + std::extent<decltype(index_buffer)>::value);
}

void Interleaved_mesh_data_old::push_back_vertex(const Vertex_old& v)
{
	float data_buffer[12];
	size_t offset = 0;

	if (has_position(_format.attribs)) {
		data_buffer[0] = v.position.x;
		data_buffer[1] = v.position.y;
		data_buffer[2] = v.position.z;
		offset += 3;
	}

	if (has_normal(_format.attribs)) {
		data_buffer[offset]		= v.normal.x;
		data_buffer[offset + 1] = v.normal.y;
		data_buffer[offset + 2] = v.normal.z;
		offset += 3;
	}

	if (has_tex_coord(_format.attribs)) {
		data_buffer[offset]		= v.tex_coord.u;
		data_buffer[offset + 1] = v.tex_coord.v;
		offset += 2;
	}

	if (has_tangent_space(_format.attribs)) {
		data_buffer[offset]		= v.tangent_space.x;
		data_buffer[offset + 1] = v.tangent_space.y;
		data_buffer[offset + 2] = v.tangent_space.z;
		data_buffer[offset + 3] = v.tangent_space.w;
		offset += 4;
	}

	_data.insert(_data.end(), data_buffer, data_buffer + offset);
}

void Interleaved_mesh_data_old::push_back_vertices(const Vertex_old& v0, const Vertex_old& v1, const Vertex_old& v2)
{
	push_back_vertex(v0);
	push_back_vertex(v1);
	push_back_vertex(v2);
}

// ----- funcs -----

std::ostream& operator<<(std::ostream& out, const Interleaved_vertex_format_old& fmt)
{
	out << fmt.attribs;
	return out;
}

std::wostream& operator<<(std::wostream& out, const Interleaved_vertex_format_old& fmt)
{
	out << fmt.attribs;
	return out;
}

std::ostream& operator<<(std::ostream& out, const Vertex_attribs_old& attribs)
{
	out << "Vertex_attribs_old(";

	if (attribs == Vertex_attribs_old::none) {
		out << "none";
	}
	else {
		if (has_position(attribs)) out << "position";
		if (has_normal(attribs)) out << "normal ";
		if (has_tex_coord(attribs)) out << "tex_coord ";
		if (has_tangent_space(attribs)) out << "tangent_h";
	}

	out << ")";
	return out;
}

std::wostream& operator<<(std::wostream& out, const Vertex_attribs_old& attribs)
{
	out << "Vertex_attribs_old(";

	if (attribs == Vertex_attribs_old::none) {
		out << "none";
	}
	else {
		if (has_position(attribs)) out << "position";
		if (has_normal(attribs)) out << "normal ";
		if (has_tex_coord(attribs)) out << "tex_coord ";
		if (has_tangent_space(attribs)) out << "tangent_h";
	}

	out << ")";
	return out;
}

std::ostream& operator<<(std::ostream& out, const Vertex_old& v)
{
	out << "Vertex_old(p: " << v.position
		<< "; n: " << v.normal
		<< "; tc: " << v.tex_coord
		<< "; tan_h: " << v.tangent_space << ")";
	return out;
}

std::wostream& operator<<(std::wostream& out, const Vertex_old& v)
{
	out << "Vertex_old(p: " << v.position
		<< "; n: " << v.normal
		<< "; tc: " << v.tex_coord
		<< "; tan_h: " << v.tangent_space << ")";
	return out;
}

float4 compute_tangent_h(const Vertex_old& v0, const Vertex_old& v1, const Vertex_old& v2)
{
	float3 pos0 = v0.position;
	float3 pos1 = v1.position;
	float3 pos2 = v2.position;

	float dx1 = pos1.x - pos0.x;
	float dx2 = pos2.x - pos0.x;
	float dy1 = pos1.y - pos0.y;
	float dy2 = pos2.y - pos0.y;
	float dz1 = pos1.z - pos0.z;
	float dz2 = pos2.z - pos0.z;

	float2 tc0 = v0.tex_coord;
	float2 tc1 = v1.tex_coord;
	float2 tc2 = v2.tex_coord;
	float ds1 = tc1.x - tc0.x;
	float ds2 = tc2.x - tc0.x;
	float dt1 = tc1.y - tc0.y;
	float dt2 = tc2.y - tc0.y;

	float r = 1.0f / (ds1 * dt2 - ds2 * dt1);
	float3 t = normalize(r * float3((dt2 * dx1 - dt1 * dx2), (dt2 * dy1 - dt1 * dy2), (dt2 * dz1 - dt1 * dz2)));
	float3 b = normalize(r * float3((ds1 * dx2 - ds2 * dx1), (ds1 * dy2 - ds2 * dy1), (ds1 * dz2 - ds2 * dz1)));
	float3 normal = v0.normal;
	float handedness = (dot(b, cross(normal, t)) > 0.f) ? 1.f : -1.f;

	// Gram-Schmidt orthogonalize.
	// project tangent vector onto normal.
	/*float3 t_prj_n = normal * dot(t, normal);
	float3 tangent = normalize(t - t_prj_n);
	return float4(tangent, handedness);*/
	return float4(t, handedness);
}

Interleaved_mesh_data_old load_mesh_wavefront_old(const std::string& filename,
	Vertex_attribs_old attribs, size_t approx_vertex_count, size_t approx_index_count)
{
	By_line_iterator it(filename);
	return ::load_mesh_wavefront_old(std::move(it), attribs, approx_vertex_count, approx_index_count);
}

Interleaved_mesh_data_old load_mesh_wavefront_old(const char* filename,
	Vertex_attribs_old attribs, size_t approx_vertex_count, size_t approx_index_count)
{
	By_line_iterator it(filename);
	return ::load_mesh_wavefront_old(std::move(it), attribs, approx_vertex_count, approx_index_count);
}

} // namesapce data
} // namespace cg