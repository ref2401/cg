#include "cg/rnd/opengl/buffer.h"

#include "CppUnitTest.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace Microsoft { namespace VisualStudio { namespace CppUnitTestFramework {

//template<> inline std::wstring ToString<long long>(const long long& t) { RETURN_WIDE_STRING(t); }
//template<> inline std::wstring ToString<com_fake_class>(const com_fake_class& t) { RETURN_WIDE_STRING(t); }

}}} // namespace Microsoft::VisualStudio::CppUnitTestFramework

namespace unittest {


TEST_CLASS(cg_rnd_opengl_buffer_Funcs) {
public:

	TEST_METHOD(is_opengl_buffer)
	{
		using cg::rnd::opengl::is_opengl_buffer;

		Assert::IsTrue(is_opengl_buffer<cg::rnd::opengl::Buffer>::value);
		Assert::IsTrue(is_opengl_buffer<cg::rnd::opengl::Buffer_immut>::value);
		Assert::IsTrue(is_opengl_buffer<cg::rnd::opengl::Buffer_persistent_map>::value);

		Assert::IsFalse(is_opengl_buffer<size_t>::value);
		Assert::IsFalse(is_opengl_buffer<GLenum>::value);
	}
};

} // namespace unittest
