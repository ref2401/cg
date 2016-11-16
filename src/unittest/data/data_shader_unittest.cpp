#include "cg/data/shader.h"

#include <utility>

#include "CppUnitTest.h"

using cg::data::Glsl_program_data;
using cg::data::Hlsl_shader_set_data;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace unittest {

TEST_CLASS(cg_data_shader_Glsl_program_data) {
public:

	TEST_METHOD(ctors)
	{
		Glsl_program_data dt0;
		Assert::IsTrue(dt0.vertex_shader_source_code.empty());
		Assert::IsTrue(dt0.fragment_shader_source_code.empty());

		std::string vert_srt = "abc";
		std::string pixel_str = "123";
		Glsl_program_data dt1(vert_srt, pixel_str);
		Assert::AreEqual(vert_srt, dt1.vertex_shader_source_code);
		Assert::AreEqual(pixel_str, dt1.fragment_shader_source_code);

		Glsl_program_data dt2("abc", "123");
		Assert::AreEqual("abc", dt2.vertex_shader_source_code.c_str());
		Assert::AreEqual("123", dt2.fragment_shader_source_code.c_str());

		// copy ctor
		Glsl_program_data dt_c = dt2;
		Assert::AreEqual(dt2.vertex_shader_source_code, dt_c.vertex_shader_source_code);
		Assert::AreEqual(dt2.fragment_shader_source_code, dt_c.fragment_shader_source_code);

		// move ctor
		Glsl_program_data dt_m = std::move(dt_c);
		Assert::AreEqual(dt2.vertex_shader_source_code, dt_m.vertex_shader_source_code);
		Assert::AreEqual(dt2.fragment_shader_source_code, dt_m.fragment_shader_source_code);
		Assert::IsTrue(dt_c.vertex_shader_source_code.empty());
		Assert::IsTrue(dt_c.fragment_shader_source_code.empty());
	}

	TEST_METHOD(assignments)
	{
		Glsl_program_data dt("abc", "123");

		Glsl_program_data dt_c;
		dt_c = dt;
		Assert::AreEqual(dt.vertex_shader_source_code, dt_c.vertex_shader_source_code);
		Assert::AreEqual(dt.fragment_shader_source_code, dt_c.fragment_shader_source_code);

		Glsl_program_data dt_m;
		dt_m = std::move(dt_c);
		Assert::AreEqual(dt.vertex_shader_source_code, dt_m.vertex_shader_source_code);
		Assert::AreEqual(dt.fragment_shader_source_code, dt_m.fragment_shader_source_code);
		Assert::IsTrue(dt_c.vertex_shader_source_code.empty());
		Assert::IsTrue(dt_c.fragment_shader_source_code.empty());
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

} // namespace unittest