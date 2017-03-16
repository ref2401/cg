#ifndef CG_DATA_MODEL_H_
#define CG_DATA_MODEL_H_

#include <cassert>
#include <cstdint>
#include <iterator>
#include <ostream>
#include <type_traits>
#include <vector>
#include "cg/data/vertex.h"
#include "cg/math/math.h"


namespace cg {
namespace data {

template<Vertex_attribs attribs>
struct Model_geometry_vertex;

template<> 
struct Model_geometry_vertex<Vertex_attribs::p> final {

	Model_geometry_vertex() noexcept {}

	Model_geometry_vertex(const cg::float3& position) noexcept :
		position(position)
	{}

	union {
		struct {
			cg::float3 position;
		};

		unsigned char data[Vertex_interleaved_format<Vertex_attribs::p>::vertex_byte_count];
	};
};

template<>
struct Model_geometry_vertex<Vertex_attribs::p_n> final {

	Model_geometry_vertex() noexcept {}

	Model_geometry_vertex(const cg::float3& position, const cg::float3& normal) noexcept :
		position(position), normal(normal)
	{}

	union {
		struct {
			cg::float3 position;
			cg::float3 normal;
		};

		unsigned char data[Vertex_interleaved_format<Vertex_attribs::p_n>::vertex_byte_count];
	};
};

template<>
struct Model_geometry_vertex<Vertex_attribs::p_n_tc> final {

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

		unsigned char data[Vertex_interleaved_format<Vertex_attribs::p_n_tc>::vertex_byte_count];
	};
};

template<>
struct Model_geometry_vertex<Vertex_attribs::p_tc> final {

	Model_geometry_vertex() noexcept {}

	Model_geometry_vertex(const cg::float3& position, const cg::float2& tex_coord) noexcept :
		position(position), tex_coord(tex_coord)
	{}

	union {
		struct {
			cg::float3 position;
			cg::float2 tex_coord;
		};

		unsigned char data[Vertex_interleaved_format<Vertex_attribs::p_tc>::vertex_byte_count];
	};
};

template<>
struct Model_geometry_vertex<Vertex_attribs::p_n_tc_ts> final {
	
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

		unsigned char data[Vertex_interleaved_format<Vertex_attribs::p_n_tc_ts>::vertex_byte_count];
	};
};

// Model_mesh_info stores all the necessary info that is used to draw a single mesh.
struct Model_mesh_info final {
	Model_mesh_info() noexcept = default;

	Model_mesh_info(size_t vertex_count, size_t base_vertex, size_t index_count, size_t index_offset) noexcept :
		vertex_count(vertex_count), base_vertex(base_vertex),
		index_count(index_count), index_offset(index_offset)
	{}

	size_t vertex_count = 0;
	size_t base_vertex = 0;
	size_t index_count = 0;
	size_t index_offset = 0;
};

// Any part of a model may require different material instance or different
// renderer pipeline settings. Such parts of a model are called a mesh.
// 
template<Vertex_attribs attribs>
class Model_geometry_data final {
public:

	using Format = Vertex_interleaved_format<attribs>;

	using Vertex = Model_geometry_vertex<attribs>;


	Model_geometry_data() = default;

	explicit Model_geometry_data(size_t mesh_count);


	size_t mesh_count() const noexcept
	{
		return _meshes.size();
	}

	size_t vertex_count() const noexcept
	{
		return _vertex_data.size() / Format::vertex_byte_count;
	}

	size_t index_count() const noexcept
	{
		return _index_data.size();
	}

	const std::vector<Model_mesh_info>& meshes() const noexcept
	{
		return _meshes;
	}

	const std::vector<unsigned char>& vertex_data() const noexcept
	{
		return _vertex_data;
	}

	const std::vector<uint32_t>& index_data() const noexcept
	{
		return _index_data;
	}

	size_t vertex_data_byte_count() const noexcept
	{
		return _vertex_data.size();
	}

	size_t index_data_byte_count() const noexcept
	{
		return _index_data.size() * sizeof(uint32_t);
	}

	void push_back_mesh(size_t vertex_count, size_t base_vertex, 
		size_t index_count, size_t index_offset);

	void push_back_index(uint32_t index);

	void push_back_indices(uint32_t i0, uint32_t i1, uint32_t i2);

	template<typename Container>
	void push_back_indices(const Container& container);

	template<size_t count>
	void push_back_indices(const uint32_t(&indices)[count]);

	template<typename Input_iterator>
	void push_back_indices(Input_iterator b, Input_iterator e);

	void push_back_vertex(const Vertex& vertex);

private:

	std::vector<Model_mesh_info> _meshes;
	std::vector<unsigned char> _vertex_data;
	std::vector<uint32_t> _index_data;
};

template<Vertex_attribs attribs>
Model_geometry_data<attribs>::Model_geometry_data(size_t mesh_count)
{
	_meshes.reserve(mesh_count);
}

template<Vertex_attribs attribs>
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

template<Vertex_attribs attribs>
void Model_geometry_data<attribs>::push_back_index(uint32_t index)
{
	_index_data.push_back(index);
}

template<Vertex_attribs attribs>
void Model_geometry_data<attribs>::push_back_indices(uint32_t i0, uint32_t i1, uint32_t i2)
{
	_index_data.push_back(i0);
	_index_data.push_back(i1);
	_index_data.push_back(i2);
}

template<Vertex_attribs attribs>
template<typename Container>
void Model_geometry_data<attribs>::push_back_indices(const Container& container)
{
	_index_data.insert(_index_data.end(), container.cbegin(), container.cend());
}

template<Vertex_attribs attribs>
template<size_t count>
void Model_geometry_data<attribs>::push_back_indices(const uint32_t(&indices)[count])
{
	_index_data.insert(_index_data.end(), std::cbegin(indices), std::cend(indices));
}

template<Vertex_attribs attribs>
template<typename Input_iterator>
void Model_geometry_data<attribs>::push_back_indices(Input_iterator b, Input_iterator e)
{
	_index_data.insert(_index_data.end(), b, e);
}

template<Vertex_attribs attribs>
void Model_geometry_data<attribs>::push_back_vertex(const Vertex& vertex)
{
	_vertex_data.insert(_vertex_data.cend(), std::cbegin(vertex.data), std::cend(vertex.data));
}


inline bool operator==(const Model_mesh_info& l, const Model_mesh_info& r) noexcept
{
	return (l.vertex_count == r.vertex_count)
		&& (l.base_vertex == r.base_vertex)
		&& (l.index_count == r.index_count)
		&& (l.index_offset == r.index_offset);
}

inline bool operator!=(const Model_mesh_info& l, const Model_mesh_info& r) noexcept
{
	return !(l == r);
}

std::ostream& operator<<(std::ostream& o, const Model_mesh_info& mi);

std::wostream& operator<<(std::wostream& o, const Model_mesh_info& mi);

template<Vertex_attribs attribs>
Model_geometry_data<attribs> load_model(const char* filename);

template<Vertex_attribs attribs>
inline Model_geometry_data<attribs> load_model(const std::string& filename)
{
	return load_model<attribs>(filename.c_str());
}


} // namespace data
} // namespace cg

#endif // CG_DATA_MODEL_H_

