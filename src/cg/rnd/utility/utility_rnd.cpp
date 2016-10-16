#include "cg/rnd/utility/utility.h"

#include <cassert>
#include <cmath>
#include <random>
#include <functional>

using cg::float3;


namespace cg {
namespace rnd {
namespace utility {

std::vector<float3> generate_hemispherical_sample_kernel(size_t sample_count)
{
	using cg::lerp;
	using cg::normalize;


	assert(sample_count > 0);
	auto rnd = std::bind(std::uniform_real_distribution<float>(0.0f, 1.0f), std::minstd_rand{});

	std::vector<float3> kernel;
	kernel.reserve(sample_count);
	
	for (size_t i = 0; i < sample_count; ++i) {
		float3 dir = normalize(float3(
			rnd() * 2.0f - 1.0f, 
			rnd() * 2.0f - 1.0f,
			rnd())
		);
		
		float factor = std::pow(float(i) / sample_count, 2);
		float scale = lerp(0.1f, 1.0f, factor);
		kernel.push_back(dir * scale);
	}

	return kernel;
}

std::vector<float3> generate_sphere_normalized_sample_kernel(size_t sample_count)
{
	using cg::lerp;
	using cg::normalize;


	assert(sample_count > 0);
	auto rnd = std::bind(std::uniform_real_distribution<float>(0.0f, 1.0f), std::minstd_rand{});

	std::vector<float3> kernel;
	kernel.reserve(sample_count);

	for (size_t i = 0; i < sample_count; ++i) {
		float3 dir = normalize(float3(
			rnd() * 2.0f - 1.0f,
			rnd() * 2.0f - 1.0f,
			rnd() * 2.0f - 1.0f)
		);
		kernel.push_back(dir);
	}

	return kernel;
}


} // namespace utility
} // namespace rnd
} // namespace cg