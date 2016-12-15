#ifndef TECHNIQUE_FUR_SIMULATION_FUR_MODEL_H_
#define TECHNIQUE_FUR_SIMULATION_FUR_MODEL_H_

#include <initializer_list>
#include <vector>
#include "cg/data/image.h"
#include "cg/math/math.h"


namespace fur_simulation {

struct Vertex {
	
	Vertex(const cg::float3& position,
		const cg::float3& normal, const cg::float2& tex_coord) noexcept;

	cg::float3 position;
	cg::float3 strand_rest_position;
	cg::float3 strand_curr_position;
	cg::float3 normal;
	cg::float2 tex_coord;
};

class Model_geometry_data final {
public:

	Model_geometry_data(const std::vector<Vertex>& vertices, std::initializer_list<uint32_t> indices);

	Model_geometry_data(const Model_geometry_data&) = delete;

	Model_geometry_data(Model_geometry_data&&) noexcept = default;


	Model_geometry_data& operator=(const Model_geometry_data&) = delete;

	Model_geometry_data& operator=(Model_geometry_data&&) noexcept = default;


	const std::vector<float>& vertex_data() const noexcept
	{
		return _vertex_data;
	}

	size_t vertex_data_byte_count() const noexcept
	{
		return sizeof(float) * _vertex_data.size();
	
	}

	const std::vector<uint32_t> index_data() const noexcept
	{
		return _index_data;
	}

	size_t index_data_byte_count() const noexcept
	{
		return sizeof(uint32_t) * _index_data.size();
	}

	size_t vertex_byte_count() const noexcept
	{
		return _vertex_byte_count;
	}

	size_t position_byte_offset() const noexcept
	{
		return _position_byte_offset;
	}

	size_t normal_byte_offset() const noexcept
	{
		return _normal_byte_offset;
	}

	size_t tex_coord_byte_offset() const noexcept
	{
		return _tex_coord_byte_offset;
	}

	size_t strand_rest_position_byte_offset() const noexcept
	{
		return _strand_rest_position_byte_offset;
	}

	size_t strand_curr_position_byte_offset() const noexcept
	{
		return _strand_curr_position_byte_offset;
	}

private:

	std::vector<float> _vertex_data;
	std::vector<uint32_t> _index_data;
	size_t _vertex_byte_count;
	size_t _position_byte_offset;
	size_t _normal_byte_offset;
	size_t _tex_coord_byte_offset;
	size_t _strand_rest_position_byte_offset;
	size_t _strand_curr_position_byte_offset;
};

class Square_model final {
public:

	Square_model();


	Model_geometry_data get_geometry_data() const;

private:

	std::vector<Vertex> _vertices;
};

} // fur_simlation

#endif // TECHNIQUE_FUR_SIMULATION_FUR_MODEL_H_

