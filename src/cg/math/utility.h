#ifndef CG_MATH_UTILITY_H_
#define CG_MATH_UTILITY_H_

#include <cassert>
#include <cmath>
#include <algorithm>
#include <type_traits>


namespace cg {

// Determines whether lhs is approximately equal to rhs 
// admitting a maximum absolute difference max_abs.
// If Numeric is an integral value then max_abs doesn't matter.
template<typename Numeric>
bool approx_equal(Numeric lhs, Numeric rhs, Numeric max_abs = 1e-5f);

// Clamps v into the given bounds [lo, hi].
template<typename Numeric>
Numeric clamp(const Numeric& v, const Numeric& lo, const Numeric& hi);



namespace internal {

// Approx_equal_helper resolvses to different operator() overloads
// which depends on whether the Numeric type is floating point or intergral.
// If Numeric is neither an integer nor a floating point value then compilation fails.
template<typename Numeric,
	bool is_floating_point = std::is_floating_point<Numeric>::value,
	bool is_integral = std::is_integral<Numeric>::value>
struct Approx_equal_helper;

template<typename Numeric>
struct Approx_equal_helper<Numeric, false, true> {
	bool operator()(const Numeric& lhs, const Numeric& rhs, const Numeric& max_abs)
	{
		return lhs == rhs;
	}
};

template<typename Numeric>
struct Approx_equal_helper<Numeric, true, false> {
	bool operator()(const Numeric& lhs, const Numeric& rhs, const Numeric& max_abs)
	{
		assert(std::isfinite(lhs));
		assert(std::isfinite(rhs));
		assert(std::isfinite(max_abs));

		return std::abs(lhs - rhs) <= max_abs;
	}
};

} // namespace internal


template<typename Numeric>
inline bool approx_equal(Numeric lhs, Numeric rhs, Numeric max_abs)
{
	cg::internal::Approx_equal_helper<Numeric> aeh;
	return aeh(lhs, rhs, max_abs);
}

template<typename Numeric>
inline Numeric clamp(const Numeric& v, const Numeric& lo, const Numeric& hi)
{
	static_assert(std::is_integral<Numeric>::value || std::is_floating_point<Numeric>::value,
		"Numeric type must be an integer or a floating point value.");
	assert(lo <= hi);
	
	return std::min(hi, std::max(lo, v));
}

} // namespace cg

#endif // CG_MATH_UTILITY_H_
