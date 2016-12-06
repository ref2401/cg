#include "cg/data/model.h"

#include <cstring>
#include <algorithm>
#include <iterator>
#include <utility>
#include "CppUnitTest.h"

using cg::float2;
using cg::float3;
using cg::float4;
using cg::data::Model_geometry_data;
using cg::data::Model_mesh_info;
using cg::data::Vertex_attribs;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace Microsoft { namespace VisualStudio { namespace CppUnitTestFramework {

template<> inline std::wstring ToString<Model_mesh_info>(const Model_mesh_info& t) { RETURN_WIDE_STRING(t); }

}}} // namespace Microsoft::VisualStudio::CppUnitTestFramework


namespace unittest {

TEST_CLASS(cg_data_model_Model_geometry_data) {
public:

	TEST_METHOD(push_back_indices)
	{
		Model_geometry_data<Vertex_attribs::p> geometry_data;
		Assert::AreEqual<size_t>(0, geometry_data.index_count());
		Assert::IsTrue(geometry_data.index_data().empty());

		// single vertex
		geometry_data.push_back_index(0);
		Assert::AreEqual<size_t>(1, geometry_data.index_count());
		Assert::AreEqual<size_t>(1, geometry_data.index_data().size());

		// static array
		uint32_t indices[3] = { 1, 2, 3 };
		geometry_data.push_back_indices(indices);
		Assert::AreEqual<size_t>(4, geometry_data.index_count());
		Assert::AreEqual<size_t>(4, geometry_data.index_data().size());

		// container
		std::vector<uint32_t> index_list = { 4, 5, 6, 7 };
		geometry_data.push_back_indices(index_list);
		Assert::AreEqual<size_t>(8, geometry_data.index_count());
		Assert::AreEqual<size_t>(8, geometry_data.index_data().size());

		// pair of iterators
		geometry_data.push_back_indices(index_list.cbegin(), index_list.cend());
		Assert::AreEqual<size_t>(12, geometry_data.index_count());
		Assert::AreEqual<size_t>(12, geometry_data.index_data().size());

		uint32_t expected_data[12] = { 0, 1, 2, 3, 4, 5, 6, 7, 4, 5, 6, 7 };
		Assert::IsTrue(std::equal(
			std::cbegin(expected_data), std::cend(expected_data),
			geometry_data.index_data().cbegin()));
	}

	TEST_METHOD(push_back_mesh)
	{
		using Model_geometry_data_t = Model_geometry_data<Vertex_attribs::p>;
		constexpr size_t vertex_byte_count = Model_geometry_data_t::Format::vertex_byte_count;

		Model_geometry_data_t geometry_data;
		Assert::AreEqual<size_t>(0, geometry_data.mesh_count());
		Assert::AreEqual<size_t>(0, geometry_data.vertex_data().capacity());
		Assert::AreEqual<size_t>(0, geometry_data.index_data().capacity());

		// a mesh contains 1 vertex and 1 index
		geometry_data.push_back_mesh(1, 0, 1, 0);
		Assert::AreEqual<size_t>(1, geometry_data.mesh_count());
		Assert::IsTrue(geometry_data.vertex_data().capacity() >= vertex_byte_count);
		Assert::IsTrue(geometry_data.index_data().capacity() >= 1);

		// a mesh contains 4 vertices and 6 indices
		geometry_data.push_back_mesh(4, 0, 6, 0);
		Assert::AreEqual<size_t>(2, geometry_data.mesh_count());
		Assert::IsTrue(geometry_data.vertex_data().capacity() >= 4 * vertex_byte_count);
		Assert::IsTrue(geometry_data.index_data().capacity() >= 6);

		Model_mesh_info expected_meshes[2] = {
			{ 1, 0, 1, 0 },
			{ 4, 0, 6, 0 }
		};

		Assert::IsTrue(std::equal(
			std::cbegin(expected_meshes), std::cend(expected_meshes),
			geometry_data.meshes().cbegin()));
	}

	TEST_METHOD(push_back_vertex_p)
	{
		using Model_geometry_data_t = Model_geometry_data<Vertex_attribs::p>;
		constexpr size_t vertex_byte_count = Model_geometry_data_t::Format::vertex_byte_count;

		Model_geometry_data_t::Vertex v0(float3(1, 2, 3));
		Model_geometry_data_t::Vertex v1(float3(4, 5, 6));

		Model_geometry_data_t geometry_data;
		Assert::AreEqual<size_t>(0, geometry_data.vertex_count());
		Assert::IsTrue(geometry_data.vertex_data().empty());

		geometry_data.push_back_vertex(v0);
		Assert::AreEqual<size_t>(1, geometry_data.vertex_count());
		Assert::AreEqual<size_t>(vertex_byte_count, geometry_data.vertex_data().size());
		
		geometry_data.push_back_vertex(v1);
		Assert::AreEqual<size_t>(2, geometry_data.vertex_count());
		Assert::AreEqual<size_t>(2 * vertex_byte_count, geometry_data.vertex_data().size());


		unsigned char expected_vertex_data[2 * vertex_byte_count];
		std::memcpy(expected_vertex_data, v0.data, vertex_byte_count);
		std::memcpy(expected_vertex_data + vertex_byte_count, v1.data, vertex_byte_count);
		Assert::IsTrue(std::equal(
			std::cbegin(expected_vertex_data), std::cend(expected_vertex_data),
			geometry_data.vertex_data().cbegin()));
	}

	TEST_METHOD(push_back_vertex_p_n)
	{
		using Model_geometry_data_t = Model_geometry_data<Vertex_attribs::p_n>;
		constexpr size_t vertex_byte_count = Model_geometry_data_t::Format::vertex_byte_count;

		Model_geometry_data_t::Vertex v0(float3(1, 2, 3), float3(4, 5, 6));
		Model_geometry_data_t::Vertex v1(float3(4, 5, 6), float3(7, 8, 9));

		Model_geometry_data_t geometry_data;
		Assert::AreEqual<size_t>(0, geometry_data.vertex_count());
		Assert::IsTrue(geometry_data.vertex_data().empty());

		geometry_data.push_back_vertex(v0);
		Assert::AreEqual<size_t>(1, geometry_data.vertex_count());
		Assert::AreEqual<size_t>(vertex_byte_count, geometry_data.vertex_data().size());

		geometry_data.push_back_vertex(v1);
		Assert::AreEqual<size_t>(2, geometry_data.vertex_count());
		Assert::AreEqual<size_t>(2 * vertex_byte_count, geometry_data.vertex_data().size());


		unsigned char expected_vertex_data[2 * vertex_byte_count];
		std::memcpy(expected_vertex_data, v0.data, vertex_byte_count);
		std::memcpy(expected_vertex_data + vertex_byte_count, v1.data, vertex_byte_count);
		Assert::IsTrue(std::equal(
			std::cbegin(expected_vertex_data), std::cend(expected_vertex_data),
			geometry_data.vertex_data().cbegin()));
	}

	TEST_METHOD(push_back_vertex_p_n_tc)
	{
		using Model_geometry_data_t = Model_geometry_data<Vertex_attribs::p_n_tc>;
		constexpr size_t vertex_byte_count = Model_geometry_data_t::Format::vertex_byte_count;

		Model_geometry_data_t::Vertex v0(float3(1, 2, 3), float3(4, 5, 6), float2(7, 8));
		Model_geometry_data_t::Vertex v1(float3(4, 5, 6), float3(7, 8, 9), float2(10, 11));

		Model_geometry_data_t geometry_data;
		Assert::AreEqual<size_t>(0, geometry_data.vertex_count());
		Assert::IsTrue(geometry_data.vertex_data().empty());

		geometry_data.push_back_vertex(v0);
		Assert::AreEqual<size_t>(1, geometry_data.vertex_count());
		Assert::AreEqual<size_t>(vertex_byte_count, geometry_data.vertex_data().size());

		geometry_data.push_back_vertex(v1);
		Assert::AreEqual<size_t>(2, geometry_data.vertex_count());
		Assert::AreEqual<size_t>(2 * vertex_byte_count, geometry_data.vertex_data().size());


		unsigned char expected_vertex_data[2 * vertex_byte_count];
		std::memcpy(expected_vertex_data, v0.data, vertex_byte_count);
		std::memcpy(expected_vertex_data + vertex_byte_count, v1.data, vertex_byte_count);
		Assert::IsTrue(std::equal(
			std::cbegin(expected_vertex_data), std::cend(expected_vertex_data),
			geometry_data.vertex_data().cbegin()));
	}

	TEST_METHOD(push_back_vertex_p_tc)
	{
		using Model_geometry_data_t = Model_geometry_data<Vertex_attribs::p_tc>;
		constexpr size_t vertex_byte_count = Model_geometry_data_t::Format::vertex_byte_count;

		Model_geometry_data_t::Vertex v0(float3(1, 2, 3), float2(4, 5));
		Model_geometry_data_t::Vertex v1(float3(4, 5, 6), float2(7, 8));

		Model_geometry_data_t geometry_data;
		Assert::AreEqual<size_t>(0, geometry_data.vertex_count());
		Assert::IsTrue(geometry_data.vertex_data().empty());

		geometry_data.push_back_vertex(v0);
		Assert::AreEqual<size_t>(1, geometry_data.vertex_count());
		Assert::AreEqual<size_t>(vertex_byte_count, geometry_data.vertex_data().size());

		geometry_data.push_back_vertex(v1);
		Assert::AreEqual<size_t>(2, geometry_data.vertex_count());
		Assert::AreEqual<size_t>(2 * vertex_byte_count, geometry_data.vertex_data().size());


		unsigned char expected_vertex_data[2 * vertex_byte_count];
		std::memcpy(expected_vertex_data, v0.data, vertex_byte_count);
		std::memcpy(expected_vertex_data + vertex_byte_count, v1.data, vertex_byte_count);
		Assert::IsTrue(std::equal(
			std::cbegin(expected_vertex_data), std::cend(expected_vertex_data),
			geometry_data.vertex_data().cbegin()));
	}

	TEST_METHOD(push_back_vertex_p_n_tc_ts)
	{
		using Model_geometry_data_t = Model_geometry_data<Vertex_attribs::p_n_tc_ts>;
		constexpr size_t vertex_byte_count = Model_geometry_data_t::Format::vertex_byte_count;

		Model_geometry_data_t::Vertex v0(float3(1, 2, 3), float3(4, 5, 6), float2(7, 8), float4(9, 10, 11, 12));
		Model_geometry_data_t::Vertex v1(float3(4, 5, 6), float3(7, 8, 9), float2(10, 11), float4(12, 13, 14, 15));

		Model_geometry_data_t geometry_data;
		Assert::AreEqual<size_t>(0, geometry_data.vertex_count());
		Assert::IsTrue(geometry_data.vertex_data().empty());

		geometry_data.push_back_vertex(v0);
		Assert::AreEqual<size_t>(1, geometry_data.vertex_count());
		Assert::AreEqual<size_t>(vertex_byte_count, geometry_data.vertex_data().size());

		geometry_data.push_back_vertex(v1);
		Assert::AreEqual<size_t>(2, geometry_data.vertex_count());
		Assert::AreEqual<size_t>(2 * vertex_byte_count, geometry_data.vertex_data().size());


		unsigned char expected_vertex_data[2 * vertex_byte_count];
		std::memcpy(expected_vertex_data, v0.data, vertex_byte_count);
		std::memcpy(expected_vertex_data + vertex_byte_count, v1.data, vertex_byte_count);
		Assert::IsTrue(std::equal(
			std::cbegin(expected_vertex_data), std::cend(expected_vertex_data),
			geometry_data.vertex_data().cbegin()));
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
