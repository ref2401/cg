#include "cg/sys/input.h"

#include <cassert>


namespace cg {
namespace sys {

// ----- Mouse -----

cg::float2 Mouse::get_ndc_position(const cg::uint2& window_size) const noexcept
{
	assert(!_is_out);

	float x = 2.f * float(_position.x) / window_size.width - 1.f;
	float y = 2.f * float(_position.y) / window_size.height - 1.f;

	return float2(x, y);
}

// ----- funcs -----

std::ostream& operator<<(std::ostream& out, const Mouse_buttons& mb)
{
	out << "Mouse_buttons::";
	
	if (mb == Mouse_buttons::none) {
		out << "none";
	}
	else {
		if ((mb & Mouse_buttons::left) == Mouse_buttons::left) out << "left ";
		if ((mb & Mouse_buttons::middle) == Mouse_buttons::middle) out << "middle ";
		if ((mb & Mouse_buttons::right) == Mouse_buttons::right) out << "right";
	}

	out << ')';
	return out;
}

std::wostream& operator<<(std::wostream& out, const Mouse_buttons& mb)
{
	out << "Mouse_buttons::";

	if (mb == Mouse_buttons::none) {
		out << "none";
	}
	else {
		if ((mb & Mouse_buttons::left) == Mouse_buttons::left) out << "left ";
		if ((mb & Mouse_buttons::middle) == Mouse_buttons::middle) out << "middle ";
		if ((mb & Mouse_buttons::right) == Mouse_buttons::right) out << "right";
	}

	out << ')';
	return out;
}

} // namespace sys
} // namespace cg