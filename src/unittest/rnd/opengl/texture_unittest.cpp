#include "cg/rnd/opengl/texture.h"

#include "CppUnitTest.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace Microsoft { namespace VisualStudio { namespace CppUnitTestFramework {

//template<> inline std::wstring ToString<long long>(const long long& t) { RETURN_WIDE_STRING(t); }
//template<> inline std::wstring ToString<Com_fake_class>(const Com_fake_class& t) { RETURN_WIDE_STRING(t); }

}}} // namespace Microsoft::VisualStudio::CppUnitTestFramework


namespace unittest {

TEST_CLASS(cg_rnd_opengl_texture_Funcs) {
public:

	TEST_METHOD(is_valid_texture_internal_format)
	{
		using cg::rnd::opengl::is_valid_texture_internal_format;

		Assert::IsTrue(is_valid_texture_internal_format(GL_R8));
		Assert::IsTrue(is_valid_texture_internal_format(GL_R32F));
		Assert::IsTrue(is_valid_texture_internal_format(GL_RG8));
		Assert::IsTrue(is_valid_texture_internal_format(GL_RG32F));
		Assert::IsTrue(is_valid_texture_internal_format(GL_RGB8));
		Assert::IsTrue(is_valid_texture_internal_format(GL_RGB32F));
		Assert::IsTrue(is_valid_texture_internal_format(GL_RGBA8));
		Assert::IsTrue(is_valid_texture_internal_format(GL_RGBA32F));
		Assert::IsTrue(is_valid_texture_internal_format(GL_DEPTH_COMPONENT24));
		Assert::IsTrue(is_valid_texture_internal_format(GL_DEPTH24_STENCIL8));
		Assert::IsTrue(is_valid_texture_internal_format(GL_DEPTH32F_STENCIL8));
		Assert::IsTrue(is_valid_texture_internal_format(GL_DEPTH_COMPONENT32));
		Assert::IsTrue(is_valid_texture_internal_format(GL_DEPTH_COMPONENT32F));

		Assert::IsFalse(is_valid_texture_internal_format(GL_NONE));
		Assert::IsFalse(is_valid_texture_internal_format(GL_RED));
	}

	TEST_METHOD(is_valid_texture_wrap_mode)
	{
		using cg::rnd::opengl::is_valid_texture_wrap_mode;

		Assert::IsTrue(is_valid_texture_wrap_mode(GL_REPEAT));
		Assert::IsTrue(is_valid_texture_wrap_mode(GL_CLAMP_TO_BORDER));
		Assert::IsTrue(is_valid_texture_wrap_mode(GL_CLAMP_TO_EDGE));
		Assert::IsTrue(is_valid_texture_wrap_mode(GL_MIRRORED_REPEAT));
		Assert::IsTrue(is_valid_texture_wrap_mode(GL_MIRROR_CLAMP_TO_EDGE));

		Assert::IsFalse(is_valid_texture_wrap_mode(GL_NONE));
		Assert::IsFalse(is_valid_texture_wrap_mode(GL_TEXTURE));
	}
};

} // namespace unittest
