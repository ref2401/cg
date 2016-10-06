#include "cg/sys/input.h"

#include "CppUnitTest.h"

using cg::sys::Mouse;
using cg::sys::Mouse_buttons;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace Microsoft { namespace VisualStudio { namespace CppUnitTestFramework {

template<> inline std::wstring ToString<Mouse_buttons>(const Mouse_buttons& t) { RETURN_WIDE_STRING(t); }

}}} // namespace Microsoft::VisualStudio::CppUnitTestFramework


namespace unittest {

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
};

} // namespace unittest