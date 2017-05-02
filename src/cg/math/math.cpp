#include "cg/math/math.h"



namespace cg {

std::ostream& operator<<(std::ostream& out, const Viewpoint& vp)
{
	out << "Viewpoint(" << vp.position << ", " << vp.target << ", " << vp.up << ')';
	return out;
}

std::wostream& operator<<(std::wostream& out, const Viewpoint& vp)
{
	out << "Viewpoint(" << vp.position << ", " << vp.target << ", " << vp.up << ')';
	return out;
}

Viewpoint lerp(const Viewpoint& l, const Viewpoint& r, float factor) noexcept
{
	assert(0.0f <= factor && factor <= 1.0f);

	return Viewpoint(
		cg::lerp(l.position, r.position, factor),
		cg::lerp(l.target, r.target, factor),
		cg::lerp(l.up, r.up, factor)
	);
}

} // namespace cg
