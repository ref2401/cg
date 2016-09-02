#ifndef CG_MATH_VECTOR_H_
#define CG_MATH_VECTOR_H_

#include <ostream>
#include "cg/math/utils.h"


namespace cg {

struct float_3 {
	static const float_3 zero;
	static const float_3 unit_x;
	static const float_3 unit_y;
	static const float_3 unit_z;
	static const float_3 unit_xyz;


	float_3() = default;

	explicit float_3(float v);

	float_3(float x, float y, float z);


	float x = 0;
	float y = 0;
	float z = 0;
};

inline float_3::float_3(float v) : x(v), y(v), z(v) {}

inline float_3::float_3(float x, float y, float z) : x(x), y(y), z(z) {}

inline bool operator==(const float_3& lhs, const float_3& rhs)
{
	return approx_equal(lhs.x, rhs.x) 
		&& approx_equal(lhs.y, rhs.y)
		&& approx_equal(lhs.z, rhs.z);
}

inline bool operator!=(const float_3& lhs, const float_3& rhs)
{
	return !(lhs == rhs);
}

inline std::ostream& operator<<(std::ostream& out, const cg::float_3& v)
{
	out << "(" << v.x << ", " << v.y << ", " << v.z << ")";
	return out;
}

inline std::wostream& operator<<(std::wostream& out, const cg::float_3& v)
{
	out << "(" << v.x << ", " << v.y << ", " << v.z << ")";
	return out;
}


} // namespace cg

#endif // CG_MATH_VECTOR_H_
