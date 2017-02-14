#ifndef CG_MATH_UTILITY_H_
#define CG_MATH_UTILITY_H_

#include <cassert>
#include <cmath>
#include <algorithm>
#include <type_traits>


namespace cg {

namespace internal {

// Approx_equal_helper resolvses to different operator() overloads
// which depends on whether the Numeric type is floating point or intergral.
// If Numeric is neither an integer nor a floating point value then compilation fails.
template<typename Numeric, bool is_floating_point = std::is_floating_point<Numeric>::value>
struct Approx_equal_helper;

template<typename Numeric>
struct Approx_equal_helper<Numeric, false> final {
	bool operator()(const Numeric& lhs, const Numeric& rhs, const Numeric& max_abs_diff) noexcept
	{
		return lhs == rhs;
	}
};

template<typename Numeric>
struct Approx_equal_helper<Numeric, true> final {
	bool operator()(const Numeric& lhs, const Numeric& rhs, const Numeric& max_abs_diff) noexcept
	{
		assert(std::isfinite(lhs));
		assert(std::isfinite(rhs));
		assert(std::isfinite(max_abs_diff));

		return std::abs(lhs - rhs) <= max_abs_diff;
	}
};

} // namespace internal

// Determines whether lhs is approximately equal to rhs 
// admitting a maximum absolute difference max_abs.
// If Numeric is an integral value then max_abs doesn't matter.
template<typename Numeric>
inline bool approx_equal(Numeric lhs, Numeric rhs, Numeric max_abs_diff = 1e-5f) noexcept
{
	cg::internal::Approx_equal_helper<Numeric> aeh;
	return aeh(lhs, rhs, max_abs_diff);
}

// Clamps v into the given bounds [lo, hi].
template<typename Numeric>
inline Numeric clamp(const Numeric& v, const Numeric& lo, const Numeric& hi) noexcept
{
	static_assert(std::is_integral<Numeric>::value || std::is_floating_point<Numeric>::value,
		"Numeric type must be an integer or a floating point value.");
	assert(lo <= hi);
	
	return std::min(hi, std::max(lo, v));
}

// Linearly interpolates between two values.
// Params:
// -	lhs:	The start of the range in which to interpolate.
// -	rhs:	The end of the range in which to interpolate.
// -	factor:	The value to use to interpolate between lhs & rhs.
//				Factor has to lie within the range [0 .. 1].
inline float lerp(const float& lhs, const float& rhs, float factor) noexcept
{
	assert(0.f <= factor && factor <= 1.f);
	return lhs + factor * (rhs - lhs);
}

} // namespace cg

#endif // CG_MATH_UTILITY_H_
