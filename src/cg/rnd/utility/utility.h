#ifndef CG_RND_UTILITY_UTILITY_H_
#define CG_RND_UTILITY_UTILITY_H_

#include <vector>
#include "cg/math/math.h"


namespace cg {
namespace rnd {
namespace utility {

// Generates random vectors within a unit hemisphere.
// The hemisphere is located at (0, 0, 0) and oriendted along the positive OZ axis.
std::vector<cg::float3> generate_hemispherical_sample_kernel(size_t sample_count);

std::vector<cg::float3> generate_sphere_normalized_sample_kernel(size_t sample_count);

} // namespace utility
} // namespace rnd
} // namespace cg

#endif // CG_RND_UTILITY_UTILITY_H_
