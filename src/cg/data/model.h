#ifndef CG_DATA_MODEL_H_
#define CG_DATA_MODEL_H_

#include <cassert>
#include <cstdint>
#include <iterator>
#include <ostream>
#include <type_traits>
#include <vector>
#include "cg/math/math.h"


namespace cg {
namespace data {

// All possible combinations of vertex attributes.
// p	- position;
// n	- normal;
// tc	- texture coordinates;
// ts	- tangent space (tangent & bitangent or tangnet & handedness);
enum class Vertex_attribs_new : unsigned char {
	p = 0,
	p_n,
	p_n_tc,
	p_tc,
	p_n_tc_ts
};

// 
template<Vertex_attribs_new attribs>
struct Vertex_interleaved_format_new;

using Vertex_interleaved_format_all = Vertex_interleaved_format_new<Vertex_attribs_new::p_n_tc_ts>;

template<>
struct Vertex_interleaved_format_new<Vertex_attribs_new::p_n_tc_ts> {
	static constexpr size_t position_component_count = 3;
	static constexpr size_t position_byte_count = sizeof(float) * position_component_count;
	static constexpr size_t position_byte_offset = 0;

	static constexpr size_t normal_component_count = 3;
	static constexpr size_t normal_byte_count = sizeof(float) * normal_component_count;
	static constexpr size_t normal_byte_offset = position_byte_offset + position_byte_count;

	static constexpr size_t tex_coord_component_count = 2;
	static constexpr size_t tex_coord_byte_count = sizeof(float) * tex_coord_component_count;
	static constexpr size_t tex_coord_byte_offset = normal_byte_offset + normal_byte_count;

	static constexpr size_t tangnet_space_component_count = 4;
	static constexpr size_t tangent_space_byte_count = sizeof(float) * tangnet_space_component_count;
	static constexpr size_t tangent_space_byte_offset = tex_coord_byte_offset + tex_coord_byte_count;

	static constexpr size_t vertex_component_count = position_component_count
		+ normal_component_count + tex_coord_component_count + tangnet_space_component_count;
	static constexpr size_t vertex_byte_count = position_byte_count
		+ normal_byte_count + tex_coord_byte_count + tangent_space_byte_count;
};

template<>
struct Vertex_interleaved_format_new<Vertex_attribs_new::p> {
	static constexpr size_t position_component_count = Vertex_interleaved_format_all::position_component_count;
	static constexpr size_t position_byte_count = Vertex_interleaved_format_all::position_byte_count;
	static constexpr size_t position_byte_offset = 0;

	static constexpr size_t vertex_component_count = position_component_count;
	static constexpr size_t vertex_byte_count = position_byte_count;
};

template<>
struct Vertex_interleaved_format_new<Vertex_attribs_new::p_n> {
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
struct Vertex_interleaved_format_new<Vertex_attribs_new::p_n_tc> {
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
struct Vertex_interleaved_format_new<Vertex_attribs_new::p_tc> {
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

//
template<Vertex_attribs_new attribs>
struct Model_geometry_vertex;

template<> 
struct Model_geometry_vertex<Vertex_attribs_new::p> final {

	Model_geometry_vertex() noexcept {}

	Model_geometry_vertex(const cg::float3& position) noexcept :
		position(position)
	{}

	union {
		struct {
			cg::float3 position;
		};

		unsigned char data[Vertex_interleaved_format_new<Vertex_attribs_new::p>::vertex_byte_count];
	};
};

template<>
struct Model_geometry_vertex<Vertex_attribs_new::p_n> final {

	Model_geometry_vertex() noexcept {}

	Model_geometry_vertex(const cg::float3& position, const cg::float3& normal) noexcept :
		position(position), normal(normal)
	{}

	union {
		struct {
			cg::float3 position;
			cg::float3 normal;
		};

		unsigned char data[Vertex_interleaved_format_new<Vertex_attribs_new::p_n>::vertex_byte_count];
	};
};

template<>
struct Model_geometry_vertex<Vertex_attribs_new::p_n_tc> final {

	Model_geometry_vertex() noexcept {}

	Model_geometry_vertex(const cg::float3& position, const cg::float3& normal,
		const cg::float2& tex_coord) noexcept :
		position(position), normal(normal), tex_coord(tex_coord)
	{}

	union {
		struct {
			cg::float3 position;
			cg::float3 normal;
			cg::float2 tex_coord;
		};

		unsigned char data[Vertex_interleaved_format_new<Vertex_attribs_new::p_n_tc>::vertex_byte_count];
	};
};

template<>
struct Model_geometry_vertex<Vertex_attribs_new::p_tc> final {

	Model_geometry_vertex() noexcept {}

	Model_geometry_vertex(const cg::float3& position, const cg::float2& tex_coord) noexcept :
		position(position), tex_coord(tex_coord)
	{}

	union {
		struct {
			cg::float3 position;
			cg::float2 tex_coord;
		};

		unsigned char data[Vertex_interleaved_format_new<Vertex_attribs_new::p_tc>::vertex_byte_count];
	};
};

template<>
struct Model_geometry_vertex<Vertex_attribs_new::p_n_tc_ts> final {
	
	Model_geometry_vertex() noexcept {}

	Model_geometry_vertex(const cg::float3& position, const cg::float3& normal,
		const cg::float2& tex_coord, const cg::float4& tangent_h) noexcept :
		position(position), normal(normal), tex_coord(tex_coord), tangent_h(tangent_h)
	{}

	union {
		struct {
			cg::float3 position;
			cg::float3 normal;
			cg::float2 tex_coord;
			cg::float4 tangent_h;
		};

		unsigned char data[Vertex_interleaved_format_new<Vertex_attribs_new::p_n_tc_ts>::vertex_byte_count];
	};
};

// 
template<Vertex_attribs_new attribs>
class Model_geometry_data final {
public:

	struct Mesh_data_info final {
		Mesh_data_info() noexcept = default;

		Mesh_data_info(size_t vertex_count, size_t base_vertex,
			size_t index_count, size_t index_offset) noexcept :
		vertex_count(vertex_count), base_vertex(base_vertex),
			index_count(index_count), index_offset(index_offset)
		{}

		size_t vertex_count = 0;
		size_t base_vertex = 0;
		size_t index_count = 0;
		size_t index_offset = 0;
	};


	using Format = Vertex_interleaved_format_new<attribs>;

	using Vertex = Model_geometry_vertex<attribs>;


	Model_geometry_data(size_t mesh_count);


	size_t mesh_count() const noexcept
	{
		return _meshes.size();
	}

	const std::vector<Mesh_data_info>& meshes() const noexcept
	{
		return _meshes;
	}

	size_t vertex_count() const noexcept
	{
		return _vertex_data.size() / Format::vertex_byte_count;
	}

	const std::vector<unsigned char>& vertex_data() const noexcept
	{
		return _vertex_data;
	}

	size_t vertex_data_byte_count() const noexcept
	{
		return _vertex_data.size();
	}

	size_t index_count() const noexcept
	{
		return _index_data.size();
	}

	const std::vector<uint32_t>& index_data() const noexcept
	{
		return _index_data;
	}

	size_t index_data_byte_count() const noexcept
	{
		return _index_data.size() * sizeof(uint32_t);
	}

	void push_back_mesh(size_t vertex_count, size_t base_vertex, 
		size_t index_count, size_t index_offset);

	void push_back_index(uint32_t index);

	void push_back_indices(uint32_t i0, uint32_t i1, uint32_t i2);

	void push_back_vertex(const Vertex& vertex);

private:
	std::vector<Mesh_data_info> _meshes;
	std::vector<unsigned char> _vertex_data;
	std::vector<uint32_t> _index_data;
};

template<Vertex_attribs_new attribs>
Model_geometry_data<attribs>::Model_geometry_data(size_t mesh_count)
{
	_meshes.reserve(mesh_count);
}

template<Vertex_attribs_new attribs>
void Model_geometry_data<attribs>::push_back_mesh(size_t vertex_count, size_t base_vertex,
	size_t index_count, size_t index_offset)
{
	assert(vertex_count > 0);
	assert(index_count > 0);

	size_t vd_req_len = vertex_count * Format::vertex_byte_count + _vertex_data.size();
	_vertex_data.reserve(vd_req_len);
	_index_data.reserve(_index_data.size() + index_count);

	_meshes.emplace_back(vertex_count, base_vertex, index_count, index_offset);
}

template<Vertex_attribs_new attribs>
void Model_geometry_data<attribs>::push_back_index(uint32_t index)
{
	_index_data.push_back(index);
}

template<Vertex_attribs_new attribs>
void Model_geometry_data<attribs>::push_back_indices(uint32_t i0, uint32_t i1, uint32_t i2)
{
	_index_data.push_back(i0);
	_index_data.push_back(i1);
	_index_data.push_back(i2);
}

template<Vertex_attribs_new attribs>
void Model_geometry_data<attribs>::push_back_vertex(const Vertex& vertex)
{
	_vertex_data.insert(_vertex_data.cend(), std::cbegin(vertex.data), std::cend(vertex.data));
}


std::ostream& operator<<(std::ostream& out, const Vertex_attribs_new& attribs);

std::wostream& operator<<(std::wostream& out, const Vertex_attribs_new& attribs);

// Computes orthogonalized tangent and bitangent's handedness.
// All vectors have to be normalized.
// Returns: vector of 4 floats, xyz stands for the tangent & w stands for the handedness value.
cg::float4 compute_tangent_handedness(const cg::float3& tangent,
	const cg::float3& bitangent, const cg::float3& normal) noexcept;

template<Vertex_attribs_new attribs>
Model_geometry_data<attribs> load_model(const char* filename);

template<Vertex_attribs_new attribs>
inline Model_geometry_data<attribs> load_model(const std::string& filename)
{
	return load_model<attribs>(filename.c_str());
}


} // namespace data
} // namespace cg

#endif // CG_DATA_MODEL_H_

