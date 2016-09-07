#ifndef CG_MATH_TRANSFORM_H_
#define CG_MATH_TRANSFORM_H_

#include <type_traits>
#include "cg/math/float3.h"
#include "cg/math/float4.h"
#include "cg/math/mat3.h"
#include "cg/math/mat4.h"
#include "cg/math/quat.h"
#include "cg/math/utils.h"


namespace cg {

namespace internal {

// Checks whether the given type T is float3 or float4 vector.
template<typename T>
struct is_3d_float_vector : std::false_type {};

template<> struct is_3d_float_vector<float3> : std::true_type {};
template<> struct is_3d_float_vector<float4> : std::true_type {};

// Checks whether the given type T is mat3 or mat4 vector.
template<typename T>
struct is_matrix : std::false_type {};
template<> struct is_matrix<mat3> : std::true_type {};
template<> struct is_matrix<mat4> : std::true_type {};

} // namespace internal

} // namespace cg

#endif // CG_MATH_TRANSFORM_H_
