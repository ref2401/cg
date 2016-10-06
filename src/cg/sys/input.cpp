#include "cg/sys/input.h"


namespace cg {
namespace sys {

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