#include "cg/opengl/texture.h"

#include "cg/data/image.h"
#include "CppUnitTest.h"

using cg::data::Image_format;
using cg::opengl::Texture_format;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace Microsoft { namespace VisualStudio { namespace CppUnitTestFramework {

template<> inline std::wstring ToString<Image_format>(const Image_format& t) { RETURN_WIDE_STRING(t); }
template<> inline std::wstring ToString<Texture_format>(const Texture_format& t) { RETURN_WIDE_STRING(t); }

}}} // namespace Microsoft::VisualStudio::CppUnitTestFramework


namespace unittest {

TEST_CLASS(cg_opengl_texture_Funcs) {
public:

	TEST_METHOD(texture_format)
	{
		using cg::opengl::texture_format;

		Assert::AreEqual(Texture_format::none, texture_format(Image_format::none));
		Assert::AreEqual(Texture_format::red_8, texture_format(Image_format::red_8));
		Assert::AreEqual(Texture_format::rgb_8, texture_format(Image_format::rgb_8));
		Assert::AreEqual(Texture_format::rgba_8, texture_format(Image_format::rgba_8));
		Assert::AreEqual(Texture_format::rgb_8, texture_format(Image_format::bgr_8));
		Assert::AreEqual(Texture_format::rgba_8, texture_format(Image_format::bgra_8));
	}

	TEST_METHOD(texture_sub_image_format)
	{
		using cg::opengl::texture_sub_image_format;

		Assert::AreEqual<GLenum>(GL_RED, texture_sub_image_format(Image_format::red_8));
		Assert::AreEqual<GLenum>(GL_RGB, texture_sub_image_format(Image_format::rgb_8));
		Assert::AreEqual<GLenum>(GL_RGBA, texture_sub_image_format(Image_format::rgba_8));
		Assert::AreEqual<GLenum>(GL_BGR, texture_sub_image_format(Image_format::bgr_8));
		Assert::AreEqual<GLenum>(GL_BGRA, texture_sub_image_format(Image_format::bgra_8));
	}

	TEST_METHOD(texture_sub_image_type)
	{
		using cg::opengl::texture_sub_image_type;

		Assert::AreEqual<GLenum>(GL_UNSIGNED_BYTE, texture_sub_image_type(Image_format::red_8));
		Assert::AreEqual<GLenum>(GL_UNSIGNED_BYTE, texture_sub_image_type(Image_format::rgb_8));
		Assert::AreEqual<GLenum>(GL_UNSIGNED_BYTE, texture_sub_image_type(Image_format::rgba_8));
		Assert::AreEqual<GLenum>(GL_UNSIGNED_BYTE, texture_sub_image_type(Image_format::bgr_8));
		Assert::AreEqual<GLenum>(GL_UNSIGNED_BYTE, texture_sub_image_type(Image_format::bgra_8));
	}
};

} // namespace unittest