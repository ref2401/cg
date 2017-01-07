#ifndef TECHNIQUE_FUR_SIMULATION_FUR_MODEL_H_
#define TECHNIQUE_FUR_SIMULATION_FUR_MODEL_H_

#include <initializer_list>
#include <utility>
#include <vector>
#include "cg/data/image.h"
#include "cg/data/model.h"
#include "cg/math/math.h"


namespace fur_simulation {

// Fur model vertex format:
// float3 p_base
// float3 p_rest
// float3 p_curr
// float3 velocity
// float3 normal
// float2 tex_coord
// float4 tangent_h
struct Model_geometry_data final {

	Model_geometry_data() = default;

	Model_geometry_data(float strand_length, const char* geometry_filename);

	Model_geometry_data(const Model_geometry_data&) = delete;

	Model_geometry_data(Model_geometry_data&&) noexcept = default;


	Model_geometry_data& operator=(const Model_geometry_data&) = delete;

	Model_geometry_data& operator=(Model_geometry_data&&) noexcept = default;


	// Meshes draw params
	std::vector<cg::data::Model_mesh_info> meshes;

	// p_base, p_rest
	std::vector<float> position_buffer;

	// p_curr, velocity
	std::vector<float> simulation_buffer;

	// normal, tex_coord, tangent_h
	std::vector<float> model_attribs_buffer;

	// indices
	std::vector<uint32_t> index_buffer;

private:

	void push_back_position_buffer_data(const cg::float3& p_base, const cg::float3& p_rest);

	void push_back_simulation_buffer_data(const cg::float3& p_curr, const cg::float3& velocity);

	void push_back_model_attribs_buffer_data(const cg::float3& normal,
		const cg::float2& tex_coord, const cg::float4& tangent_h);

	void push_back_index_buffer_data(uint32_t i0, uint32_t i1, uint32_t i2);
};

} // namespace fur_simulation

#endif // TECHNIQUE_FUR_SIMULATION_FUR_MODEL_H_
