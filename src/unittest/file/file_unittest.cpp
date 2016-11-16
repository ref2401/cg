#include "cg/file/file.h"

#include "cg/data/shader.h"
#include "unittest/file/common_file.h"

using cg::data::Glsl_program_data;
using cg::data::Hlsl_shader_set_data;


namespace unittest {

TEST_CLASS(cg_file_Funcs) {
public:

	TEST_METHOD(load_glsl_program_data)
	{
		using cg::file::load_glsl_program_data;
		using cg::file::load_text;

		Assert::ExpectException<std::exception&>([] { load_glsl_program_data("unknown_file"); });

		auto expected_vertex_source = load_text(Filenames::not_real_vertex_glsl);
		auto expected_fragment_source = load_text(Filenames::not_real_fragment_glsl);
		auto dt0 = load_glsl_program_data(Filenames::not_real_glsl_program_name);
		Assert::AreEqual(expected_vertex_source, dt0.vertex_shader_source_code);
		Assert::AreEqual(expected_fragment_source, dt0.fragment_shader_source_code);

		// separate filenames
		Assert::ExpectException<std::exception&>([] { load_glsl_program_data("unknown_file0", "unknown_file1"); });
		Assert::ExpectException<std::exception&>([] { load_glsl_program_data(Filenames::not_real_vertex_glsl, "unknown_file1"); });
		Assert::ExpectException<std::exception&>([] { load_glsl_program_data("unknown_file0", Filenames::not_real_fragment_glsl); });

		auto dt1 = load_glsl_program_data(Filenames::not_real_vertex_glsl, Filenames::not_real_fragment_glsl);
		Assert::AreEqual(expected_vertex_source, dt1.vertex_shader_source_code);
		Assert::AreEqual(expected_fragment_source, dt1.fragment_shader_source_code);
	}

	TEST_METHOD(load_hlsl_shader_set_data)
	{
		using cg::file::load_hlsl_shader_set_data;
		using cg::file::load_text;

		auto expected_source = load_text(Filenames::not_real_code_hlsl);

		Assert::ExpectException<std::exception&>([] { load_hlsl_shader_set_data("unknown_file0"); });

		auto hd0 = load_hlsl_shader_set_data(Filenames::not_real_code_hlsl);
		Assert::AreEqual(expected_source, hd0.source_code);
		Assert::AreEqual(Filenames::not_real_code_hlsl, hd0.source_filename);
		Assert::IsTrue(hd0.vertex_shader_entry_point.empty());
		Assert::IsTrue(hd0.pixel_shader_entry_point.empty());
		Assert::AreEqual<uint32_t>(0, hd0.compile_flags);
	}
};

} // namespace unittest
