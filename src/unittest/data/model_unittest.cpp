#include "cg/data/model.h"

#include <utility>
#include "CppUnitTest.h"

using cg::data::Model_mesh_info;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace Microsoft { namespace VisualStudio { namespace CppUnitTestFramework {

template<> inline std::wstring ToString<Model_mesh_info>(const Model_mesh_info& t) { RETURN_WIDE_STRING(t); }

}}} // namespace Microsoft::VisualStudio::CppUnitTestFramework


namespace unittest {

TEST_CLASS(cg_data_model_Model_geometry_data) {
public:

	TEST_METHOD(ctors)
	{

	}
};

TEST_CLASS(cg_data_model_Model_mesh_info) {
public:

	TEST_METHOD(assignment_operator)
	{
		Model_mesh_info mi(1, 2, 3, 4);

		Model_mesh_info mi_c;
		mi_c = mi;
		Assert::AreEqual<size_t>(mi.vertex_count, mi_c.vertex_count);
		Assert::AreEqual<size_t>(mi.base_vertex, mi_c.base_vertex);
		Assert::AreEqual<size_t>(mi.index_count, mi_c.index_count);
		Assert::AreEqual<size_t>(mi.index_offset, mi_c.index_offset);

		Model_mesh_info mi_m;
		mi_m = std::move(mi_c);
		Assert::AreEqual<size_t>(mi.vertex_count, mi_m.vertex_count);
		Assert::AreEqual<size_t>(mi.base_vertex, mi_m.base_vertex);
		Assert::AreEqual<size_t>(mi.index_count, mi_m.index_count);
		Assert::AreEqual<size_t>(mi.index_offset, mi_m.index_offset);
	}

	TEST_METHOD(ctors)
	{
		Model_mesh_info mi0;
		Assert::AreEqual<size_t>(0, mi0.vertex_count);
		Assert::AreEqual<size_t>(0, mi0.base_vertex);
		Assert::AreEqual<size_t>(0, mi0.index_count);
		Assert::AreEqual<size_t>(0, mi0.index_offset);

		Model_mesh_info mi1(1, 2, 3, 4);
		Assert::AreEqual<size_t>(1, mi1.vertex_count);
		Assert::AreEqual<size_t>(2, mi1.base_vertex);
		Assert::AreEqual<size_t>(3, mi1.index_count);
		Assert::AreEqual<size_t>(4, mi1.index_offset);

		Model_mesh_info mi_c = mi1;
		Assert::AreEqual<size_t>(mi1.vertex_count, mi_c.vertex_count);
		Assert::AreEqual<size_t>(mi1.base_vertex, mi_c.base_vertex);
		Assert::AreEqual<size_t>(mi1.index_count, mi_c.index_count);
		Assert::AreEqual<size_t>(mi1.index_offset, mi_c.index_offset);

		Model_mesh_info mi_m = std::move(mi_c);
		Assert::AreEqual<size_t>(mi1.vertex_count, mi_m.vertex_count);
		Assert::AreEqual<size_t>(mi1.base_vertex, mi_m.base_vertex);
		Assert::AreEqual<size_t>(mi1.index_count, mi_m.index_count);
		Assert::AreEqual<size_t>(mi1.index_offset, mi_m.index_offset);
	}

	TEST_METHOD(equal_operator)
	{
		Model_mesh_info mi(1, 2, 3, 4);

		Assert::AreNotEqual(mi, Model_mesh_info(100, 2, 3, 4));
		Assert::AreNotEqual(mi, Model_mesh_info(1, 200, 3, 4));
		Assert::AreNotEqual(mi, Model_mesh_info(1, 2, 300, 4));
		Assert::AreNotEqual(mi, Model_mesh_info(1, 2, 3, 400));

		Assert::AreEqual(mi, Model_mesh_info(1, 2, 3, 4));
	}
};

} // namespace
