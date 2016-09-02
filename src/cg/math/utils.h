#ifndef CG_MATH_UTILS_H_
#define CG_MATH_UTILS_H_

#include <cassert>
#include <cmath>
#include <type_traits>


namespace cg {

// Determines whether lhs is approximately equal to rhs 
// admitting a maximum absolute difference max_abs.
template<typename T>
inline bool approx_equal(T lhs, T rhs, T max_abs = 1e-5f)
{
	static_assert(std::is_floating_point<T>::value, "T must be a floating point value.");
	assert(std::isfinite(lhs));
	assert(std::isfinite(rhs));
	assert(std::isfinite(max_abs));

	return std::abs(lhs - rhs) <= max_abs;
}


} // namespace cg

#endif // CG_MATH_UTILS_H_
