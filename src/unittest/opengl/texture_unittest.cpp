#include "cg/opengl/texture.h"

#include "cg/data/image.h"
#include "cg/math/math.h"
#include "CppUnitTest.h"

using cg::uint2;
using cg::data::Image_2d;
using cg::data::Image_format;
using cg::opengl::Texture_2d_sub_image;
using cg::opengl::Texture_format;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace Microsoft { namespace VisualStudio { namespace CppUnitTestFramework {

template<> inline std::wstring ToString<uint2>(const uint2& t) { RETURN_WIDE_STRING(t); }
template<> inline std::wstring ToString<Image_format>(const Image_format& t) { RETURN_WIDE_STRING(t); }
template<> inline std::wstring ToString<Texture_2d_sub_image>(const Texture_2d_sub_image& t) { RETURN_WIDE_STRING(t); }
template<> inline std::wstring ToString<Texture_format>(const Texture_format& t) { RETURN_WIDE_STRING(t); }

}}} // namespace Microsoft::VisualStudio::CppUnitTestFramework


namespace unittest {

TEST_CLASS(cg_opengl_texture_Texture_2d_sub_image) {
public:

	TEST_METHOD(ctors)
	{
		float data[3];
		Texture_2d_sub_image tex_img0(0, uint2::zero, uint2(8, 4), GL_RGB, GL_FLOAT, data);
		Assert::AreEqual<size_t>(0, tex_img0.mipmap_index);
		Assert::AreEqual(uint2::zero, tex_img0.offset);
		Assert::AreEqual(uint2(8, 4), tex_img0.size);
		Assert::AreEqual<GLenum>(GL_RGB, tex_img0.pixel_format);
		Assert::AreEqual<GLenum>(GL_FLOAT, tex_img0.pixel_type);
		Assert::AreEqual<const void*>(data, tex_img0.pixels);

		Image_2d img(uint2(4, 3), Image_format::bgra_8);
		Texture_2d_sub_image tex_img1(1, uint2(1), img);
		Assert::AreEqual<size_t>(1, tex_img1.mipmap_index);
		Assert::AreEqual(uint2(1), tex_img1.offset);
		Assert::AreEqual(uint2(4, 3), tex_img1.size);
		Assert::AreEqual<GLenum>(GL_BGRA, tex_img1.pixel_format);
		Assert::AreEqual<GLenum>(GL_UNSIGNED_BYTE, tex_img1.pixel_type);
		Assert::AreEqual<const void*>(img.data(), tex_img1.pixels);
	}

	TEST_METHOD(equal_operator)
	{
		unsigned char data[3];
		unsigned char data_other[3];
		Texture_2d_sub_image sub_imt(0, uint2(1), uint2(2), GL_RED, GL_UNSIGNED_INT, data);

		Assert::AreNotEqual(sub_imt, Texture_2d_sub_image(100, uint2(1), uint2(2), GL_RED, GL_UNSIGNED_INT, data));
		Assert::AreNotEqual(sub_imt, Texture_2d_sub_image(0, uint2(100), uint2(2), GL_RED, GL_UNSIGNED_INT, data));
		Assert::AreNotEqual(sub_imt, Texture_2d_sub_image(0, uint2(1), uint2(200), GL_RED, GL_UNSIGNED_INT, data));
		Assert::AreNotEqual(sub_imt, Texture_2d_sub_image(0, uint2(1), uint2(2), GL_BGRA, GL_UNSIGNED_INT, data));
		Assert::AreNotEqual(sub_imt, Texture_2d_sub_image(0, uint2(1), uint2(2), GL_RED, GL_FLOAT, data));
		Assert::AreNotEqual(sub_imt, Texture_2d_sub_image(1, uint2(1), uint2(2), GL_RED, GL_UNSIGNED_INT, data_other));

		Assert::AreEqual(sub_imt, Texture_2d_sub_image(0, uint2(1), uint2(2), GL_RED, GL_UNSIGNED_INT, data));
	}
};

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

	TEST_METHOD(texture_internal_format)
	{
		using cg::opengl::texture_internal_format;

		Assert::AreEqual<GLenum>(GL_NONE, texture_internal_format(Texture_format::none));
		Assert::AreEqual<GLenum>(GL_R8, texture_internal_format(Texture_format::red_8));
		Assert::AreEqual<GLenum>(GL_R32F, texture_internal_format(Texture_format::red_32f));
		Assert::AreEqual<GLenum>(GL_RG8, texture_internal_format(Texture_format::rg_8));
		Assert::AreEqual<GLenum>(GL_RG32F, texture_internal_format(Texture_format::rg_32f));
		Assert::AreEqual<GLenum>(GL_RGB8, texture_internal_format(Texture_format::rgb_8));
		Assert::AreEqual<GLenum>(GL_RGB32F, texture_internal_format(Texture_format::rgb_32f));
		Assert::AreEqual<GLenum>(GL_RGBA8, texture_internal_format(Texture_format::rgba_8));
		Assert::AreEqual<GLenum>(GL_RGBA32F, texture_internal_format(Texture_format::rgba_32f));
		Assert::AreEqual<GLenum>(GL_DEPTH_COMPONENT24, texture_internal_format(Texture_format::depth_24));
		Assert::AreEqual<GLenum>(GL_DEPTH24_STENCIL8, texture_internal_format(Texture_format::depth_24_stencil_8));
		Assert::AreEqual<GLenum>(GL_DEPTH_COMPONENT32, texture_internal_format(Texture_format::depth_32));
		Assert::AreEqual<GLenum>(GL_DEPTH_COMPONENT32F, texture_internal_format(Texture_format::depth_32f));
		Assert::AreEqual<GLenum>(GL_DEPTH32F_STENCIL8, texture_internal_format(Texture_format::depth_32f_stencil_8));
	}

	TEST_METHOD(texture_sub_image_format)
	{
		using cg::opengl::texture_sub_image_format;

		Assert::AreEqual<GLenum>(GL_NONE, texture_sub_image_format(Image_format::none));
		Assert::AreEqual<GLenum>(GL_RED, texture_sub_image_format(Image_format::red_8));
		Assert::AreEqual<GLenum>(GL_RGB, texture_sub_image_format(Image_format::rgb_8));
		Assert::AreEqual<GLenum>(GL_RGBA, texture_sub_image_format(Image_format::rgba_8));
		Assert::AreEqual<GLenum>(GL_BGR, texture_sub_image_format(Image_format::bgr_8));
		Assert::AreEqual<GLenum>(GL_BGRA, texture_sub_image_format(Image_format::bgra_8));
	}

	TEST_METHOD(texture_sub_image_type)
	{
		using cg::opengl::texture_sub_image_type;

		Assert::AreEqual<GLenum>(GL_NONE, texture_sub_image_type(Image_format::none));
		Assert::AreEqual<GLenum>(GL_UNSIGNED_BYTE, texture_sub_image_type(Image_format::red_8));
		Assert::AreEqual<GLenum>(GL_UNSIGNED_BYTE, texture_sub_image_type(Image_format::rgb_8));
		Assert::AreEqual<GLenum>(GL_UNSIGNED_BYTE, texture_sub_image_type(Image_format::rgba_8));
		Assert::AreEqual<GLenum>(GL_UNSIGNED_BYTE, texture_sub_image_type(Image_format::bgr_8));
		Assert::AreEqual<GLenum>(GL_UNSIGNED_BYTE, texture_sub_image_type(Image_format::bgra_8));
	}
};

} // namespace unittest