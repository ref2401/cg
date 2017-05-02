#include "cg/sys/input.h"

#include <cassert>
#include <algorithm>
#include <iterator>


namespace cg {
namespace sys {

// ----- Keyboard -----

Keyboard::Keyboard() noexcept
{
	std::fill(std::begin(_key_table), std::end(_key_table), Key_state::up);
}

// ----- Mouse -----

float2 Mouse::get_ndc_position(const uint2& window_size) const noexcept
{
	assert(!_is_out);
	assert(window_size > uint32_t(0));

	float x = 2.0f * float(_position.x) / (window_size.x - 1) - 1.0f;
	float y = 2.0f * float(_position.y) / (window_size.y - 1) - 1.0f;

	return float2(x, y);
}

// ----- funcs -----

std::ostream& operator<<(std::ostream& out, const Key& state) 
{
	out << "Key::";
	switch (state) {
		case Key::alt_left:					out << "alt_left"; break;
		case Key::alt_right:				out << "alt_right"; break;
		case Key::apps:						out << "apps"; break;
		case Key::arrow_down:				out << "arrow_down"; break;
		case Key::arrow_left:				out << "arrow_left"; break;
		case Key::arrow_right:				out << "arrow_right"; break;
		case Key::arrow_up:					out << "arrow_up"; break;
		case Key::backspace:				out << "backspace"; break;
		case Key::caps_lock:				out << "caps_lock"; break;
		case Key::comma:					out << "comma"; break;
		case Key::ctrl_left:				out << "ctrl_left"; break;
		case Key::ctrl_right:				out << "ctrl_right"; break;
		case Key::del:						out << "del"; break;
		case Key::digit_0:					out << "digit_0"; break;
		case Key::digit_1:					out << "digit_1"; break;
		case Key::digit_2:					out << "digit_2"; break;
		case Key::digit_3:					out << "digit_3"; break;
		case Key::digit_4:					out << "digit_4"; break;
		case Key::digit_5:					out << "digit_5"; break;
		case Key::digit_6:					out << "digit_6"; break;
		case Key::digit_7:					out << "digit_7"; break;
		case Key::digit_8:					out << "digit_8"; break;
		case Key::digit_9:					out << "digit_9"; break;
		case Key::end:						out << "end"; break;
		case Key::enter:					out << "enter"; break;
		case Key::escape:					out << "escape"; break;
		case Key::f1:						out << "f1"; break;
		case Key::f2:						out << "f2"; break;
		case Key::f3:						out << "f3"; break;
		case Key::f4:						out << "f4"; break;
		case Key::f5:						out << "f5"; break;
		case Key::f6:						out << "f6"; break;
		case Key::f7:						out << "f7"; break;
		case Key::f8:						out << "f8"; break;
		case Key::f9:						out << "f9"; break;
		case Key::f10:						out << "f10"; break;
		case Key::f11:						out << "f11"; break;
		case Key::f12:						out << "f12"; break;
		case Key::f13:						out << "f13"; break;
		case Key::f14:						out << "f14"; break;
		case Key::f15:						out << "f15"; break;
		case Key::home:						out << "home"; break;
		case Key::insert:					out << "insert"; break;
		case Key::minus:					out << "minus"; break;
		case Key::numpad_0:					out << "numpad_0"; break;
		case Key::numpad_1:					out << "numpad_1"; break;
		case Key::numpad_2:					out << "numpad_2"; break;
		case Key::numpad_3:					out << "numpad_3"; break;
		case Key::numpad_4:					out << "numpad_4"; break;
		case Key::numpad_5:					out << "numpad_5"; break;
		case Key::numpad_6:					out << "numpad_6"; break;
		case Key::numpad_7:					out << "numpad_7"; break;
		case Key::numpad_8:					out << "numpad_8"; break;
		case Key::numpad_9:					out << "numpad_9"; break;
		case Key::numpad_comma:				out << "numpad_comma"; break;
		case Key::numpad_devide:			out << "numpad_devide"; break;
		case Key::num_lock:					out << "num_lock"; break;
		case Key::numpad_minus:				out << "numpad_minus"; break;
		case Key::numpad_multiply:			out << "numpad_multiply"; break;
		case Key::numpad_plus:				out << "numpad_plus"; break;
		case Key::page_down:				out << "page_down"; break;
		case Key::page_up:					out << "page_up"; break;
		case Key::pause:					out << "pause"; break;
		case Key::period:					out << "period"; break;
		case Key::plus:						out << "plus"; break;
		case Key::print:					out << "print"; break;
		case Key::prtsrc:					out << "prtsrc"; break;
		case Key::quote:					out << "quote"; break;
		case Key::quote_back:				out << "quote_back"; break;
		case Key::scroll_lock:				out << "scroll_lock"; break;
		case Key::semicolon:				out << "semicolon"; break;
		case Key::shift_left:				out << "shift_left"; break;
		case Key::shift_right:				out << "shift_right"; break;
		case Key::slash:					out << "slash"; break;
		case Key::slash_back:				out << "slash_back"; break;
		case Key::space:					out << "space"; break;
		case Key::square_bracket_close:		out << "square_bracket_close"; break;
		case Key::square_bracket_open:		out << "square_bracket_open"; break;
		case Key::tab:						out << "tab"; break;
		case Key::win_left:					out << "win_left"; break;
		case Key::win_right:				out << "win_right"; break;
		case Key::a:						out << "a"; break;
		case Key::b:						out << "b"; break;
		case Key::c:						out << "c"; break;
		case Key::d:						out << "d"; break;
		case Key::e:						out << "e"; break;
		case Key::f:						out << "f"; break;
		case Key::g:						out << "g"; break;
		case Key::h:						out << "h"; break;
		case Key::i:						out << "i"; break;
		case Key::j:						out << "j"; break;
		case Key::k:						out << "k"; break;
		case Key::l:						out << "l"; break;
		case Key::m:						out << "m"; break;
		case Key::n:						out << "n"; break;
		case Key::o:						out << "o"; break;
		case Key::p:						out << "p"; break;
		case Key::q:						out << "q"; break;
		case Key::r:						out << "r"; break;
		case Key::s:						out << "s"; break;
		case Key::t:						out << "t"; break;
		case Key::u:						out << "u"; break;
		case Key::v:						out << "v"; break;
		case Key::w:						out << "w"; break;
		case Key::x:						out << "x"; break;
		case Key::y:						out << "y"; break;
		case Key::z:						out << "z"; break;
		case Key::unknown:					out << "unknown"; break;
	}

	out << ";";
	return out;
}

std::wostream& operator<<(std::wostream& out, const Key& state)
{
	out << "Key::";
	switch (state) {
		case Key::alt_left:					out << "alt_left"; break;
		case Key::alt_right:				out << "alt_right"; break;
		case Key::apps:						out << "apps"; break;
		case Key::arrow_down:				out << "arrow_down"; break;
		case Key::arrow_left:				out << "arrow_left"; break;
		case Key::arrow_right:				out << "arrow_right"; break;
		case Key::arrow_up:					out << "arrow_up"; break;
		case Key::backspace:				out << "backspace"; break;
		case Key::caps_lock:				out << "caps_lock"; break;
		case Key::comma:					out << "comma"; break;
		case Key::ctrl_left:				out << "ctrl_left"; break;
		case Key::ctrl_right:				out << "ctrl_right"; break;
		case Key::del:						out << "del"; break;
		case Key::digit_0:					out << "digit_0"; break;
		case Key::digit_1:					out << "digit_1"; break;
		case Key::digit_2:					out << "digit_2"; break;
		case Key::digit_3:					out << "digit_3"; break;
		case Key::digit_4:					out << "digit_4"; break;
		case Key::digit_5:					out << "digit_5"; break;
		case Key::digit_6:					out << "digit_6"; break;
		case Key::digit_7:					out << "digit_7"; break;
		case Key::digit_8:					out << "digit_8"; break;
		case Key::digit_9:					out << "digit_9"; break;
		case Key::end:						out << "end"; break;
		case Key::enter:					out << "enter"; break;
		case Key::escape:					out << "escape"; break;
		case Key::f1:						out << "f1"; break;
		case Key::f2:						out << "f2"; break;
		case Key::f3:						out << "f3"; break;
		case Key::f4:						out << "f4"; break;
		case Key::f5:						out << "f5"; break;
		case Key::f6:						out << "f6"; break;
		case Key::f7:						out << "f7"; break;
		case Key::f8:						out << "f8"; break;
		case Key::f9:						out << "f9"; break;
		case Key::f10:						out << "f10"; break;
		case Key::f11:						out << "f11"; break;
		case Key::f12:						out << "f12"; break;
		case Key::f13:						out << "f13"; break;
		case Key::f14:						out << "f14"; break;
		case Key::f15:						out << "f15"; break;
		case Key::home:						out << "home"; break;
		case Key::insert:					out << "insert"; break;
		case Key::minus:					out << "minus"; break;
		case Key::numpad_0:					out << "numpad_0"; break;
		case Key::numpad_1:					out << "numpad_1"; break;
		case Key::numpad_2:					out << "numpad_2"; break;
		case Key::numpad_3:					out << "numpad_3"; break;
		case Key::numpad_4:					out << "numpad_4"; break;
		case Key::numpad_5:					out << "numpad_5"; break;
		case Key::numpad_6:					out << "numpad_6"; break;
		case Key::numpad_7:					out << "numpad_7"; break;
		case Key::numpad_8:					out << "numpad_8"; break;
		case Key::numpad_9:					out << "numpad_9"; break;
		case Key::numpad_comma:				out << "numpad_comma"; break;
		case Key::numpad_devide:			out << "numpad_devide"; break;
		case Key::num_lock:					out << "num_lock"; break;
		case Key::numpad_minus:				out << "numpad_minus"; break;
		case Key::numpad_multiply:			out << "numpad_multiply"; break;
		case Key::numpad_plus:				out << "numpad_plus"; break;
		case Key::page_down:				out << "page_down"; break;
		case Key::page_up:					out << "page_up"; break;
		case Key::pause:					out << "pause"; break;
		case Key::period:					out << "period"; break;
		case Key::plus:						out << "plus"; break;
		case Key::print:					out << "print"; break;
		case Key::prtsrc:					out << "prtsrc"; break;
		case Key::quote:					out << "quote"; break;
		case Key::quote_back:				out << "quote_back"; break;
		case Key::scroll_lock:				out << "scroll_lock"; break;
		case Key::semicolon:				out << "semicolon"; break;
		case Key::shift_left:				out << "shift_left"; break;
		case Key::shift_right:				out << "shift_right"; break;
		case Key::slash:					out << "slash"; break;
		case Key::slash_back:				out << "slash_back"; break;
		case Key::space:					out << "space"; break;
		case Key::square_bracket_close:		out << "square_bracket_close"; break;
		case Key::square_bracket_open:		out << "square_bracket_open"; break;
		case Key::tab:						out << "tab"; break;
		case Key::win_left:					out << "win_left"; break;
		case Key::win_right:				out << "win_right"; break;
		case Key::a:						out << "a"; break;
		case Key::b:						out << "b"; break;
		case Key::c:						out << "c"; break;
		case Key::d:						out << "d"; break;
		case Key::e:						out << "e"; break;
		case Key::f:						out << "f"; break;
		case Key::g:						out << "g"; break;
		case Key::h:						out << "h"; break;
		case Key::i:						out << "i"; break;
		case Key::j:						out << "j"; break;
		case Key::k:						out << "k"; break;
		case Key::l:						out << "l"; break;
		case Key::m:						out << "m"; break;
		case Key::n:						out << "n"; break;
		case Key::o:						out << "o"; break;
		case Key::p:						out << "p"; break;
		case Key::q:						out << "q"; break;
		case Key::r:						out << "r"; break;
		case Key::s:						out << "s"; break;
		case Key::t:						out << "t"; break;
		case Key::u:						out << "u"; break;
		case Key::v:						out << "v"; break;
		case Key::w:						out << "w"; break;
		case Key::x:						out << "x"; break;
		case Key::y:						out << "y"; break;
		case Key::z:						out << "z"; break;
		case Key::unknown:					out << "unknown"; break;
	}

	out << ";";
	return out;
}

std::ostream& operator<<(std::ostream& out, const Key_state& state)
{
	out << "Key_state";

	switch (state) {
		case Key_state::down:	out << "down"; break;
		case Key_state::up:		out << "up"; break;
	}

	return out;
}

std::wostream& operator<<(std::wostream& out, const Key_state& state)
{
	out << "Key_state";

	switch (state) {
		case Key_state::down:	out << "down"; break;
		case Key_state::up:		out << "up"; break;
	}

	return out;
}


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