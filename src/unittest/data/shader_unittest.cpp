#include "cg/data/shader.h"

#include <algorithm>
#include <utility>
#include "cg/data/file.h"
#include "unittest/data/common_file.h"
#include "CppUnitTest.h"

using cg::data::Glsl_compute_data;
using cg::data::Glsl_program_data;
using cg::data::Hlsl_shader_set_data;
using cg::data::Transform_feedback;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;



namespace Microsoft { namespace VisualStudio { namespace CppUnitTestFramework {

template<> inline std::wstring ToString<Transform_feedback>(const Transform_feedback& t) { RETURN_WIDE_STRING(t); }

}}} // namespace Microsoft::VisualStudio::CppUnitTestFramework


namespace unittest {

TEST_CLASS(cg_data_shader_Glsl_compute_data) {
public:

	TEST_METHOD(ctors)
	{
		Glsl_compute_data cd0;
		Assert::IsTrue(cd0.compute_shader_source_code.empty());

		std::string compute_srt = "abc";
		Glsl_compute_data cd1(compute_srt);
		Assert::AreEqual(compute_srt, cd1.compute_shader_source_code);

		Glsl_compute_data cd2("abc");
		Assert::AreEqual("abc", cd2.compute_shader_source_code.c_str());

		// copy ctor
		Glsl_compute_data cd_c = cd2;
		Assert::AreEqual(cd2.compute_shader_source_code, cd_c.compute_shader_source_code);

		// move ctor
		Glsl_compute_data cd_m = std::move(cd_c);
		Assert::AreEqual(cd2.compute_shader_source_code, cd_m.compute_shader_source_code);
		Assert::IsTrue(cd_c.compute_shader_source_code.empty());
	}

	TEST_METHOD(assignments)
	{
		Glsl_compute_data cd("abc");

		Glsl_compute_data cd_c;
		cd_c = cd;
		Assert::AreEqual(cd.compute_shader_source_code, cd_c.compute_shader_source_code);

		Glsl_compute_data cd_m;
		cd_m = std::move(cd_c);
		Assert::AreEqual(cd.compute_shader_source_code, cd_m.compute_shader_source_code);
		Assert::IsTrue(cd_c.compute_shader_source_code.empty());
	}
};

TEST_CLASS(cg_data_shader_Glsl_program_data) {
public:

	TEST_METHOD(ctors)
	{
		Glsl_program_data pd0;
		Assert::IsTrue(pd0.vertex_shader_source_code.empty());
		Assert::IsTrue(pd0.fragment_shader_source_code.empty());

		pd0.vertex_shader_source_code = "abc";
		pd0.fragment_shader_source_code = "123";

		// copy ctor
		Glsl_program_data pd_c = pd0;
		Assert::AreEqual(pd0.vertex_shader_source_code, pd_c.vertex_shader_source_code);
		Assert::AreEqual(pd0.fragment_shader_source_code, pd_c.fragment_shader_source_code);

		// move ctor
		Glsl_program_data pd_m = std::move(pd_c);
		Assert::AreEqual(pd0.vertex_shader_source_code, pd_m.vertex_shader_source_code);
		Assert::AreEqual(pd0.fragment_shader_source_code, pd_m.fragment_shader_source_code);
		Assert::IsTrue(pd_c.vertex_shader_source_code.empty());
		Assert::IsTrue(pd_c.fragment_shader_source_code.empty());
	}

	TEST_METHOD(assignments)
	{
		Glsl_program_data pd;
		pd.vertex_shader_source_code = "abc";
		pd.fragment_shader_source_code = "123";

		Glsl_program_data pd_c;
		pd_c = pd;
		Assert::AreEqual(pd.vertex_shader_source_code, pd_c.vertex_shader_source_code);
		Assert::AreEqual(pd.fragment_shader_source_code, pd_c.fragment_shader_source_code);

		Glsl_program_data pd_m;
		pd_m = std::move(pd_c);
		Assert::AreEqual(pd.vertex_shader_source_code, pd_m.vertex_shader_source_code);
		Assert::AreEqual(pd.fragment_shader_source_code, pd_m.fragment_shader_source_code);
		Assert::IsTrue(pd_c.vertex_shader_source_code.empty());
		Assert::IsTrue(pd_c.fragment_shader_source_code.empty());
	}

	TEST_METHOD(has_xxx_shader)
	{
		Glsl_program_data hd;
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
		Hlsl_shader_set_data hd0;
		hd0.source_code = "abc;";
		hd0.source_filename = ".txt";
		hd0.vertex_shader_entry_point = "vs";
		hd0.pixel_shader_entry_point = "ps";
		hd0.compile_flags = 42;

		// copy assignment
		Hlsl_shader_set_data hd_c;
		hd_c = hd0;
		Assert::AreEqual(hd_c.source_code, hd0.source_code);
		Assert::AreEqual(hd_c.source_filename, hd0.source_filename);
		Assert::AreEqual(hd_c.vertex_shader_entry_point, hd0.vertex_shader_entry_point);
		Assert::AreEqual(hd_c.pixel_shader_entry_point, hd0.pixel_shader_entry_point);
		Assert::AreEqual(hd_c.compile_flags, hd0.compile_flags);

		// move assignment
		Hlsl_shader_set_data hd_m;
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
		Hlsl_shader_set_data hd0;
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
		Hlsl_shader_set_data hd_c = hd0;
		Assert::AreEqual(hd_c.source_code, hd0.source_code);
		Assert::AreEqual(hd_c.source_filename, hd0.source_filename);
		Assert::AreEqual(hd_c.vertex_shader_entry_point, hd0.vertex_shader_entry_point);
		Assert::AreEqual(hd_c.pixel_shader_entry_point, hd0.pixel_shader_entry_point);
		Assert::AreEqual(hd_c.compile_flags, hd0.compile_flags);

		// move ctor
		Hlsl_shader_set_data hd_m = std::move(hd_c);
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
		Hlsl_shader_set_data hd;
		Assert::IsFalse(hd.has_vertex_shader());
		Assert::IsFalse(hd.has_pixel_shader());

		hd.vertex_shader_entry_point = "vs";
		Assert::IsTrue(hd.has_vertex_shader());

		hd.pixel_shader_entry_point = "ps";
		Assert::IsTrue(hd.has_pixel_shader());
	}
};

TEST_CLASS(cg_data_shader_Transform_feedback) { 
public:

	TEST_METHOD(assignment_operator)
	{
		Transform_feedback tf;
		tf.varying_names.push_back("out_var_0");
		tf.varying_names.push_back("out_var_1");
		tf.interleaved_buffer_mode = true;

		// copy assignment
		Transform_feedback tf_c;
		tf_c = tf;
		Assert::IsTrue(std::equal(tf.varying_names.cbegin(), tf.varying_names.cend(), tf_c.varying_names.cbegin()));
		Assert::AreEqual(tf.interleaved_buffer_mode, tf_c.interleaved_buffer_mode);

		// move assignments
		Transform_feedback tf_m;
		tf_m = std::move(tf_c);
		Assert::IsTrue(std::equal(tf.varying_names.cbegin(), tf.varying_names.cend(), tf_m.varying_names.cbegin()));
		Assert::AreEqual(tf.interleaved_buffer_mode, tf_m.interleaved_buffer_mode);
		Assert::IsTrue(tf_c.varying_names.empty());
		Assert::IsFalse(tf_c.interleaved_buffer_mode);
	}

	TEST_METHOD(ctors)
	{
		Transform_feedback tf;
		Assert::IsTrue(tf.varying_names.empty());
		Assert::IsFalse(tf.interleaved_buffer_mode);
		Assert::IsFalse(tf.is_used());

		tf.varying_names.push_back("out_var_0");
		tf.varying_names.push_back("out_var_1");
		tf.interleaved_buffer_mode = true;

		// copy ctor
		Transform_feedback tf_c = tf;
		Assert::IsTrue(std::equal(tf.varying_names.cbegin(), tf.varying_names.cend(), tf_c.varying_names.cbegin()));
		Assert::AreEqual(tf.interleaved_buffer_mode, tf_c.interleaved_buffer_mode);

		// move ctor
		Transform_feedback tf_m = std::move(tf_c);
		Assert::IsTrue(std::equal(tf.varying_names.cbegin(), tf.varying_names.cend(), tf_m.varying_names.cbegin()));
		Assert::AreEqual(tf.interleaved_buffer_mode, tf_m.interleaved_buffer_mode);
		Assert::IsTrue(tf_c.varying_names.empty());
		Assert::IsFalse(tf_c.interleaved_buffer_mode);
	}

	TEST_METHOD(equal_operator)
	{
		Transform_feedback tf;
		tf.interleaved_buffer_mode = true;
		tf.varying_names.push_back("out_var_0");

		// interleaved_buffer_mode
		Transform_feedback tf_0;
		tf_0.interleaved_buffer_mode = false;
		tf_0.varying_names.push_back("out_var_0");
		Assert::AreNotEqual(tf, tf_0);

		// varying_names
		Transform_feedback tf_1;
		tf_1.interleaved_buffer_mode = true;
		Assert::AreNotEqual(tf, tf_1);

		Transform_feedback tf_2;
		tf_2.interleaved_buffer_mode = true;
		tf_2.varying_names.push_back("out_var_100");
		Assert::AreNotEqual(tf, tf_2);

		// same objects
		Transform_feedback tf_3;
		tf_3.interleaved_buffer_mode = true;
		tf_3.varying_names.push_back("out_var_0");
		Assert::AreEqual(tf, tf_3);
	}

	TEST_METHOD(is_used_property)
	{
		Transform_feedback tf;
		Assert::IsTrue(tf.varying_names.empty());
		Assert::IsFalse(tf.interleaved_buffer_mode);
		Assert::IsFalse(tf.is_used());

		tf.varying_names.push_back("out_var_0");
		Assert::IsTrue(tf.is_used());

		tf.varying_names.clear();
		Assert::IsFalse(tf.is_used());
	}
};

TEST_CLASS(cg_data_shader_Funcs) {
public:

	TEST_METHOD(load_glsl_compute_data)
	{
		using cg::data::load_glsl_compute_data;
		using cg::data::load_text;

		Assert::ExpectException<std::exception&>([] { load_glsl_compute_data("unknown_file"); });

		auto cd = load_glsl_compute_data(Filenames::not_real_compute_glsl);
		auto expected_compute_source = load_text(Filenames::not_real_compute_glsl);
		Assert::AreEqual(expected_compute_source, cd.compute_shader_source_code);
	}

	TEST_METHOD(load_glsl_program_data)
	{
		using cg::data::load_glsl_program_data;
		using cg::data::load_text;

		Assert::ExpectException<std::exception&>([] { load_glsl_program_data("unknown_file"); });

		// vertex & fragment
		auto expected_vertex_source = load_text(Filenames::not_real_vertex_glsl);
		auto expected_fragment_source = load_text(Filenames::not_real_fragment_glsl);
		auto pd0 = load_glsl_program_data(Filenames::not_real_glsl_program_name);
		Assert::AreEqual(expected_vertex_source, pd0.vertex_shader_source_code);
		Assert::AreEqual(expected_fragment_source, pd0.fragment_shader_source_code);

		// vertex only
		auto expected_single_vertex_source = load_text(Filenames::not_real_single_vertex_glsl);
		auto pd1 = load_glsl_program_data(Filenames::not_real_glsl_single_vertex_program_name);
		Assert::AreEqual(expected_single_vertex_source, pd1.vertex_shader_source_code);
		Assert::IsTrue(pd1.fragment_shader_source_code.empty());

		// separate filenames
		Assert::ExpectException<std::exception&>([] { load_glsl_program_data("unknown_file0", "unknown_file1"); });
		Assert::ExpectException<std::exception&>([] { load_glsl_program_data(Filenames::not_real_vertex_glsl, "unknown_file1"); });
		Assert::ExpectException<std::exception&>([] { load_glsl_program_data("unknown_file0", Filenames::not_real_fragment_glsl); });

		auto pd2 = load_glsl_program_data(Filenames::not_real_vertex_glsl, Filenames::not_real_fragment_glsl);
		Assert::AreEqual(expected_vertex_source, pd2.vertex_shader_source_code);
		Assert::AreEqual(expected_fragment_source, pd2.fragment_shader_source_code);

		auto pd3 = load_glsl_program_data(Filenames::not_real_single_vertex_glsl, "");
		Assert::AreEqual(expected_single_vertex_source, pd3.vertex_shader_source_code);
		Assert::IsTrue(pd3.fragment_shader_source_code.empty());
	}

	TEST_METHOD(load_hlsl_shader_set_data)
	{
		using cg::data::load_hlsl_shader_set_data;
		using cg::data::load_text;

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

