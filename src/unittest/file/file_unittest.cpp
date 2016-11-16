#include "cg/file/file.h"

#include "cg/data/shader.h"
#include "unittest/file/common_file.h"

using cg::data::Shader_program_source_code;


namespace unittest {

TEST_CLASS(cg_file_Funcs) {
public:

	TEST_METHOD(load_glsl_program_source)
	{
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
	}

	TEST_METHOD(load_hlsl_program_source)
	{
		using cg::file::load_hlsl_program_source;
		using cg::file::load_text;

		auto src0 = load_hlsl_program_source("unknown_file");
		Assert::IsTrue(src0.vertex_source.empty());
		Assert::IsTrue(src0.pixel_source.empty());

		auto expected_vertex_source = load_text(Filenames::not_real_vertex_hlsl);
		auto expected_pixel_source = load_text(Filenames::not_real_pixel_hlsl);
		auto src1 = load_hlsl_program_source(Filenames::not_real_shader_program_name);
		Assert::AreEqual(expected_vertex_source, src1.vertex_source);
		Assert::AreEqual(expected_pixel_source, src1.pixel_source);
	}

	TEST_METHOD(load_shader_program_source)
	{
		using cg::file::load_shader_program_source;
		using cg::file::load_text;

		auto expected_hlsl_vertex_source = load_text(Filenames::not_real_vertex_hlsl);
		auto expected_hlsl_pixel_source = load_text(Filenames::not_real_pixel_hlsl);
		auto expected_glsl_vertex_source = load_text(Filenames::not_real_vertex_glsl);
		auto expected_glsl_pixel_source = load_text(Filenames::not_real_pixel_glsl);

		auto src0 = load_shader_program_source("unknown_file0", "unknown_file1");
		Assert::IsTrue(src0.vertex_source.empty());
		Assert::IsTrue(src0.pixel_source.empty());

		auto hsls_src1 = load_shader_program_source(Filenames::not_real_vertex_hlsl, "");
		Assert::AreEqual(expected_hlsl_vertex_source, hsls_src1.vertex_source);
		Assert::IsTrue(hsls_src1.pixel_source.empty());

		auto glsl_src1 = load_shader_program_source("", Filenames::not_real_pixel_glsl);
		Assert::IsTrue(glsl_src1.vertex_source.empty());
		Assert::AreEqual(expected_glsl_pixel_source, glsl_src1.pixel_source);

		// loads hlsl vertex shader code & glsl pixel shader code
		auto src2 = load_shader_program_source(Filenames::not_real_vertex_glsl, Filenames::not_real_pixel_hlsl);
		Assert::AreEqual(expected_glsl_vertex_source, src2.vertex_source);
		Assert::AreEqual(expected_hlsl_pixel_source, src2.pixel_source);
	}
};

} // namespace unittest
