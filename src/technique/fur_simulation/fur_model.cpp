#include "technique/fur_simulation/fur_model.h"

#include <cassert>

using namespace cg;


namespace fur_simulation {

// ----- Vertex -----

Vertex::Vertex(const cg::data::Model_geometry_vertex<cg::data::Vertex_attribs::p_n_tc_ts>& vertex,
	float length, const cg::float3& strand_curr_direction) noexcept
	: position(vertex.position),
	strand_rest_position(position + length * vertex.normal),
	strand_curr_position(position + length * strand_curr_direction),
	normal(vertex.normal),
	tex_coord(vertex.tex_coord),
	tangent_h(vertex.tangent_h)
{ }

Vertex::Vertex(const cg::float3& position, const cg::float3& normal, 
	const cg::float2& tex_coord, const cg::float4& tangent_h,
	const cg::float3& strand_curr_direction) noexcept
	: position(position),
	strand_rest_position(position + 0.3f * normal),
	strand_curr_position(position + 0.3f * strand_curr_direction),
	normal(normal),
	tex_coord(tex_coord),
	tangent_h(tangent_h)
{
	assert(cg::is_normalized(normal));
}

// ----- Model_geometry_data -----

Model_geometry_data::Model_geometry_data(const std::vector<Vertex>& vertices)
{
	assert(vertices.size() > 0);

	// pos, rest_pos, curr_pos normal, tex_coord + tangent_h
	constexpr size_t component_count = 3 + 3 + 3 + 3 + 2 + 4;
	_vertex_byte_count = sizeof(float) * component_count;
	_position_byte_offset = 0;
	_normal_byte_offset = sizeof(float) * 3;
	_tex_coord_byte_offset = _normal_byte_offset + sizeof(float) * 3;
	_tangent_h_byte_offset = _tex_coord_byte_offset + sizeof(float) * 2;
	_strand_rest_position_byte_offset = _tangent_h_byte_offset + sizeof(float) * 4;
	_strand_curr_position_byte_offset = _strand_rest_position_byte_offset + sizeof(float) * 3;

	_vertex_data = std::vector<float>(vertices.size() * component_count);

	size_t data_index = 0;
	for (size_t vi = 0; vi < vertices.size(); ++vi) {
		const auto& vertex = vertices[vi];

		// position
		_vertex_data[data_index++] = vertex.position.x;
		_vertex_data[data_index++] = vertex.position.y;
		_vertex_data[data_index++] = vertex.position.z;
		// normal
		_vertex_data[data_index++] = vertex.normal.x;
		_vertex_data[data_index++] = vertex.normal.y;
		_vertex_data[data_index++] = vertex.normal.z;
		// tex_coord
		_vertex_data[data_index++] = vertex.tex_coord.u;
		_vertex_data[data_index++] = vertex.tex_coord.v;
		// tangent_h
		_vertex_data[data_index++] = vertex.tangent_h.x;
		_vertex_data[data_index++] = vertex.tangent_h.y;
		_vertex_data[data_index++] = vertex.tangent_h.z;
		_vertex_data[data_index++] = vertex.tangent_h.w;
		// strand_rest_position
		_vertex_data[data_index++] = vertex.strand_rest_position.x;
		_vertex_data[data_index++] = vertex.strand_rest_position.y;
		_vertex_data[data_index++] = vertex.strand_rest_position.z;
		// strand_curr_position
		_vertex_data[data_index++] = vertex.strand_curr_position.x;
		_vertex_data[data_index++] = vertex.strand_curr_position.y;
		_vertex_data[data_index++] = vertex.strand_curr_position.z;
	}
}

Model_geometry_data::Model_geometry_data(const std::vector<Vertex>& vertices,
	std::initializer_list<uint32_t> indices)
	: Model_geometry_data(vertices)
{
	assert(indices.size() > 0);
	_index_data.insert(_index_data.cend(), indices);
}

Model_geometry_data::Model_geometry_data(const std::vector<Vertex>& vertices, 
	const std::vector<uint32_t>& indices)
	: Model_geometry_data(vertices)
{
	assert(indices.size() > 0);
	_index_data.insert(_index_data.cend(), indices.cbegin(), indices.cend());
}

// ----- Sphere_model -----

Arbitrary_model::Arbitrary_model(float fur_length, const char* filename)
{
	using cg::data::Vertex_attribs;
	using Model_geometry_data_t = cg::data::Model_geometry_data<Vertex_attribs::p_n_tc_ts>;
	using Vertex_data_t = Model_geometry_data_t::Vertex;

	auto model_data = cg::data::load_model<Model_geometry_data_t::Format::attribs>(filename);

	_vertices.reserve(model_data.vertex_count());
	for (size_t vi = 0; vi < model_data.vertex_count(); ++vi) {
		auto* vd_ptr = &model_data.vertex_data()[vi * sizeof(Vertex_data_t)];
		Vertex_data_t vd = *(reinterpret_cast<const Vertex_data_t*>(vd_ptr));
		
		_vertices.emplace_back(vd, fur_length, vd.normal);
	}

	_indices.insert(_indices.cend(), model_data.index_data().cbegin(), model_data.index_data().cend());
}

Model_geometry_data Arbitrary_model::get_geometry_data() const
{
	return Model_geometry_data(_vertices, _indices);
}

// ----- Square_model -----

Square_model::Square_model()
{
	const float3 origin_normal = float3::unit_z;
	const float3 normal = origin_normal;
	//const float3 normal = normalize(origin_normal + float3(-0.4f, 0.0f, 0));

	_vertices.reserve(4);
	
	// left-bottom
	_vertices.emplace_back(float3(-1, -1, 0), normal, float2::zero, float4(1, 0, 0, 1), normal);
		//normalize(normal + float3(0.2f, 0, 0)));
	// right-bottom
	_vertices.emplace_back(float3(1, -1, 0), normal, float2::unit_x, float4(1, 0, 0, 1), normal);
		//normalize(normal + float3(0.2f, 0, 0)));
	// right-top
	_vertices.emplace_back(float3(1, 1, 0), normal, float2::unit_xy, float4(1, 0, 0, 1), normal);
		//normalize(normal + float3(0.2f, 0, 0)));
	// left-bottom
	_vertices.emplace_back(float3(-1, 1, 0), normal, float2::unit_y, float4(1, 0, 0, 1), normal);
		//normalize(normal + float3(0.2f, 0, 0)));

	//_vertices.emplace_back(float3::zero, normal, tex_coord_factor * float2::zero, float4(1, 0, 0, 1), normal);
}

Model_geometry_data Square_model::get_geometry_data() const
{
	return Model_geometry_data(_vertices, { 0, 1, 2, 2, 3, 0 });
}

} // fur_simlation
