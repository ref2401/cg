#ifndef CG_SYS_INPUT_H_
#define CG_SYS_INPUT_H_

#include <ostream>
#include <type_traits>

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



class Mouse final {
public:

	Mouse() noexcept = default;

	Mouse(const Mouse&) = delete;

	Mouse(Mouse&&) noexcept = delete;


private:
	Mouse_buttons _buttons = Mouse_buttons::none;
};


std::ostream& operator<<(std::ostream& out, const Mouse_buttons& mb);

std::wostream& operator<<(std::wostream& out, const Mouse_buttons& mb);

} // namespace sys
} // namespace cg

#endif // CG_SYS_INPUT_H_
