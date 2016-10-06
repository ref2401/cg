#ifndef CG_SYS_INPUT_H_
#define CG_SYS_INPUT_H_

#include <ostream>
#include <type_traits>
#include "cg/math/math.h"


namespace cg {
namespace sys {

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


class Mouse final {
public:

	Mouse() noexcept = default;

	Mouse(const Mouse&) = delete;

	Mouse(Mouse&&) noexcept = delete;


	// The state of all the mouse's buttons.
	const Mouse_buttons& buttons() const noexcept
	{
		return _buttons;
	}

	void set_buttons(const Mouse_buttons mb) noexcept
	{
		_buttons = mb;
	}

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


std::ostream& operator<<(std::ostream& out, const Mouse_buttons& mb);

std::wostream& operator<<(std::wostream& out, const Mouse_buttons& mb);

} // namespace sys
} // namespace cg

#endif // CG_SYS_INPUT_H_
