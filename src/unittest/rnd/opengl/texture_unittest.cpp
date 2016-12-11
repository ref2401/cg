#include "cg/rnd/opengl/texture.h"

#include "CppUnitTest.h"

using cg::rnd::opengl::Sampler_desc;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace Microsoft { namespace VisualStudio { namespace CppUnitTestFramework {

template<> inline std::wstring ToString<Sampler_desc>(const Sampler_desc& t) { RETURN_WIDE_STRING(t); }
//template<> inline std::wstring ToString<Com_fake_class>(const Com_fake_class& t) { RETURN_WIDE_STRING(t); }

}}} // namespace Microsoft::VisualStudio::CppUnitTestFramework


namespace unittest {

TEST_CLASS(cg_rnd_opengl_texture_Sampler_desc) {
public:

	TEST_METHOD(ctors)
	{
		Sampler_desc desc0;
		Assert::AreEqual<GLenum>(GL_LINEAR, desc0.min_filter);
		Assert::AreEqual<GLenum>(GL_LINEAR, desc0.mag_filter);
		Assert::AreEqual<GLenum>(GL_CLAMP_TO_EDGE, desc0.wrap_s);
		Assert::AreEqual<GLenum>(GL_CLAMP_TO_EDGE, desc0.wrap_t);
		Assert::AreEqual<GLenum>(GL_CLAMP_TO_EDGE, desc0.wrap_r);

		Sampler_desc desc1(GL_LINEAR_MIPMAP_LINEAR, GL_NEAREST, GL_REPEAT);
		Assert::AreEqual<GLenum>(GL_LINEAR_MIPMAP_LINEAR, desc1.min_filter);
		Assert::AreEqual<GLenum>(GL_NEAREST, desc1.mag_filter);
		Assert::AreEqual<GLenum>(GL_REPEAT, desc1.wrap_s);
		Assert::AreEqual<GLenum>(GL_REPEAT, desc1.wrap_t);
		Assert::AreEqual<GLenum>(GL_REPEAT, desc1.wrap_r);

		Sampler_desc desc2(GL_NEAREST_MIPMAP_NEAREST, GL_LINEAR,
			GL_REPEAT, GL_MIRRORED_REPEAT, GL_CLAMP_TO_BORDER);
		Assert::AreEqual<GLenum>(GL_NEAREST_MIPMAP_NEAREST, desc2.min_filter);
		Assert::AreEqual<GLenum>(GL_LINEAR, desc2.mag_filter);
		Assert::AreEqual<GLenum>(GL_REPEAT, desc2.wrap_s);
		Assert::AreEqual<GLenum>(GL_MIRRORED_REPEAT, desc2.wrap_t);
		Assert::AreEqual<GLenum>(GL_CLAMP_TO_BORDER, desc2.wrap_r);
	}

	TEST_METHOD(equal_operator)
	{
		Sampler_desc desc(GL_NEAREST, GL_NEAREST, GL_REPEAT, GL_REPEAT, GL_REPEAT);

		Assert::AreNotEqual(desc, Sampler_desc(GL_LINEAR, GL_NEAREST, GL_REPEAT, GL_REPEAT, GL_REPEAT));
		Assert::AreNotEqual(desc, Sampler_desc(GL_NEAREST, GL_LINEAR, GL_REPEAT, GL_REPEAT, GL_REPEAT));
		Assert::AreNotEqual(desc, Sampler_desc(GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_REPEAT, GL_REPEAT));
		Assert::AreNotEqual(desc, Sampler_desc(GL_NEAREST, GL_NEAREST, GL_REPEAT, GL_CLAMP_TO_EDGE, GL_REPEAT));
		Assert::AreNotEqual(desc, Sampler_desc(GL_NEAREST, GL_NEAREST, GL_REPEAT, GL_REPEAT, GL_CLAMP_TO_EDGE));
		
		Assert::AreEqual(desc, Sampler_desc(GL_NEAREST, GL_NEAREST, GL_REPEAT, GL_REPEAT, GL_REPEAT));
	}
};

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

	TEST_METHOD(is_valid_texture_mag_filter)
	{
		using cg::rnd::opengl::is_valid_texture_mag_filter;

		Assert::IsTrue(is_valid_texture_mag_filter(GL_NEAREST));
		Assert::IsTrue(is_valid_texture_mag_filter(GL_LINEAR));

		Assert::IsFalse(is_valid_texture_mag_filter(GL_NONE));
		Assert::IsFalse(is_valid_texture_mag_filter(GL_NEAREST_MIPMAP_NEAREST));
	}

	TEST_METHOD(is_valid_texture_min_filter)
	{
		using cg::rnd::opengl::is_valid_texture_min_filter;

		Assert::IsTrue(is_valid_texture_min_filter(GL_NEAREST));
		Assert::IsTrue(is_valid_texture_min_filter(GL_LINEAR));
		Assert::IsTrue(is_valid_texture_min_filter(GL_NEAREST_MIPMAP_NEAREST));
		Assert::IsTrue(is_valid_texture_min_filter(GL_LINEAR_MIPMAP_NEAREST));
		Assert::IsTrue(is_valid_texture_min_filter(GL_NEAREST_MIPMAP_LINEAR));
		Assert::IsTrue(is_valid_texture_min_filter(GL_LINEAR_MIPMAP_LINEAR));

		Assert::IsFalse(is_valid_texture_min_filter(GL_NONE));
		Assert::IsFalse(is_valid_texture_min_filter(GL_REPEAT));
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
