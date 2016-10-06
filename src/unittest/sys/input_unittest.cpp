#include "cg/sys/input.h"

#include "CppUnitTest.h"

using cg::sys::Mouse_buttons;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace Microsoft { namespace VisualStudio { namespace CppUnitTestFramework {

template<> inline std::wstring ToString<Mouse_buttons>(const Mouse_buttons& t) { RETURN_WIDE_STRING(t); }

}}} // namespace Microsoft::VisualStudio::CppUnitTestFramework


namespace unittest {

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