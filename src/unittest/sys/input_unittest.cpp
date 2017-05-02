#include "cg/sys/input.h"

#include "cg/base/math.h"
#include "CppUnitTest.h"

using cg::sys::Key;
using cg::sys::Key_state;
using cg::sys::Keyboard;
using cg::sys::Mouse;
using cg::sys::Mouse_buttons;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace Microsoft { namespace VisualStudio { namespace CppUnitTestFramework {

template<> inline std::wstring ToString<float2>(const float2& t) { RETURN_WIDE_STRING(t); }
template<> inline std::wstring ToString<uint2>(const uint2& t) { RETURN_WIDE_STRING(t); }
template<> inline std::wstring ToString<Key>(const Key& t) { RETURN_WIDE_STRING(t); }
template<> inline std::wstring ToString<Key_state>(const Key_state& t) { RETURN_WIDE_STRING(t); }
template<> inline std::wstring ToString<Mouse_buttons>(const Mouse_buttons& t) { RETURN_WIDE_STRING(t); }

}}} // namespace Microsoft::VisualStudio::CppUnitTestFramework


namespace unittest {

TEST_CLASS(cg_sys_Keyboard) {
public:

	TEST_METHOD(ctors)
	{
		Keyboard kb;
		Assert::IsTrue(kb.is_up(Key::ctrl_left));
		Assert::IsTrue(kb.is_up(Key::arrow_up));
		Assert::IsTrue(kb.is_up(Key::unknown));
	}

	TEST_METHOD(is_down_up_set_state)
	{
		Keyboard kb;
		Assert::IsTrue(kb.is_up(Key::ctrl_left));
		Assert::IsTrue(kb.is_up(Key::arrow_up));
		Assert::IsTrue(kb.is_up(Key::unknown));

		// ctrl_left
		kb.set_key_state(Key::ctrl_left, Key_state::down);
		Assert::IsTrue(kb.is_down(Key::ctrl_left));
		Assert::IsFalse(kb.is_up(Key::ctrl_left));
		Assert::IsTrue(kb.is_up(Key::arrow_up));
		Assert::IsTrue(kb.is_up(Key::unknown));
		kb.set_key_state(Key::ctrl_left, Key_state::up);

		// arrow_up
		kb.set_key_state(Key::arrow_up, Key_state::down);
		Assert::IsTrue(kb.is_up(Key::ctrl_left));
		Assert::IsTrue(kb.is_down(Key::arrow_up));
		Assert::IsFalse(kb.is_up(Key::arrow_up));
		Assert::IsTrue(kb.is_up(Key::unknown));
		kb.set_key_state(Key::arrow_up, Key_state::up);

		// unknown
		kb.set_key_state(Key::unknown, Key_state::down);
		Assert::IsTrue(kb.is_up(Key::ctrl_left));
		Assert::IsTrue(kb.is_up(Key::arrow_up));
		Assert::IsTrue(kb.is_down(Key::unknown));
		Assert::IsFalse(kb.is_up(Key::unknown));
		kb.set_key_state(Key::unknown, Key_state::up);
	}
};

TEST_CLASS(cg_sys_input_Mouse) {
public:

	TEST_METHOD(buttons)
	{
		Mouse mouse;
		Assert::AreEqual(Mouse_buttons::none, mouse.buttons());
		Assert::IsFalse(mouse.left_down());
		Assert::IsFalse(mouse.middle_down());
		Assert::IsFalse(mouse.right_down());

		mouse.set_buttons(Mouse_buttons::left);
		Assert::AreEqual(Mouse_buttons::left, mouse.buttons());
		Assert::IsTrue(mouse.left_down());
		Assert::IsFalse(mouse.middle_down());
		Assert::IsFalse(mouse.right_down());

		mouse.set_buttons(Mouse_buttons::middle);
		Assert::AreEqual(Mouse_buttons::middle, mouse.buttons());
		Assert::IsFalse(mouse.left_down());
		Assert::IsTrue(mouse.middle_down());
		Assert::IsFalse(mouse.right_down());

		mouse.set_buttons(Mouse_buttons::right);
		Assert::AreEqual(Mouse_buttons::right, mouse.buttons());
		Assert::IsFalse(mouse.left_down());
		Assert::IsFalse(mouse.middle_down());
		Assert::IsTrue(mouse.right_down());

		mouse.set_buttons(Mouse_buttons::left | Mouse_buttons::middle | Mouse_buttons::right);
		Assert::AreEqual(Mouse_buttons::left | Mouse_buttons::middle | Mouse_buttons::right, mouse.buttons());
		Assert::IsTrue(mouse.left_down());
		Assert::IsTrue(mouse.middle_down());
		Assert::IsTrue(mouse.right_down());

		mouse.set_buttons(Mouse_buttons::none);
		Assert::AreEqual(Mouse_buttons::none, mouse.buttons());
		Assert::IsFalse(mouse.left_down());
		Assert::IsFalse(mouse.middle_down());
		Assert::IsFalse(mouse.right_down());
	}

	TEST_METHOD(ctors)
	{
		Mouse mouse;
		Assert::AreEqual(Mouse_buttons::none, mouse.buttons());
		Assert::IsTrue(mouse.is_out());
		Assert::AreEqual(uint2::zero, mouse.position());
	}

	TEST_METHOD(get_ndc_position)
	{
		Mouse mouse;
		mouse.set_is_out(false);

		uint2 wnd_size(61, 7);

		mouse.set_position(uint2::zero);
		Assert::AreEqual(float2(-1.0f), mouse.get_ndc_position(wnd_size));

		mouse.set_position(uint2(30, 3));
		Assert::AreEqual(float2::zero, mouse.get_ndc_position(wnd_size));

		mouse.set_position(uint2(60, 6));
		Assert::AreEqual(float2(1.0f, 1.0f), mouse.get_ndc_position(wnd_size));
	}

	TEST_METHOD(props_get_set)
	{
		Mouse mouse;
		mouse.set_position(uint2(3, 4));
		Assert::AreEqual(uint2(3, 4), mouse.position());
	}
};

TEST_CLASS(cg_sys_input_Mouse_buttons) {
public:

	TEST_METHOD(assignment_operators)
	{
		auto mb = Mouse_buttons::none;
		mb |= Mouse_buttons::left;
		mb |= Mouse_buttons::right;

		Assert::AreEqual(Mouse_buttons::left, mb & Mouse_buttons::left);
		Assert::AreEqual(Mouse_buttons::none, mb & Mouse_buttons::middle);
		Assert::AreEqual(Mouse_buttons::right, mb & Mouse_buttons::right);

		auto origin = Mouse_buttons::left;
		
		mb = origin;
		mb &= Mouse_buttons::left;
		Assert::AreEqual(Mouse_buttons::left, mb & Mouse_buttons::left);
		Assert::AreEqual(Mouse_buttons::none, mb & Mouse_buttons::middle);
		Assert::AreEqual(Mouse_buttons::none, mb & Mouse_buttons::right);

		mb = origin;
		mb &= Mouse_buttons::middle;
		Assert::AreEqual(Mouse_buttons::none, mb & Mouse_buttons::left);
		Assert::AreEqual(Mouse_buttons::none, mb & Mouse_buttons::middle);
		Assert::AreEqual(Mouse_buttons::none, mb & Mouse_buttons::right);

		mb = origin;
		mb &= Mouse_buttons::right;
		Assert::AreEqual(Mouse_buttons::none, mb & Mouse_buttons::left);
		Assert::AreEqual(Mouse_buttons::none, mb & Mouse_buttons::middle);
		Assert::AreEqual(Mouse_buttons::none, mb & Mouse_buttons::right);
	}

	TEST_METHOD(binary_operators)
	{
		auto mb = Mouse_buttons::none;

		Assert::AreEqual(Mouse_buttons::none, (mb & Mouse_buttons::left));
		Assert::AreEqual(Mouse_buttons::none, (mb & Mouse_buttons::middle));
		Assert::AreEqual(Mouse_buttons::none, (mb & Mouse_buttons::right));

		mb = Mouse_buttons::left | Mouse_buttons::middle | Mouse_buttons::right;
		Assert::AreEqual(Mouse_buttons::left, (mb & Mouse_buttons::left));
		Assert::AreEqual(Mouse_buttons::middle, (mb & Mouse_buttons::middle));
		Assert::AreEqual(Mouse_buttons::right, (mb & Mouse_buttons::right));
	}

	TEST_METHOD(unary_operators)
	{
		auto mb = Mouse_buttons::none;
		Assert::AreEqual(Mouse_buttons::left | Mouse_buttons::middle | Mouse_buttons::right, ~mb);

		mb = Mouse_buttons::left;
		Assert::AreEqual(Mouse_buttons::middle | Mouse_buttons::right, ~mb);

		mb = Mouse_buttons::middle;
		Assert::AreEqual(Mouse_buttons::left | Mouse_buttons::right, ~mb);

		mb = Mouse_buttons::right;
		Assert::AreEqual(Mouse_buttons::left | Mouse_buttons::middle, ~mb);

		mb = Mouse_buttons::left | Mouse_buttons::middle | Mouse_buttons::right;
		Assert::AreEqual(Mouse_buttons::none, ~mb);
	}
};

} // namespace unittest