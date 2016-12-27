#ifndef CG_SYS_INPUT_H_
#define CG_SYS_INPUT_H_

#include <ostream>
#include <type_traits>
#include "cg/math/math.h"


namespace cg {
namespace sys {

enum class Key {
	alt_left,
	alt_right,
	// Menu key. Microsoft natural keyboard.
	apps,
	arrow_down,
	arrow_left,
	arrow_right,
	arrow_up,
	backspace,
	caps_lock,
	// ",<"
	comma,
	ctrl_left,
	ctrl_right,
	del,
	digit_0,
	digit_1,
	digit_2,
	digit_3,
	digit_4,
	digit_5,
	digit_6,
	digit_7,
	digit_8,
	digit_9,
	end,
	enter,
	escape,
	f1,
	f2,
	f3,
	f4,
	f5,
	f6,
	f7,
	f8,
	f9,
	f10,
	f11,
	f12,
	f13,
	f14,
	f15,
	home,
	insert,
	// -_
	minus,
	numpad_0,
	numpad_1,
	numpad_2,
	numpad_3,
	numpad_4,
	numpad_5,
	numpad_6,
	numpad_7,
	numpad_8,
	numpad_9,
	numpad_comma,
	numpad_devide,
	num_lock,
	numpad_minus,
	numpad_multiply,
	numpad_plus,
	page_down,
	page_up,
	pause,
	// .>
	period,
	// =+
	plus,
	print,
	prtsrc,
	// '"
	quote,
	// `~
	quote_back,
	scroll_lock,
	// for us ";:"
	semicolon,
	shift_left,
	shift_right,
	// for us "\|"
	slash,
	// "/?"
	slash_back,
	space,
	// "]}"
	square_bracket_close,
	// [{
	square_bracket_open,
	tab,
	// Windows key. Microsoft natural keyboard.
	win_left,
	// Windows key. Microsoft natural keyboard.
	win_right,
	a,
	b,
	c,
	d,
	e,
	f,
	g,
	h,
	i,
	j,
	k,
	l,
	m,
	n,
	o,
	p,
	q,
	r,
	s,
	t,
	u,
	v,
	w,
	x,
	y,
	z,
	unknown
};

enum class Key_state : unsigned char {
	up,
	down
};

// Specifies constants that define which mouse button was pressed.
enum class Mouse_buttons : unsigned char {
	none = 0,
	left = 1,
	middle = 2,
	right = 4
};

constexpr Mouse_buttons operator|(Mouse_buttons l, Mouse_buttons r) noexcept
{
	using Val_t = std::underlying_type<Mouse_buttons>::type;
	return static_cast<Mouse_buttons>(static_cast<Val_t>(l) | static_cast<Val_t>(r));
}

constexpr Mouse_buttons operator&(Mouse_buttons l, Mouse_buttons r) noexcept
{
	using Val_t = std::underlying_type<Mouse_buttons>::type;
	return static_cast<Mouse_buttons>(static_cast<Val_t>(l) & static_cast<Val_t>(r));
}

inline Mouse_buttons& operator|=(Mouse_buttons& l, Mouse_buttons r) noexcept
{
	l = l | r;
	return l;
}

inline Mouse_buttons& operator&=(Mouse_buttons& l, Mouse_buttons r) noexcept
{
	l = l & r;
	return l;
}

inline Mouse_buttons operator~(Mouse_buttons mb) noexcept
{
	auto res = Mouse_buttons::none;

	if ((mb & Mouse_buttons::left) == Mouse_buttons::none)
		res |= Mouse_buttons::left;

	if ((mb & Mouse_buttons::middle) == Mouse_buttons::none)
		res |= Mouse_buttons::middle;

	if ((mb & Mouse_buttons::right) == Mouse_buttons::none)
		res |= Mouse_buttons::right;

	return res;
}


class Keyboard final {
public:

	Keyboard() noexcept;

	Keyboard(const Keyboard&) = delete;

	Keyboard(Keyboard&&) = delete;


	Keyboard& operator=(const Keyboard) = delete;

	Keyboard& operator=(Keyboard&&) = delete;


	// Indicates whether the specified key is down (pressed).
	bool is_down(const Key& key) const noexcept 
	{
		return (_key_table[static_cast<size_t>(key)] == Key_state::down);
	}

	// Indicates whether the specified key is up (released).
	bool is_up(const Key& key) const noexcept 
	{
		return (_key_table[static_cast<size_t>(key)] == Key_state::up);
	}


	void set_key_state(const Key& key, const Key_state& state) noexcept
	{
		_key_table[static_cast<size_t>(key)] = state;
	}

private:

	Key_state _key_table[static_cast<size_t>(Key::unknown) + 1];
};

class Mouse final {
public:

	Mouse() noexcept = default;

	Mouse(const Mouse&) = delete;

	Mouse(Mouse&&) noexcept = delete;


	Mouse& operator=(const Mouse&) = delete;

	Mouse& operator=(Mouse&&) = delete;


	// The state of all the mouse's buttons.
	// The value is undefined if is_out() returns true.
	const Mouse_buttons& buttons() const noexcept
	{
		return _buttons;
	}

	void set_buttons(const Mouse_buttons mb) noexcept
	{
		_buttons = mb;
	}

	// Calculates mouse's position as normalized device coordinates.
	cg::float2 get_ndc_position(const cg::uint2& window_size) const noexcept;

	// True if cursor left the client area of the window.
	bool is_out() const noexcept
	{
		return _is_out;
	}

	void set_is_out(bool out) noexcept
	{
		_is_out = out;
	}

	// true if left mouse button has been pressed.
	bool left_down() const noexcept
	{
		return (_buttons & Mouse_buttons::left) == Mouse_buttons::left;
	}

	// true if middle mouse button has been pressed.
	bool middle_down() const noexcept 
	{
		return (_buttons & Mouse_buttons::middle) == Mouse_buttons::middle;
	}

	// true if right mouse button has been pressed.
	bool right_down() const noexcept
	{
		return (_buttons & Mouse_buttons::right) == Mouse_buttons::right;
	}

	// Mouse position within the window's client area. 
	// The value is relative to the bottom-left corner.
	// The value is undefined if is_out() returns true.
	const cg::uint2& position() const noexcept
	{
		return _position;
	}

	void set_position(const cg::uint2& position) noexcept
	{
		_position = position;
	}

private:
	cg::uint2 _position = cg::uint2::zero;
	Mouse_buttons _buttons = Mouse_buttons::none;
	bool _is_out = true;
};


std::ostream& operator<<(std::ostream& out, const Key& state);

std::wostream& operator<<(std::wostream& out, const Key& state);

std::ostream& operator<<(std::ostream& out, const Key_state& state);

std::wostream& operator<<(std::wostream& out, const Key_state& state);

std::ostream& operator<<(std::ostream& out, const Mouse_buttons& mb);

std::wostream& operator<<(std::wostream& out, const Mouse_buttons& mb);

} // namespace sys
} // namespace cg

#endif // CG_SYS_INPUT_H_
