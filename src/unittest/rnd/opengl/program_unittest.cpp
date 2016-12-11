#include "cg/rnd/opengl/program.h"

#include "CppUnitTest.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Microsoft { namespace VisualStudio { namespace CppUnitTestFramework {

//template<> inline std::wstring ToString<long long>(const long long& t) { RETURN_WIDE_STRING(t); }
//template<> inline std::wstring ToString<Com_fake_class>(const Com_fake_class& t) { RETURN_WIDE_STRING(t); }

}}} // namespace Microsoft::VisualStudio::CppUnitTestFramework

namespace unittest {

TEST_CLASS(cg_rnd_opengl_program_Funcs) {
public:

	TEST_METHOD(is_valid_shader_property)
	{
		using cg::rnd::opengl::is_valid_shader_property;

		Assert::IsTrue(is_valid_shader_property(GL_COMPILE_STATUS));
		Assert::IsTrue(is_valid_shader_property(GL_DELETE_STATUS));
		Assert::IsTrue(is_valid_shader_property(GL_INFO_LOG_LENGTH));
		Assert::IsTrue(is_valid_shader_property(GL_SHADER_SOURCE_LENGTH));
		Assert::IsTrue(is_valid_shader_property(GL_SHADER_TYPE));

		Assert::IsFalse(is_valid_shader_property(GL_NONE));
		Assert::IsFalse(is_valid_shader_property(GL_VERTEX_SHADER));
	}

	TEST_METHOD(is_valid_shader_type)
	{
		using cg::rnd::opengl::is_valid_shader_type;

		Assert::IsTrue(is_valid_shader_type(GL_VERTEX_SHADER));
		Assert::IsTrue(is_valid_shader_type(GL_FRAGMENT_SHADER));

		Assert::IsFalse(is_valid_shader_type(GL_NONE));
		Assert::IsFalse(is_valid_shader_type(GL_TEXTURE_2D));
	}
};

} // namespace unittest
