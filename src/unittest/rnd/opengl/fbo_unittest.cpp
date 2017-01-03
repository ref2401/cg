#include "cg/rnd/opengl/fbo.h"

#include "CppUnitTest.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace Microsoft { namespace VisualStudio { namespace CppUnitTestFramework {

//template<> inline std::wstring ToString<long long>(const long long& t) { RETURN_WIDE_STRING(t); }
//template<> inline std::wstring ToString<Com_fake_class>(const Com_fake_class& t) { RETURN_WIDE_STRING(t); }

}}} // namespace Microsoft::VisualStudio::CppUnitTestFramework


namespace unittest {

TEST_CLASS(cg_rnd_opengl_fbo_Funcs) {
public:

	TEST_METHOD(is_valid_color_attachment)
	{
		using cg::rnd::opengl::is_valid_color_attachment;

		Assert::IsTrue(is_valid_color_attachment(GL_COLOR_ATTACHMENT0));
		Assert::IsTrue(is_valid_color_attachment(GL_COLOR_ATTACHMENT1));
		Assert::IsTrue(is_valid_color_attachment(GL_COLOR_ATTACHMENT2));
		Assert::IsTrue(is_valid_color_attachment(GL_COLOR_ATTACHMENT3));
		Assert::IsTrue(is_valid_color_attachment(GL_COLOR_ATTACHMENT4));
		Assert::IsTrue(is_valid_color_attachment(GL_COLOR_ATTACHMENT5));
		Assert::IsTrue(is_valid_color_attachment(GL_COLOR_ATTACHMENT6));
		Assert::IsTrue(is_valid_color_attachment(GL_COLOR_ATTACHMENT7));

		Assert::IsFalse(is_valid_color_attachment(GL_NONE));
		Assert::IsFalse(is_valid_color_attachment(GL_TEXTURE_2D));
	}
};

} // namespace unittest