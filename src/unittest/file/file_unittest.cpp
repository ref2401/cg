#include "cg/file/file.h"

#include "cg/data/shader.h"
#include "unittest/file/common_file.h"


namespace unittest {

TEST_CLASS(cg_file_Funcs) {
public:

	TEST_METHOD(load_glsl_program_source)
	{
		using cg::data::Shader_program_source_code;
		using cg::file::load_glsl_program_source;
		using cg::file::load_text;

		auto src0 = load_glsl_program_source("unknown_file");
		Assert::IsTrue(src0.vertex_source.empty());
		Assert::IsTrue(src0.pixel_source.empty());

		auto expected_vertex_source = load_text(Filenames::not_real_vertex_glsl);
		auto expected_pixel_source = load_text(Filenames::not_real_pixel_glsl);
		auto src1 = load_glsl_program_source(Filenames::not_real_shader_program_name);
		Assert::AreEqual(expected_vertex_source, src1.vertex_source);
		Assert::AreEqual(expected_pixel_source, src1.pixel_source);

		// separate filname specification
		auto src2 = load_glsl_program_source("unknown_file0", "unknown_file1");
		Assert::IsTrue(src2.vertex_source.empty());
		Assert::IsTrue(src2.pixel_source.empty());

		auto src30 = load_glsl_program_source(Filenames::not_real_vertex_glsl, "");
		Assert::AreEqual(expected_vertex_source, src30.vertex_source);
		Assert::IsTrue(src30.pixel_source.empty());

		auto src31 = load_glsl_program_source("", Filenames::not_real_pixel_glsl);
		Assert::IsTrue(src31.vertex_source.empty());
		Assert::AreEqual(expected_pixel_source, src31.pixel_source);

		auto src4 = load_glsl_program_source(Filenames::not_real_vertex_glsl, Filenames::not_real_pixel_glsl);
		Assert::AreEqual(expected_vertex_source, src4.vertex_source);
		Assert::AreEqual(expected_pixel_source, src4.pixel_source);
	}

};

} // namespace unittest
