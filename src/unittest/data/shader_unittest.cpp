#include "cg/data/shader.h"

#include <algorithm>
#include <iterator>
#include <utility>
#include "cg/data/file.h"
#include "unittest/data/common_file.h"
#include "CppUnitTest.h"

using cg::data::Glsl_compute_desc;
using cg::data::Glsl_program_desc;
using cg::data::Hlsl_shader_desc;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace unittest {

TEST_CLASS(cg_data_shader_Glsl_compute_data) {
public:

	TEST_METHOD(ctors)
	{
		Glsl_compute_desc cd0;
		Assert::IsTrue(cd0.compute_shader_source_code.empty());

		std::string name = "cs-name";
		std::string compute_srt = "abc";
		Glsl_compute_desc cd1(name, compute_srt);
		Assert::AreEqual(name, cd1.name);
		Assert::AreEqual(compute_srt, cd1.compute_shader_source_code);

		Glsl_compute_desc cd2(name.c_str(), compute_srt.c_str());
		Assert::AreEqual(name.c_str(), cd2.name.c_str());
		Assert::AreEqual(compute_srt.c_str(), cd2.compute_shader_source_code.c_str());

		// copy ctor
		Glsl_compute_desc cd_c = cd2;
		Assert::AreEqual(cd2.name, cd_c.name);
		Assert::AreEqual(cd2.compute_shader_source_code, cd_c.compute_shader_source_code);

		// move ctor
		Glsl_compute_desc cd_m = std::move(cd_c);
		Assert::AreEqual(cd2.name, cd_m.name);
		Assert::AreEqual(cd2.compute_shader_source_code, cd_m.compute_shader_source_code);
		Assert::IsTrue(cd_c.name.empty());
		Assert::IsTrue(cd_c.compute_shader_source_code.empty());
	}

	TEST_METHOD(assignments)
	{
		Glsl_compute_desc cd("cs-name", "abc");

		Glsl_compute_desc cd_c;
		cd_c = cd;
		Assert::AreEqual(cd.name, cd_c.name);
		Assert::AreEqual(cd.compute_shader_source_code, cd_c.compute_shader_source_code);

		Glsl_compute_desc cd_m;
		cd_m = std::move(cd_c);
		Assert::AreEqual(cd.compute_shader_source_code, cd_m.compute_shader_source_code);
		Assert::IsTrue(cd_c.name.empty());
		Assert::IsTrue(cd_c.compute_shader_source_code.empty());
	}
};

TEST_CLASS(cg_data_shader_Glsl_program_desc) {
public:

	TEST_METHOD(ctors)
	{
		Glsl_program_desc pd0;
		Assert::IsTrue(pd0.vertex_shader_source_code.empty());
		Assert::IsTrue(pd0.fragment_shader_source_code.empty());

		pd0.name = "awesome-shader-name";
		pd0.vertex_shader_source_code = "abc";
		pd0.fragment_shader_source_code = "123";
		pd0.tf_varying_names.push_back("out_var_0");
		pd0.tf_interleaved_buffer_mode = true;

		// copy ctor
		Glsl_program_desc pd_c = pd0;
		Assert::AreEqual(pd0.name, pd_c.name);
		Assert::AreEqual(pd0.vertex_shader_source_code, pd_c.vertex_shader_source_code);
		Assert::AreEqual(pd0.fragment_shader_source_code, pd_c.fragment_shader_source_code);
		Assert::AreEqual(pd0.tf_interleaved_buffer_mode, pd_c.tf_interleaved_buffer_mode);
		Assert::IsTrue(std::equal(pd0.tf_varying_names.cbegin(), pd0.tf_varying_names.cend(), pd_c.tf_varying_names.cbegin()));

		// move ctor
		Glsl_program_desc pd_m = std::move(pd_c);
		Assert::AreEqual(pd0.name, pd_m.name);
		Assert::AreEqual(pd0.vertex_shader_source_code, pd_m.vertex_shader_source_code);
		Assert::AreEqual(pd0.fragment_shader_source_code, pd_m.fragment_shader_source_code);
		Assert::AreEqual(pd0.tf_interleaved_buffer_mode, pd_m.tf_interleaved_buffer_mode);
		Assert::IsTrue(std::equal(pd0.tf_varying_names.cbegin(), pd0.tf_varying_names.cend(), pd_m.tf_varying_names.cbegin()));
		Assert::IsTrue(pd_c.vertex_shader_source_code.empty());
		Assert::IsTrue(pd_c.fragment_shader_source_code.empty());
		Assert::IsTrue(pd_c.tf_varying_names.empty());
	}

	TEST_METHOD(assignments)
	{
		Glsl_program_desc pd;
		pd.name = "awesome-shader-name";
		pd.vertex_shader_source_code = "abc";
		pd.fragment_shader_source_code = "123";
		pd.tf_varying_names.push_back("out_var_0");
		pd.tf_interleaved_buffer_mode = true;

		Glsl_program_desc pd_c;
		pd_c = pd;
		Assert::AreEqual(pd.name, pd_c.name);
		Assert::AreEqual(pd.vertex_shader_source_code, pd_c.vertex_shader_source_code);
		Assert::AreEqual(pd.fragment_shader_source_code, pd_c.fragment_shader_source_code);
		Assert::AreEqual(pd.tf_interleaved_buffer_mode, pd_c.tf_interleaved_buffer_mode);
		Assert::IsTrue(std::equal(pd.tf_varying_names.cbegin(), pd.tf_varying_names.cend(), pd_c.tf_varying_names.cbegin()));


		Glsl_program_desc pd_m;
		pd_m = std::move(pd_c);
		Assert::AreEqual(pd.name, pd_m.name);
		Assert::AreEqual(pd.vertex_shader_source_code, pd_m.vertex_shader_source_code);
		Assert::AreEqual(pd.fragment_shader_source_code, pd_m.fragment_shader_source_code);
		Assert::AreEqual(pd.tf_interleaved_buffer_mode, pd_m.tf_interleaved_buffer_mode);
		Assert::IsTrue(std::equal(pd.tf_varying_names.cbegin(), pd.tf_varying_names.cend(), pd_m.tf_varying_names.cbegin()));
		Assert::IsTrue(pd_c.vertex_shader_source_code.empty());
		Assert::IsTrue(pd_c.fragment_shader_source_code.empty());
		Assert::IsTrue(pd_c.tf_varying_names.empty());
	}

	TEST_METHOD(has_xxx_shader)
	{
		Glsl_program_desc hd;
		Assert::IsFalse(hd.has_vertex_shader());
		Assert::IsFalse(hd.has_fragment_shader());

		hd.vertex_shader_source_code = "void main() {}";
		Assert::IsTrue(hd.has_vertex_shader());

		hd.fragment_shader_source_code = "void main() {}";
		Assert::IsTrue(hd.has_fragment_shader());
	}
};

TEST_CLASS(cg_data_shader_Hlsl_shader_set_data) {
public:

	TEST_METHOD(assignment_operators)
	{
		Hlsl_shader_desc hd0;
		hd0.source_code = "abc;";
		hd0.source_filename = ".txt";
		hd0.vertex_shader_entry_point = "vs";
		hd0.pixel_shader_entry_point = "ps";
		hd0.compile_flags = 42;

		// copy assignment
		Hlsl_shader_desc hd_c;
		hd_c = hd0;
		Assert::AreEqual(hd_c.source_code, hd0.source_code);
		Assert::AreEqual(hd_c.source_filename, hd0.source_filename);
		Assert::AreEqual(hd_c.vertex_shader_entry_point, hd0.vertex_shader_entry_point);
		Assert::AreEqual(hd_c.pixel_shader_entry_point, hd0.pixel_shader_entry_point);
		Assert::AreEqual(hd_c.compile_flags, hd0.compile_flags);

		// move assignment
		Hlsl_shader_desc hd_m;
		hd_m = std::move(hd_c);
		Assert::AreEqual(hd_m.source_code, hd0.source_code);
		Assert::AreEqual(hd_m.source_filename, hd0.source_filename);
		Assert::AreEqual(hd_m.vertex_shader_entry_point, hd0.vertex_shader_entry_point);
		Assert::AreEqual(hd_m.pixel_shader_entry_point, hd0.pixel_shader_entry_point);
		Assert::AreEqual(hd_m.compile_flags, hd0.compile_flags);
		Assert::IsTrue(hd_c.source_code.empty());
		Assert::IsTrue(hd_c.source_filename.empty());
		Assert::IsTrue(hd_c.vertex_shader_entry_point.empty());
		Assert::IsTrue(hd_c.pixel_shader_entry_point.empty());
		//Assert::AreEqual<uint32_t>(0, hd_c.compile_flags); // default move assignment
	}

	TEST_METHOD(ctors)
	{
		Hlsl_shader_desc hd0;
		Assert::IsTrue(hd0.source_code.empty());
		Assert::IsTrue(hd0.source_filename.empty());
		Assert::IsTrue(hd0.vertex_shader_entry_point.empty());
		Assert::IsTrue(hd0.pixel_shader_entry_point.empty());
		Assert::AreEqual<uint32_t>(0, hd0.compile_flags);

		hd0.source_code = "abc;";
		hd0.source_filename = ".txt";
		hd0.vertex_shader_entry_point = "vs";
		hd0.pixel_shader_entry_point = "ps";
		hd0.compile_flags = 42;

		// copy ctor
		Hlsl_shader_desc hd_c = hd0;
		Assert::AreEqual(hd_c.source_code, hd0.source_code);
		Assert::AreEqual(hd_c.source_filename, hd0.source_filename);
		Assert::AreEqual(hd_c.vertex_shader_entry_point, hd0.vertex_shader_entry_point);
		Assert::AreEqual(hd_c.pixel_shader_entry_point, hd0.pixel_shader_entry_point);
		Assert::AreEqual(hd_c.compile_flags, hd0.compile_flags);

		// move ctor
		Hlsl_shader_desc hd_m = std::move(hd_c);
		Assert::AreEqual(hd_m.source_code, hd0.source_code);
		Assert::AreEqual(hd_m.source_filename, hd0.source_filename);
		Assert::AreEqual(hd_m.vertex_shader_entry_point, hd0.vertex_shader_entry_point);
		Assert::AreEqual(hd_m.pixel_shader_entry_point, hd0.pixel_shader_entry_point);
		Assert::AreEqual(hd_m.compile_flags, hd0.compile_flags);
		Assert::IsTrue(hd_c.source_code.empty());
		Assert::IsTrue(hd_c.source_filename.empty());
		Assert::IsTrue(hd_c.vertex_shader_entry_point.empty());
		Assert::IsTrue(hd_c.pixel_shader_entry_point.empty());
		//Assert::AreEqual<uint32_t>(0, hd_c.compile_flags); // default move ctor
	}

	TEST_METHOD(has_xxx_shader)
	{
		Hlsl_shader_desc hd;
		Assert::IsFalse(hd.has_vertex_shader());
		Assert::IsFalse(hd.has_pixel_shader());

		hd.vertex_shader_entry_point = "vs";
		Assert::IsTrue(hd.has_vertex_shader());

		hd.pixel_shader_entry_point = "ps";
		Assert::IsTrue(hd.has_pixel_shader());
	}
};

TEST_CLASS(cg_data_shader_Funcs) {
public:

	TEST_METHOD(load_glsl_compute_desc)
	{
		using cg::data::load_glsl_compute_desc;
		using cg::data::load_text;

		Assert::ExpectException<std::exception&>([] { load_glsl_compute_desc("name", "unknown_file"); });

		const std::string expected_name("name");
		auto expected_compute_source = load_text(Filenames::not_real_compute_glsl);

		auto cd = load_glsl_compute_desc(expected_name, Filenames::not_real_compute_glsl);
		Assert::AreEqual(expected_name, cd.name);
		Assert::AreEqual(expected_compute_source, cd.compute_shader_source_code);
	}

	TEST_METHOD(load_glsl_program_desc_single_filename)
	{
		using cg::data::load_glsl_program_desc;
		using cg::data::load_text;

		const std::string name("name");
		const auto expected_vertex_source = load_text(Filenames::not_real_vertex_glsl);
		const auto expected_fragment_source = load_text(Filenames::not_real_fragment_glsl);
		const auto expected_single_vertex_source = load_text(Filenames::not_real_single_vertex_glsl);

		Assert::ExpectException<std::exception&>([] { load_glsl_program_desc("name", "unknown_file"); });
		
		// without transform feedback
		auto pd0 = load_glsl_program_desc(name, Filenames::not_real_glsl_program_name);
		Assert::AreEqual(expected_vertex_source, pd0.vertex_shader_source_code);
		Assert::AreEqual(expected_fragment_source, pd0.fragment_shader_source_code);
		Assert::IsTrue(pd0.tf_interleaved_buffer_mode);
		Assert::IsTrue(pd0.tf_varying_names.empty());

		// vertex shader only, without transform feedback
		auto pd1 = load_glsl_program_desc(name, Filenames::not_real_glsl_single_vertex_program_name);
		Assert::AreEqual(expected_single_vertex_source, pd1.vertex_shader_source_code);
		Assert::IsTrue(pd1.fragment_shader_source_code.empty());
		Assert::IsTrue(pd1.tf_interleaved_buffer_mode);
		Assert::IsTrue(pd1.tf_varying_names.empty());
	}

	TEST_METHOD(load_glsl_program_desc_separate_filenames)
	{
		using cg::data::load_glsl_program_desc;
		using cg::data::load_text;

		Assert::ExpectException<std::exception&>([] { load_glsl_program_desc("name", "unknown_file0", "unknown_file1"); });
		Assert::ExpectException<std::exception&>([] { load_glsl_program_desc("name", Filenames::not_real_vertex_glsl, "unknown_file1"); });
		Assert::ExpectException<std::exception&>([] { load_glsl_program_desc("name", "unknown_file0", Filenames::not_real_fragment_glsl); });
	
		const std::string name("name");
		const auto expected_vertex_source = load_text(Filenames::not_real_vertex_glsl);
		const auto expected_fragment_source = load_text(Filenames::not_real_fragment_glsl);
		const auto expected_single_vertex_source = load_text(Filenames::not_real_single_vertex_glsl);

		// without transform feedback
		auto pd0 = load_glsl_program_desc(name, Filenames::not_real_vertex_glsl, Filenames::not_real_fragment_glsl);
		Assert::AreEqual(expected_vertex_source, pd0.vertex_shader_source_code);
		Assert::AreEqual(expected_fragment_source, pd0.fragment_shader_source_code);
		Assert::IsTrue(pd0.tf_interleaved_buffer_mode);
		Assert::IsTrue(pd0.tf_varying_names.empty());

		// vertex shader only without transform feedback
		auto pd1 = load_glsl_program_desc(name, Filenames::not_real_single_vertex_glsl.c_str(), "");
		Assert::AreEqual(expected_single_vertex_source, pd1.vertex_shader_source_code);
		Assert::IsTrue(pd1.fragment_shader_source_code.empty());
		Assert::IsTrue(pd1.tf_interleaved_buffer_mode);
		Assert::IsTrue(pd1.tf_varying_names.empty());
	}

	TEST_METHOD(load_hlsl_shader_set_desc)
	{
		using cg::data::load_hlsl_shader_set_desc;
		using cg::data::load_text;

		auto expected_source = load_text(Filenames::not_real_code_hlsl);

		Assert::ExpectException<std::exception&>([] { load_hlsl_shader_set_desc("unknown_file0"); });

		auto hd0 = load_hlsl_shader_set_desc(Filenames::not_real_code_hlsl);
		Assert::AreEqual(expected_source, hd0.source_code);
		Assert::AreEqual(Filenames::not_real_code_hlsl, hd0.source_filename);
		Assert::IsTrue(hd0.vertex_shader_entry_point.empty());
		Assert::IsTrue(hd0.pixel_shader_entry_point.empty());
		Assert::AreEqual<uint32_t>(0, hd0.compile_flags);
	}
};

} // namespace unittest

