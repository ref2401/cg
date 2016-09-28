#include "cg/data/shader.h"

#include <utility>

#include "CppUnitTest.h"

using cg::data::Shader_program_source_code;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace unittest {

TEST_CLASS(cg_data_shader_Shader_program_source_code) {
public:

	TEST_METHOD(ctors)
	{
		Shader_program_source_code src0;
		Assert::IsTrue(src0.vertex_source.empty());
		Assert::IsTrue(src0.pixel_source.empty());

		std::string vert_srt = "abc";
		std::string pixel_str = "123";
		Shader_program_source_code src1(vert_srt, pixel_str);
		Assert::AreEqual(vert_srt, src1.vertex_source);
		Assert::AreEqual(pixel_str, src1.pixel_source);

		Shader_program_source_code src2("abc", "123");
		Assert::AreEqual("abc", src2.vertex_source.c_str());
		Assert::AreEqual("123", src2.pixel_source.c_str());

		// copy ctor
		Shader_program_source_code src_c = src2;
		Assert::AreEqual(src2.vertex_source, src_c.vertex_source);
		Assert::AreEqual(src2.pixel_source, src_c.pixel_source);

		// move ctor
		Shader_program_source_code src_m = std::move(src_c);
		Assert::AreEqual(src2.vertex_source, src_m.vertex_source);
		Assert::AreEqual(src2.pixel_source, src_m.pixel_source);
		Assert::IsTrue(src_c.vertex_source.empty());
		Assert::IsTrue(src_c.pixel_source.empty());
	}

	TEST_METHOD(assignments)
	{
		Shader_program_source_code src("abc", "123");

		Shader_program_source_code src_c;
		src_c = src;
		Assert::AreEqual(src.vertex_source, src_c.vertex_source);
		Assert::AreEqual(src.pixel_source, src_c.pixel_source);

		Shader_program_source_code src_m;
		src_m = std::move(src_c);
		Assert::AreEqual(src.vertex_source, src_m.vertex_source);
		Assert::AreEqual(src.pixel_source, src_m.pixel_source);
		Assert::IsTrue(src_c.vertex_source.empty());
		Assert::IsTrue(src_c.pixel_source.empty());
	}
};

} // namespace unittest