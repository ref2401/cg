#ifndef TECHNIQUE_FUR_SIMULATION_FUR_MODEL_H_
#define TECHNIQUE_FUR_SIMULATION_FUR_MODEL_H_

#include <initializer_list>
#include <utility>
#include <vector>
#include "cg/data/image.h"
#include "cg/data/model.h"
#include "cg/math/math.h"


namespace fur_simulation {

class Model_rnd_params final {
public:

	Model_rnd_params() noexcept = default;

	Model_rnd_params(float length, float mass, float k, size_t mesh_count) noexcept;

	Model_rnd_params(const Model_rnd_params&) = delete;

	Model_rnd_params(Model_rnd_params&& m) noexcept;


	Model_rnd_params& operator=(const Model_rnd_params&) = delete;

	Model_rnd_params& operator=(Model_rnd_params&& m) noexcept;


	// Spring coefficient of restitution
	float k() const noexcept
	{
		return _k;
	}

	// Length of strands in model space
	float length() const noexcept
	{
		return _length;
	}

	// Strands' tips mass
	float mass() const noexcept
	{
		return _mass;
	}

	const std::vector<cg::data::Model_mesh_info>& meshes() const noexcept
	{
		return _meshes;
	}

	void push_back_mesh(size_t vertex_count, size_t base_vertex,
		size_t index_count, size_t index_offset);

private:

	std::vector<cg::data::Model_mesh_info> _meshes;
	float _length = 0.0f;
	float _mass = 0.0f;
	float _k = 1.0f;
};

class Model_geometry_data_1 final {
public:

	Model_geometry_data_1() noexcept = default;

	Model_geometry_data_1(const Model_geometry_data_1&) = delete;

	Model_geometry_data_1(Model_geometry_data_1&&) noexcept;


	Model_geometry_data_1& operator=(const Model_geometry_data_1&) = delete;

	Model_geometry_data_1& operator=(Model_geometry_data_1&& m) noexcept;


	const std::vector<uint32_t> index_buffer() const noexcept
	{
		return _index_buffer;
	}

	const std::vector<float>& physics_input_buffer() const noexcept
	{
		return _physics_input_buffer;
	}
	
	const std::vector<float>& physics_output_buffer() const noexcept
	{
		return _physics_output_buffer;
	}
	
	const std::vector<float>& render_buffer() const noexcept
	{
		return _render_buffer;
	}

	// p0 is stored in physics_input_buffer
	size_t p0_byte_offset() const noexcept
	{
		return 0;
	}

	// p1 is stored in physics_input_buffer
	size_t p1_byte_offset() const noexcept
	{
		return p0_byte_offset() + sizeof(float) * 4; // 3 (p0) + 1 (mass)
	}

	// p2 is stored in physics_output_buffer
	size_t p2_byte_offset() const noexcept
	{
		return 0;
	}

	// normal is stored in render_buffer
	size_t normal_byte_offset() const noexcept
	{
		return 0;
	}

	// tex_coord is stored in render_buffer
	size_t tex_coord_byte_offset() const noexcept
	{
		return normal_byte_offset() + sizeof(float) * 3;
	}

	// tangent_h is strored in render_buffer
	size_t tangent_h_byte_offset() const noexcept
	{
		return tex_coord_byte_offset() + sizeof(float) * 2;
	}


	size_t physics_input_buffer_byte_count() const noexcept
	{
		return sizeof(float) * _physics_input_buffer.size();
	}

	size_t physics_input_buffer_byte_stride() const noexcept
	{
		return sizeof(float) * 8; // 3(p0) + 1(mass) + 3(p1) + 1(k)
	}

	size_t physics_output_buffer_byte_count() const noexcept
	{
		return sizeof(float) * _physics_output_buffer.size();
	}

	size_t physics_output_buffer_byte_stride() const noexcept
	{
		return sizeof(float) * 3; // 3(p2)
	}

	size_t render_buffer_byte_count() const noexcept
	{
		return sizeof(float) * _render_buffer.size();
	}
	
	size_t render_buffer_byte_stride() const noexcept
	{
		return sizeof(float) * 9; // 3(normal) + 2(tex_coord) + 4(tangent_h)
	}

	size_t index_buffer_byte_count() const noexcept
	{
		return sizeof(uint32_t) * _index_buffer.size();
	}



	void push_back_physics_input(const cg::float3& p0, float mass, const cg::float3& p1, float k);

	void push_back_physics_output(const cg::float3& p2);

	void push_back_render(const cg::float3& normal, const cg::float2& tex_coord, const cg::float4& tangent_h);

	void push_back_indices(uint32_t i0, uint32_t i1, uint32_t i2);

private:

	// p0.x, p0.y, p0.z, mass, p1.x, p1.y, p1.z, k
	std::vector<float> _physics_input_buffer;
	// p2.x, p2.y, p2.z
	std::vector<float> _physics_output_buffer;
	// normal, tex_coord, tangent
	std::vector<float> _render_buffer;
	std::vector<uint32_t> _index_buffer;
};

std::pair<Model_geometry_data_1, Model_rnd_params> load_fur_model(
	float length, float mass, float k, const char* geometry_filename);


struct Vertex final {
	Vertex(const cg::data::Model_geometry_vertex<cg::data::Vertex_attribs::p_n_tc_ts>& vertex,
		float length, const cg::float3& strand_curr_direction) noexcept;

	Vertex(const cg::float3& position, const cg::float3& normal, 
		const cg::float2& tex_coord, const cg::float4& tangent_h,
		const cg::float3& strand_curr_direction) noexcept;


	cg::float3 position;
	cg::float3 strand_rest_position;
	cg::float3 strand_curr_position;
	cg::float3 normal;
	cg::float2 tex_coord;
	cg::float4 tangent_h;
};

class Model_geometry_data final {
public:

	Model_geometry_data(const std::vector<Vertex>& vertices, 
		std::initializer_list<uint32_t> indices);

	Model_geometry_data(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);

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

	size_t tangent_h_byte_offset() const noexcept
	{
		return _tangent_h_byte_offset;
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

	Model_geometry_data(const std::vector<Vertex>& vertices);


	std::vector<float> _vertex_data;
	std::vector<uint32_t> _index_data;
	size_t _vertex_byte_count;
	size_t _position_byte_offset;
	size_t _normal_byte_offset;
	size_t _tex_coord_byte_offset;
	size_t _tangent_h_byte_offset;
	size_t _strand_rest_position_byte_offset;
	size_t _strand_curr_position_byte_offset;
};

class Arbitrary_model final {
public:

	Arbitrary_model(float fur_length, const char* filename);

	Arbitrary_model(float fur_length, const std::string& filename) 
		: Arbitrary_model(fur_length, filename.c_str()) 
	{}

	Model_geometry_data get_geometry_data() const;

private:

	std::vector<Vertex> _vertices;
	std::vector<uint32_t> _indices;
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

