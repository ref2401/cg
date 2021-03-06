#include "cg/rnd/dx11/dx11.h"

#include <iostream>
#include <utility>
#include "cg/data/file.h"
#include "unittest/data/common_file.h"
#include "CppUnitTest.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using cg::rnd::dx11::com_ptr;
using cg::rnd::dx11::hlsl_shader_desc;


namespace {

struct com_fake_class {

	com_fake_class() noexcept : counter(1) {}

	long long Release() noexcept
	{
		return --counter;
	}

	long long counter = 0;
};

std::ostream& operator<<(std::ostream& o, const com_fake_class& c)
{
	o << "Com_fake_class(" << c.counter << ")";
	return o;
}

std::wostream& operator<<(std::wostream& o, const com_fake_class& c)
{
	o << "Com_fake_class(" << c.counter << ")";
	return o;
}

} // namespace


namespace Microsoft { namespace VisualStudio { namespace CppUnitTestFramework {

template<> inline std::wstring ToString<long long>(const long long& t) { RETURN_WIDE_STRING(t); }
template<> inline std::wstring ToString<com_fake_class>(const com_fake_class& t) { RETURN_WIDE_STRING(t); }

}}} // namespace Microsoft::VisualStudio::CppUnitTestFramework


namespace unittest {

TEST_CLASS(cg_rnd_dx11_com_ptr) {
public:

	TEST_METHOD(assignmen_operators)
	{
		{ // ucp0 = nullptr;
			com_fake_class com_obj;
			com_ptr<com_fake_class> ucp(&com_obj);
			ucp = nullptr;
			Assert::IsNull(ucp.ptr);
			Assert::AreEqual<long long>(0, com_obj.counter);
		}

		{ // move assignment
			com_fake_class com_obj0;
			com_ptr<com_fake_class> ucp0(&com_obj0);

			com_fake_class com_obj1;
			com_ptr<com_fake_class> ucp1(&com_obj1);
			
			ucp0 = std::move(ucp1);
			Assert::AreEqual<long long>(0, com_obj0.counter);	// ucp0 prev resource was released
			Assert::AreEqual<long long>(1, com_obj1.counter);	// ucp1 counter has not changed
			Assert::IsTrue(&com_obj1 == ucp0.ptr);				// ucp0 points to the com_obj1 resource
			Assert::IsNull(ucp1.ptr);							// ucp1 points to nothing
		}

		{ // move assignment, self assignment
			com_fake_class com_obj;
			com_ptr<com_fake_class> ucp(&com_obj);

			ucp = std::move(ucp);
			Assert::AreEqual<long long>(1, com_obj.counter);	// counter has not changed
			Assert::IsNotNull(ucp.ptr);
			Assert::IsTrue(&com_obj == ucp.ptr);
		}

		{ // ucp = raw_pointer
			com_fake_class com_obj0;
			com_ptr<com_fake_class> ucp(&com_obj0);

			com_fake_class com_obj1;

			ucp = &com_obj1;
			Assert::AreEqual<long long>(0, com_obj0.counter);	// ucp prev resource was released
			Assert::AreEqual<long long>(1, com_obj1.counter);	// counter has not changed
			Assert::IsTrue(&com_obj1 == ucp.ptr);				// ucp points to the com_obj1 resource
		}
	}

	TEST_METHOD(ctors_dtor)
	{
		com_ptr<com_fake_class> ucp0;
		Assert::IsNull(ucp0.ptr);

		com_ptr<com_fake_class> ucp00 = nullptr;
		Assert::IsNull(ucp00.ptr);

		// ctor(T*) & dtor().
		com_fake_class com_obj0;
		Assert::AreEqual<long long>(1, com_obj0.counter);
		{
			com_ptr<com_fake_class> ucp(&com_obj0);
			Assert::IsNotNull(ucp.ptr);
			Assert::IsTrue(&com_obj0 == ucp.ptr);
		}
		Assert::AreEqual<long long>(0, com_obj0.counter);

		// move ctor
		com_fake_class com_obj1;
		com_ptr<com_fake_class> ucp1(&com_obj1);
		com_ptr<com_fake_class> ucp2(std::move(ucp1));
		Assert::AreEqual<long long>(1, com_obj1.counter);
		Assert::IsNull(ucp1.ptr);
		Assert::IsNotNull(ucp2.ptr);
		Assert::IsTrue(&com_obj1 == ucp2.ptr);
	}

	TEST_METHOD(implicit_cast_to_bool)
	{
		//com_ptr<com_fake_class> ucp0;
		//Assert::IsFalse(ucp0);

		//com_fake_class com_obj;
		//com_ptr<com_fake_class> ucp1(&com_obj);
		//Assert::IsTrue(ucp1);
	}

	TEST_METHOD(implicit_cast_to_ptr)
	{
		auto is_nullptr = [](com_fake_class* ptr) { return (ptr == nullptr); };
		
		com_ptr<com_fake_class> cp0;
		Assert::IsTrue(is_nullptr(cp0));
		com_fake_class* p0 = cp0;
		Assert::IsNull(p0);

		com_fake_class com_obj;
		com_ptr<com_fake_class> cp1(&com_obj);
		Assert::IsFalse(is_nullptr(cp1));
		com_fake_class* p1 = cp1;
		Assert::IsNotNull(p1);
	}

	TEST_METHOD(dispose)
	{
		com_ptr<com_fake_class> ucp0;
		ucp0.dispose();
		Assert::IsNull(ucp0.ptr);

		com_fake_class com_obj;
		com_ptr<com_fake_class> ucp1(&com_obj);
		Assert::IsNotNull(ucp1.ptr);
		ucp1.dispose();
		Assert::IsNull(ucp1.ptr);
	}

	TEST_METHOD(operator_arrow)
	{
		com_fake_class com_obj;
		com_ptr<com_fake_class> ucp(&com_obj);
		Assert::AreEqual<long long>(1, ucp->counter);
	}

	TEST_METHOD(operator_asterisk)
	{
		com_fake_class com_obj;
		com_ptr<com_fake_class> ucp(&com_obj);
		Assert::AreEqual<long long>(1, (*ucp).counter);
	}

	TEST_METHOD(release_ownership)
	{
		com_ptr<com_fake_class> ucp0;
		auto* ptr0 = ucp0.release_ownership();
		Assert::IsNull(ucp0.ptr);
		Assert::IsNull(ptr0);

		com_fake_class com_obj;
		com_ptr<com_fake_class> ucp1(&com_obj);
		auto* ptr1= ucp1.release_ownership();
		Assert::IsNull(ucp1.ptr);
		Assert::IsTrue(&com_obj == ptr1);
	}
};

TEST_CLASS(cg_data_shader_hlsl_shader_desc) {
public:

	TEST_METHOD(assignment_operators)
	{
		hlsl_shader_desc hd0;
		hd0.source_code = "abc;";
		hd0.source_filename = ".txt";
		hd0.vertex_shader_entry_point = "vs";
		hd0.pixel_shader_entry_point = "ps";
		hd0.compile_flags = 42;

		// copy assignment
		hlsl_shader_desc hd_c;
		hd_c = hd0;
		Assert::AreEqual(hd_c.source_code, hd0.source_code);
		Assert::AreEqual(hd_c.source_filename, hd0.source_filename);
		Assert::AreEqual(hd_c.vertex_shader_entry_point, hd0.vertex_shader_entry_point);
		Assert::AreEqual(hd_c.pixel_shader_entry_point, hd0.pixel_shader_entry_point);
		Assert::AreEqual(hd_c.compile_flags, hd0.compile_flags);

		// move assignment
		hlsl_shader_desc hd_m;
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
		hlsl_shader_desc hd0;
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
		hlsl_shader_desc hd_c = hd0;
		Assert::AreEqual(hd_c.source_code, hd0.source_code);
		Assert::AreEqual(hd_c.source_filename, hd0.source_filename);
		Assert::AreEqual(hd_c.vertex_shader_entry_point, hd0.vertex_shader_entry_point);
		Assert::AreEqual(hd_c.pixel_shader_entry_point, hd0.pixel_shader_entry_point);
		Assert::AreEqual(hd_c.compile_flags, hd0.compile_flags);

		// move ctor
		hlsl_shader_desc hd_m = std::move(hd_c);
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
};

TEST_CLASS(cg_rnd_dx11_funcs) {
public:

	TEST_METHOD(has_xxx_shader_hlsl_shader_desc)
	{
		hlsl_shader_desc hd;
		Assert::IsFalse(has_vertex_shader(hd));
		Assert::IsFalse(has_pixel_shader(hd));

		hd.vertex_shader_entry_point = "vs";
		Assert::IsTrue(has_vertex_shader(hd));

		hd.pixel_shader_entry_point = "ps";
		Assert::IsTrue(has_pixel_shader(hd));
	}

	TEST_METHOD(load_hlsl_shader_set_desc)
	{
		using cg::data::load_text;
		using cg::rnd::dx11::load_hlsl_shader_set_desc;


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