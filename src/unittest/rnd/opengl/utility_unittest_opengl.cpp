#include "cg/rnd/opengl/utility.h"

#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace unittest {

TEST_CLASS(cg_rnd_opengl_utility_Funcs) {
public:

	TEST_METHOD(is_valid_color_attachment)
	{
		using cg::rnd::opengl::is_valid_color_attachment;

		Assert::IsTrue(is_valid_color_attachment(GL_NONE));
		Assert::IsTrue(is_valid_color_attachment(GL_COLOR_ATTACHMENT0));
		Assert::IsTrue(is_valid_color_attachment(GL_COLOR_ATTACHMENT1));
		Assert::IsTrue(is_valid_color_attachment(GL_COLOR_ATTACHMENT2));
		Assert::IsTrue(is_valid_color_attachment(GL_COLOR_ATTACHMENT3));
		Assert::IsTrue(is_valid_color_attachment(GL_COLOR_ATTACHMENT4));
		Assert::IsTrue(is_valid_color_attachment(GL_COLOR_ATTACHMENT5));
		Assert::IsTrue(is_valid_color_attachment(GL_COLOR_ATTACHMENT6));
		Assert::IsTrue(is_valid_color_attachment(GL_COLOR_ATTACHMENT7));

		Assert::IsFalse(is_valid_color_attachment(GL_RGB));
		Assert::IsFalse(is_valid_color_attachment(GL_TEXTURE0));
		Assert::IsFalse(is_valid_color_attachment(GL_TEXTURE_2D));
	}

	TEST_METHOD(is_valid_texture_sub_image_format)
	{
		using cg::rnd::opengl::is_valid_texture_sub_image_format;

		Assert::IsTrue(is_valid_texture_sub_image_format(GL_RED));
		Assert::IsTrue(is_valid_texture_sub_image_format(GL_RG));
		Assert::IsTrue(is_valid_texture_sub_image_format(GL_RGB));
		Assert::IsTrue(is_valid_texture_sub_image_format(GL_BGR));
		Assert::IsTrue(is_valid_texture_sub_image_format(GL_RGBA));
		Assert::IsTrue(is_valid_texture_sub_image_format(GL_BGRA));
		Assert::IsTrue(is_valid_texture_sub_image_format(GL_DEPTH_COMPONENT));
		Assert::IsTrue(is_valid_texture_sub_image_format(GL_STENCIL_INDEX));

		Assert::IsFalse(is_valid_texture_sub_image_format(GL_R8));
		Assert::IsFalse(is_valid_texture_sub_image_format(GL_RG32F));
		Assert::IsFalse(is_valid_texture_sub_image_format(GL_RGB8UI));
		Assert::IsFalse(is_valid_texture_sub_image_format(GL_DEPTH_COMPONENT32F));
	}

	TEST_METHOD(is_valid_texture_sub_image_type)
	{
		using cg::rnd::opengl::is_valid_texture_sub_image_type;

		Assert::IsTrue(is_valid_texture_sub_image_type(GL_UNSIGNED_BYTE));
		Assert::IsTrue(is_valid_texture_sub_image_type(GL_BYTE));
		Assert::IsTrue(is_valid_texture_sub_image_type(GL_UNSIGNED_SHORT));
		Assert::IsTrue(is_valid_texture_sub_image_type(GL_SHORT));
		Assert::IsTrue(is_valid_texture_sub_image_type(GL_UNSIGNED_INT));
		Assert::IsTrue(is_valid_texture_sub_image_type(GL_INT));
		Assert::IsTrue(is_valid_texture_sub_image_type(GL_FLOAT));
		Assert::IsTrue(is_valid_texture_sub_image_type(GL_UNSIGNED_BYTE_3_3_2));
		Assert::IsTrue(is_valid_texture_sub_image_type(GL_UNSIGNED_BYTE_2_3_3_REV));
		Assert::IsTrue(is_valid_texture_sub_image_type(GL_UNSIGNED_SHORT_5_6_5));
		Assert::IsTrue(is_valid_texture_sub_image_type(GL_UNSIGNED_SHORT_5_6_5_REV));
		Assert::IsTrue(is_valid_texture_sub_image_type(GL_UNSIGNED_SHORT_4_4_4_4));
		Assert::IsTrue(is_valid_texture_sub_image_type(GL_UNSIGNED_SHORT_4_4_4_4_REV));
		Assert::IsTrue(is_valid_texture_sub_image_type(GL_UNSIGNED_SHORT_5_5_5_1));
		Assert::IsTrue(is_valid_texture_sub_image_type(GL_UNSIGNED_SHORT_1_5_5_5_REV));
		Assert::IsTrue(is_valid_texture_sub_image_type(GL_UNSIGNED_INT_8_8_8_8));
		Assert::IsTrue(is_valid_texture_sub_image_type(GL_UNSIGNED_INT_8_8_8_8_REV));
		Assert::IsTrue(is_valid_texture_sub_image_type(GL_UNSIGNED_INT_10_10_10_2));
		Assert::IsTrue(is_valid_texture_sub_image_type(GL_UNSIGNED_INT_2_10_10_10_REV));

		Assert::IsFalse(is_valid_texture_sub_image_type(GL_DEPTH_COMPONENT32));
		Assert::IsFalse(is_valid_texture_sub_image_type(GL_RGB));
		Assert::IsFalse(is_valid_texture_sub_image_type(GL_TEXTURE_2D));
	}
};

} // namespace unittest