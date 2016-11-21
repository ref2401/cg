#include "cg/data/mesh.h"

#include <algorithm>
#include <iterator>
#include <type_traits>
#include "cg/math/math.h"
#include "unittest/math/common_math.h"
#include "unittest/data/common_file.h"
#include "CppUnitTest.h"

using cg::float2;
using cg::float3;
using cg::float4;
using cg::approx_equal;
using cg::data::Interleaved_mesh_data;
using cg::data::Interleaved_mesh_data_old;
using cg::data::Interleaved_vertex_format_old;
using cg::data::Mesh_builder;
using cg::data::Vertex_ts;
using cg::data::Vertex_old;
using cg::data::Vertex_attribs;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace Microsoft { namespace VisualStudio { namespace CppUnitTestFramework {

template<> inline std::wstring ToString<Interleaved_vertex_format_old>(const Interleaved_vertex_format_old& t) { RETURN_WIDE_STRING(t); }
template<> inline std::wstring ToString<Vertex_ts>(const Vertex_ts& t) { RETURN_WIDE_STRING(t); }
template<> inline std::wstring ToString<Vertex_old>(const Vertex_old& t) { RETURN_WIDE_STRING(t); }
template<> inline std::wstring ToString<Vertex_attribs>(const Vertex_attribs& t) { RETURN_WIDE_STRING(t); }

}}} // namespace Microsoft::VisualStudio::CppUnitTestFramework


namespace unittest {

TEST_CLASS(cg_data_mesh_Interleaved_mesh_data) {
public:

	TEST_METHOD(push_back_vertex_and_vertex_count)
	{
		Interleaved_mesh_data<Vertex_attribs::vertex_p_n> mesh_data(2, 0);
		Assert::AreEqual(size_t(0), mesh_data.vertex_count());
		Assert::IsTrue(mesh_data.vertex_data().empty());

		float data[6] = { 0, 1, 2, 3, 4, 5 };

		mesh_data.push_back_vertex(data);
		Assert::AreEqual(size_t(1), mesh_data.vertex_count());
		Assert::AreEqual(size_t(6), mesh_data.vertex_data().size());

		mesh_data.push_back_vertex(data);
		Assert::AreEqual(size_t(2), mesh_data.vertex_count());
		Assert::AreEqual(size_t(12), mesh_data.vertex_data().size());

		float expected_data[12] = { 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5 };
		Assert::IsTrue(std::equal(
			std::cbegin(expected_data), std::cend(expected_data),
			mesh_data.vertex_data().cbegin(),
			[](float a, float b) { return approx_equal<float>(a, b); }));
	}

	TEST_METHOD(push_back_indices_and_index_count)
	{
		Interleaved_mesh_data<Vertex_attribs::position> mesh_data(0, 4);
		Assert::AreEqual(size_t(0), mesh_data.index_count());
		Assert::IsTrue(mesh_data.index_data().empty());

		// single vertex
		mesh_data.push_back_index(0);
		Assert::AreEqual(size_t(1), mesh_data.index_count());
		Assert::AreEqual(size_t(1), mesh_data.index_data().size());

		// static array
		uint32_t indices[3] = { 1, 2, 3 };
		mesh_data.push_back_indices(indices);
		Assert::AreEqual(size_t(4), mesh_data.index_count());
		Assert::AreEqual(size_t(4), mesh_data.index_data().size());

		// container
		std::vector<uint32_t> index_list = { 4, 5, 6, 7 };
		mesh_data.push_back_indices(index_list);
		Assert::AreEqual(size_t(8), mesh_data.index_count());
		Assert::AreEqual(size_t(8), mesh_data.index_data().size());

		// pair of iterators
		mesh_data.push_back_indices(index_list.cbegin(), index_list.cend());
		Assert::AreEqual(size_t(12), mesh_data.index_count());
		Assert::AreEqual(size_t(12), mesh_data.index_data().size());

		uint32_t expected_data[12] = { 0, 1, 2, 3, 4, 5, 6, 7, 4, 5, 6, 7 };
		Assert::IsTrue(std::equal(
			std::cbegin(expected_data), std::cend(expected_data),
			mesh_data.index_data().cbegin()));
	}

	TEST_METHOD(vertex_index_data_byte_count)
	{
		using Imd_t = Interleaved_mesh_data<Vertex_attribs::vertex_p_n>;
		Imd_t::Vertex_data_array vertex_data;
		std::fill(std::begin(vertex_data), std::end(vertex_data), 0.0f);
		
		Imd_t mesh_data(3, 3);
		Assert::AreEqual<size_t>(0, mesh_data.vertex_data_byte_count());
		Assert::AreEqual<size_t>(0, mesh_data.index_data_byte_count());

		mesh_data.push_back_vertex(vertex_data);
		mesh_data.push_back_vertex(vertex_data);
		mesh_data.push_back_vertex(vertex_data);
		Assert::AreEqual<size_t>(3 * Imd_t::Format::vertex_byte_count, mesh_data.vertex_data_byte_count());

		uint32_t indices[9] = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };
		size_t expected_index_data_byte_count = std::extent<decltype(indices)>::value * sizeof(uint32_t);
		mesh_data.push_back_indices(indices);
		Assert::AreEqual<size_t>(expected_index_data_byte_count, mesh_data.index_data_byte_count());
	}
};

TEST_CLASS(cg_data_mesh_Interleaved_mesh_data_old) {
public:

	TEST_METHOD(ctors)
	{
		Interleaved_mesh_data_old md0(Vertex_attribs::position);
		Assert::AreEqual(Vertex_attribs::position, md0.attribs());
		Assert::AreEqual(0u, md0.data().capacity());
		Assert::AreEqual(0u, md0.indices().capacity());

		Interleaved_mesh_data_old md1(Vertex_attribs::vertex_p_tc, 4, 6);
		Assert::AreEqual(Vertex_attribs::vertex_p_tc, md1.attribs());
		Assert::AreEqual(md1.data().capacity(),
			4 * (Interleaved_vertex_format_old::component_count_position + Interleaved_vertex_format_old::component_count_tex_coord));
		Assert::AreEqual(6u, md1.indices().capacity());
	}

	TEST_METHOD(index_count)
	{
		Interleaved_mesh_data_old md(Vertex_attribs::position);
		Assert::AreEqual<size_t>(0, md.index_count());

		md.push_back_index(0);
		Assert::AreEqual<size_t>(1, md.index_count());

		md.push_back_index(1);
		Assert::AreEqual<size_t>(2, md.index_count());

		md.push_back_indices(2, 3, 4);
		Assert::AreEqual<size_t>(5, md.index_count());
	}

	TEST_METHOD(push_back_indices)
	{
		uint32_t expected_indices[5] = { 0, 1, 2, 3, 4 };

		Interleaved_mesh_data_old md(Vertex_attribs::position);
		md.push_back_index(0);
		md.push_back_indices(1, 2, 3);
		md.push_back_index(4);

		Assert::IsTrue(std::equal(md.indices().cbegin(), md.indices().cend(), std::cbegin(expected_indices)));
	}

	TEST_METHOD(push_back_vertices)
	{
		Vertex_old v0(float3(1, 2, 3), float3(6, 7, 8), float2(4, 5),  float4(9, 10, 11, 12));
		Vertex_old v1(float3(101, 102, 103), float3(106, 107, 108), float2(104, 105), float4(109, 110, 111, 112));
		Vertex_old v2(float3(201, 202, 203), float3(206, 207, 208), float2(204, 205), float4(209, 210, 211, 212));

		{ // position attrib
			float expected_data[9] = { 
				v0.position.x, v0.position.y, v0.position.z,
				v1.position.x, v1.position.y, v1.position.z,
				v2.position.x, v2.position.y, v2.position.z
			};

			Interleaved_mesh_data_old md(Vertex_attribs::position);
			md.push_back_vertex(v0);
			md.push_back_vertex(v1);
			md.push_back_vertex(v2);

			Assert::IsTrue(std::equal(
				md.data().cbegin(), md.data().cend(), 
				std::cbegin(expected_data), 
				[](float a, float b) { return approx_equal<float>(a, b); }));
		}

		{ // position & normal attribs
			float expected_data[18] = {
				v0.position.x, v0.position.y, v0.position.z, v0.normal.x, v0.normal.y, v0.normal.z,
				v1.position.x, v1.position.y, v1.position.z, v1.normal.x, v1.normal.y, v1.normal.z,
				v2.position.x, v2.position.y, v2.position.z, v2.normal.x, v2.normal.y, v2.normal.z
			};

			Interleaved_mesh_data_old md(Vertex_attribs::position | Vertex_attribs::normal);
			md.push_back_vertex(v0);
			md.push_back_vertex(v1);
			md.push_back_vertex(v2);

			Assert::IsTrue(std::equal(
				md.data().cbegin(), md.data().cend(),
				std::cbegin(expected_data),
				[](float a, float b) { return approx_equal<float>(a, b); }));
		}

		{ // position, normal & tex_coord attribs
			float expected_data[24] = {
				v0.position.x, v0.position.y, v0.position.z, v0.normal.x, v0.normal.y, v0.normal.z, v0.tex_coord.u, v0.tex_coord.v,
				v1.position.x, v1.position.y, v1.position.z, v1.normal.x, v1.normal.y, v1.normal.z, v1.tex_coord.u, v1.tex_coord.v,
				v2.position.x, v2.position.y, v2.position.z, v2.normal.x, v2.normal.y, v2.normal.z, v2.tex_coord.u, v2.tex_coord.v,
			};

			Interleaved_mesh_data_old md(Vertex_attribs::position | Vertex_attribs::normal | Vertex_attribs::tex_coord);
			md.push_back_vertex(v0);
			md.push_back_vertex(v1);
			md.push_back_vertex(v2);

			Assert::IsTrue(std::equal(
				md.data().cbegin(), md.data().cend(),
				std::cbegin(expected_data),
				[](float a, float b) { return approx_equal<float>(a, b); }));
		}

		{ // position, normal & tex_coord attribs
			float expected_data[36] = {
				v0.position.x, v0.position.y, v0.position.z, v0.normal.x, v0.normal.y, v0.normal.z, 
				v0.tex_coord.u, v0.tex_coord.v, v0.tangent_space.x, v0.tangent_space.y, v0.tangent_space.z, v0.tangent_space.w,
				
				v1.position.x, v1.position.y, v1.position.z, v1.normal.x, v1.normal.y, v1.normal.z, 
				v1.tex_coord.u, v1.tex_coord.v, v1.tangent_space.x, v1.tangent_space.y, v1.tangent_space.z, v1.tangent_space.w,

				v2.position.x, v2.position.y, v2.position.z, v2.normal.x, v2.normal.y, v2.normal.z, 
				v2.tex_coord.u, v2.tex_coord.v, v2.tangent_space.x, v2.tangent_space.y, v2.tangent_space.z, v2.tangent_space.w,
			};

			Interleaved_mesh_data_old md(Vertex_attribs::vertex_ts);
			md.push_back_vertex(v0);
			md.push_back_vertex(v1);
			md.push_back_vertex(v2);

			Assert::IsTrue(std::equal(
				md.data().cbegin(), md.data().cend(),
				std::cbegin(expected_data),
				[](float a, float b) { return approx_equal<float>(a, b); }));
		}
	}

	TEST_METHOD(vertex_count)
	{
		Interleaved_mesh_data_old md(Vertex_attribs::position);
		Assert::AreEqual<size_t>(0, md.vertex_count());

		md.push_back_vertex(Vertex_old());
		Assert::AreEqual<size_t>(1, md.vertex_count());
		
		md.push_back_vertex(Vertex_old());
		Assert::AreEqual<size_t>(2, md.vertex_count());

		md.push_back_vertices(Vertex_old(), Vertex_old(), Vertex_old());
		Assert::AreEqual<size_t>(5, md.vertex_count());
	}
};

TEST_CLASS(cg_data_mesh_Interleaved_vertex_fotmat_old) {
public:

	TEST_METHOD(byte_count)
	{
		Interleaved_vertex_format_old fmt0(Vertex_attribs::position);
		Assert::AreEqual(fmt0.component_count() * sizeof(float), fmt0.byte_count());

		Interleaved_vertex_format_old fmt1(Vertex_attribs::vertex_p_tc);
		Assert::AreEqual(fmt1.component_count() * sizeof(float), fmt1.byte_count());

		Interleaved_vertex_format_old fmt2(Vertex_attribs::vertex_ts);
		Assert::AreEqual(fmt2.component_count() * sizeof(float), fmt2.byte_count());
	}

	TEST_METHOD(byte_offset)
	{
		Interleaved_vertex_format_old fmt(Vertex_attribs::vertex_ts);
		
		Assert::AreEqual(fmt.byte_offset_position(), 0u);
		
		Assert::AreEqual(fmt.byte_offset_normal(), 
			Interleaved_vertex_format_old::component_count_position * sizeof(float));
		
		Assert::AreEqual(fmt.byte_offset_tex_coord(), 
			(Interleaved_vertex_format_old::component_count_position 
			+ Interleaved_vertex_format_old::component_count_normal) * sizeof(float));

		Assert::AreEqual(fmt.byte_offset_tangent_h(),
			(Interleaved_vertex_format_old::component_count_position
				+ Interleaved_vertex_format_old::component_count_normal
				+ Interleaved_vertex_format_old::component_count_tex_coord) * sizeof(float));
	}

	TEST_METHOD(ctors)
	{
		Interleaved_vertex_format_old fmt0;
		Assert::AreEqual(Vertex_attribs::none, fmt0.attribs);

		Interleaved_vertex_format_old fmt1(Vertex_attribs::normal);
		Assert::AreEqual(Vertex_attribs::normal, fmt1.attribs);

		Interleaved_vertex_format_old fmt2(Vertex_attribs::vertex_ts);
		Assert::AreEqual(Vertex_attribs::vertex_ts, fmt2.attribs);
	}

	TEST_METHOD(component_count)
	{
		Assert::AreEqual(Interleaved_vertex_format_old::component_count_position, 
			Interleaved_vertex_format_old(Vertex_attribs::position).component_count());
		
		Assert::AreEqual(Interleaved_vertex_format_old::component_count_normal,
			Interleaved_vertex_format_old(Vertex_attribs::normal).component_count());
		
		Assert::AreEqual(Interleaved_vertex_format_old::component_count_tex_coord,
			Interleaved_vertex_format_old(Vertex_attribs::tex_coord).component_count());
		
		Assert::AreEqual(Interleaved_vertex_format_old::component_count_tangent_h,
			Interleaved_vertex_format_old(Vertex_attribs::tangent_space).component_count());

		// position & normal
		auto attribs_pn = Vertex_attribs::position | Vertex_attribs::normal;
		size_t expected_pn = Interleaved_vertex_format_old::component_count_position
			+ Interleaved_vertex_format_old::component_count_normal;
		Assert::AreEqual(expected_pn, Interleaved_vertex_format_old(attribs_pn).component_count());

		// position & tex_coord
		size_t expected_ptc = Interleaved_vertex_format_old::component_count_position
			+ Interleaved_vertex_format_old::component_count_tex_coord;
		Assert::AreEqual(expected_ptc, Interleaved_vertex_format_old(Vertex_attribs::vertex_p_tc).component_count());

		// position, normal & tex_coord
		auto attribs_pntc = Vertex_attribs::position | Vertex_attribs::normal | Vertex_attribs::tex_coord;
		size_t expected_pntc = Interleaved_vertex_format_old::component_count_position
			+ Interleaved_vertex_format_old::component_count_normal
			+ Interleaved_vertex_format_old::component_count_tex_coord;
		Assert::AreEqual(expected_pntc, Interleaved_vertex_format_old(attribs_pntc).component_count());

		// position, normal, tex_coord & tangnet_h
		size_t expected_pntcth = Interleaved_vertex_format_old::component_count_position
			+ Interleaved_vertex_format_old::component_count_normal
			+ Interleaved_vertex_format_old::component_count_tex_coord
			+ Interleaved_vertex_format_old::component_count_tangent_h;
		Assert::AreEqual(expected_pntcth, Interleaved_vertex_format_old(Vertex_attribs::vertex_ts).component_count());

		// tex_coord & tangent_h
		auto attribs_tcth = Vertex_attribs::tex_coord | Vertex_attribs::tangent_space;
		size_t expected_tcth = Interleaved_vertex_format_old::component_count_tex_coord
			+ Interleaved_vertex_format_old::component_count_tangent_h;
		Assert::AreEqual(expected_tcth, Interleaved_vertex_format_old(attribs_tcth).component_count());
	}

	TEST_METHOD(component_offset_all)
	{
		Interleaved_vertex_format_old p_n_tc_th(Vertex_attribs::vertex_ts);
		Assert::AreEqual(p_n_tc_th.component_offset_position(),	0u);
		Assert::AreEqual(p_n_tc_th.component_offset_normal(), Interleaved_vertex_format_old::component_count_position);
		Assert::AreEqual(p_n_tc_th.component_offset_tex_coord(), 
			Interleaved_vertex_format_old::component_count_position + Interleaved_vertex_format_old::component_count_normal);
		Assert::AreEqual(p_n_tc_th.component_offset_tangent_h(),
			Interleaved_vertex_format_old::component_count_position + Interleaved_vertex_format_old::component_count_normal
			+ Interleaved_vertex_format_old::component_count_tex_coord);

		Interleaved_vertex_format_old n_tc_th(Vertex_attribs::normal
			| Vertex_attribs::tex_coord | Vertex_attribs::tangent_space);
		Assert::AreEqual(n_tc_th.component_offset_position(), 0u);
		Assert::AreEqual(n_tc_th.component_offset_normal(), 0u);
		Assert::AreEqual(n_tc_th.component_offset_tex_coord(), Interleaved_vertex_format_old::component_count_normal);
		Assert::AreEqual(n_tc_th.component_offset_tangent_h(), Interleaved_vertex_format_old::component_count_normal 
			+ Interleaved_vertex_format_old::component_count_tex_coord);

		Interleaved_vertex_format_old tc_th(Vertex_attribs::tex_coord | Vertex_attribs::tangent_space);
		Assert::AreEqual(tc_th.component_offset_position(), 0u);
		Assert::AreEqual(tc_th.component_offset_normal(), 0u);
		Assert::AreEqual(tc_th.component_offset_tex_coord(), 0u);
		Assert::AreEqual(tc_th.component_offset_tangent_h(), Interleaved_vertex_format_old::component_count_tex_coord);

		Interleaved_vertex_format_old th(Vertex_attribs::tangent_space);
		Assert::AreEqual(th.component_offset_position(), 0u);
		Assert::AreEqual(th.component_offset_normal(), 0u);
		Assert::AreEqual(th.component_offset_tex_coord(), 0u);
		Assert::AreEqual(th.component_offset_tangent_h(), 0u);

		Interleaved_vertex_format_old p_tc(Vertex_attribs::vertex_p_tc);
		Assert::AreEqual(p_tc.component_offset_position(), 0u);
		Assert::AreEqual(p_tc.component_offset_normal(), Interleaved_vertex_format_old::component_count_position);
		Assert::AreEqual(p_tc.component_offset_tex_coord(), Interleaved_vertex_format_old::component_count_position);
		Assert::AreEqual(p_tc.component_offset_tangent_h(), Interleaved_vertex_format_old::component_count_position
			+ Interleaved_vertex_format_old::component_count_tex_coord);

		Interleaved_vertex_format_old n_th(Vertex_attribs::normal | Vertex_attribs::tangent_space);
		Assert::AreEqual(n_th.component_offset_position(), 0u);
		Assert::AreEqual(n_th.component_offset_normal(), 0u);
		Assert::AreEqual(n_th.component_offset_tex_coord(), Interleaved_vertex_format_old::component_count_normal);
		Assert::AreEqual(n_th.component_offset_tangent_h(), Interleaved_vertex_format_old::component_count_normal);
	}

	TEST_METHOD(equal_operator)
	{
		Interleaved_vertex_format_old fmt0(Vertex_attribs::vertex_p_tc);

		Assert::AreNotEqual(fmt0, Interleaved_vertex_format_old(Vertex_attribs::position));
		Assert::AreEqual(fmt0, Interleaved_vertex_format_old(Vertex_attribs::vertex_p_tc));
	}
};

TEST_CLASS(cg_data_mesh_Mesh_builder) {
public:

	TEST_METHOD(clear)
	{
		Vertex_ts v(float3::unit_xyz);
		Mesh_builder ms(1, 1);

		ms.push_back_vertex(v);
		Assert::AreEqual(size_t(1), ms.vertex_count());
		Assert::AreEqual(size_t(1), ms.index_count());

		ms.clear();
		Assert::AreEqual(size_t(0), ms.vertex_count());
		Assert::AreEqual(size_t(0), ms.index_count());
		Assert::IsTrue(ms.vertices().empty());
		Assert::IsTrue(ms.indices().empty());
	}

	TEST_METHOD(mesh_data)
	{
		// vertices represent a square
		Vertex_ts v0(float3::zero, float3::unit_z, float2::zero, float3(2, 0, 0), float3(0, 2, 0));			// left-bottom
		Vertex_ts v1(float3::unit_x, float3::unit_z, float2::unit_x, float3(2, 0, 0), float3(0, 2, 0));		// right-bottom
		Vertex_ts v2(float3::unit_xy, float3::unit_z, float2::unit_xy, float3(2, 0, 0), float3(0, 2, 0));	// right-top
		Vertex_ts v3(float3::unit_y, float3::unit_z, float2::unit_y, float3(2, 0, 0), float3(0, 2, 0));		// left-top

		Mesh_builder mb(4, 6);
		mb.push_back_triangle(v0, v1, v2);
		mb.push_back_triangle(v2, v3, v0);

		uint32_t expected_index_data[6] = { 0, 1, 2, 2, 3, 0 };

		{ // position attrib
			float expected_vertex_data[12] = {
				0, 0, 0, 
				1, 0, 0,
				1, 1, 0,
				0, 1, 0
			};

			Interleaved_mesh_data<Vertex_attribs::vertex_p> mesh_data = mb.mesh_data<Vertex_attribs::vertex_p>();
			Assert::AreEqual(Vertex_attribs::vertex_p, decltype(mesh_data)::Format::attribs);

			Assert::IsTrue(std::equal(
				mesh_data.vertex_data().cbegin(), mesh_data.vertex_data().cend(),
				std::cbegin(expected_vertex_data),
				[](float a, float b) { return approx_equal<float>(a, b); }));

			Assert::IsTrue(std::equal(
				mesh_data.index_data().cbegin(), mesh_data.index_data().cend(),
				std::cbegin(expected_index_data)));
		}

		{ // position & normal attribs
			float expected_vertex_data[24] = {
				0, 0, 0,  0, 0, 1,
				1, 0, 0,  0, 0, 1,
				1, 1, 0,  0, 0, 1,
				0, 1, 0,  0, 0, 1
			};

			auto mesh_data = mb.mesh_data<Vertex_attribs::vertex_p_n>();
			Assert::AreEqual(Vertex_attribs::vertex_p_n, decltype(mesh_data)::Format::attribs);

			Assert::IsTrue(std::equal(
				mesh_data.vertex_data().cbegin(), mesh_data.vertex_data().cend(),
				std::cbegin(expected_vertex_data),
				[](float a, float b) { return approx_equal<float>(a, b); }));

			Assert::IsTrue(std::equal(
				mesh_data.index_data().cbegin(), mesh_data.index_data().cend(),
				std::cbegin(expected_index_data)));
		}

		{ // position normal & tex_coord attribs
			float expected_vertex_data[32] = {
				0, 0, 0,  0, 0, 1,  0, 0,
				1, 0, 0,  0, 0, 1,  1, 0,
				1, 1, 0,  0, 0, 1,  1, 1,
				0, 1, 0,  0, 0, 1,  0, 1
			};

			auto mesh_data = mb.mesh_data<Vertex_attribs::vertex_p_n_tc>();
			Assert::AreEqual(Vertex_attribs::vertex_p_n_tc, decltype(mesh_data)::Format::attribs);

			Assert::IsTrue(std::equal(
				mesh_data.vertex_data().cbegin(), mesh_data.vertex_data().cend(),
				std::cbegin(expected_vertex_data),
				[](float a, float b) { return approx_equal<float>(a, b); }));

			Assert::IsTrue(std::equal(
				mesh_data.index_data().cbegin(), mesh_data.index_data().cend(),
				std::cbegin(expected_index_data)));
		}

		{ // position & tex_coord attribs
			float expected_vertex_data[20] = {
				0, 0, 0,  0, 0,
				1, 0, 0,  1, 0,
				1, 1, 0,  1, 1,
				0, 1, 0,  0, 1
			};

			auto mesh_data = mb.mesh_data<Vertex_attribs::vertex_p_tc>();
			Assert::AreEqual(Vertex_attribs::vertex_p_tc, decltype(mesh_data)::Format::attribs);

			Assert::IsTrue(std::equal(
				mesh_data.vertex_data().cbegin(), mesh_data.vertex_data().cend(),
				std::cbegin(expected_vertex_data),
				[](float a, float b) { return approx_equal<float>(a, b); }));

			Assert::IsTrue(std::equal(
				mesh_data.index_data().cbegin(), mesh_data.index_data().cend(),
				std::cbegin(expected_index_data)));
		}

		{ // position normal & tex_coord attribs
			float expected_vertex_data[48] = {
				0, 0, 0,  0, 0, 1,  0, 0,  1, 0, 0, 1,
				1, 0, 0,  0, 0, 1,  1, 0,  1, 0, 0, 1,
				1, 1, 0,  0, 0, 1,  1, 1,  1, 0, 0, 1,
				0, 1, 0,  0, 0, 1,  0, 1,  1, 0, 0, 1
			};

			auto mesh_data = mb.mesh_data<Vertex_attribs::vertex_ts>();
			Assert::AreEqual(Vertex_attribs::vertex_ts, decltype(mesh_data)::Format::attribs);

			Assert::IsTrue(std::equal(
				mesh_data.vertex_data().cbegin(), mesh_data.vertex_data().cend(),
				std::cbegin(expected_vertex_data),
				[](float a, float b) { return approx_equal<float>(a, b); }));

			Assert::IsTrue(std::equal(
				mesh_data.index_data().cbegin(), mesh_data.index_data().cend(),
				std::cbegin(expected_index_data)));
		}
	}

	TEST_METHOD(push_back_vertex_triangle)
	{
		// vertices represent a square
		Vertex_ts v0(float3::zero, float3::unit_z, float2::zero);		// left-bottom
		Vertex_ts v1(float3::unit_x, float3::unit_z, float2::unit_x);	// right-bottom
		Vertex_ts v2(float3::unit_xy, float3::unit_z, float2::unit_xy);	// right-top
		Vertex_ts v3(float3::unit_y, float3::unit_z, float2::unit_y);	// left-top

		Vertex_ts expected_vertices[4] = { v0, v1, v2, v3 };
		uint32_t expected_indices[6] = { 0, 1, 2,  2, 3, 0 };
		
		Mesh_builder mb(3, 3);
		mb.push_back_triangle(v0, v1, v2);
		mb.push_back_triangle(v2, v3, v0);
		Assert::AreEqual(size_t(4), mb.vertex_count());
		Assert::AreEqual(size_t(6), mb.index_count());
		Assert::IsTrue(std::equal(mb.vertices().cbegin(), mb.vertices().cend(), std::cbegin(expected_vertices)));
		Assert::IsTrue(std::equal(mb.indices().cbegin(), mb.indices().cend(), std::cbegin(expected_indices)));
	}
};

TEST_CLASS(cg_data_mesh_Funcs) {
public:

	TEST_METHOD(load_mesh_wavefront_rect_negative_vertex_indices)
	{
		using cg::approx_equal;
		using cg::data::Vertex_attribs;
		using cg::data::load_mesh_wavefront;

		uint32_t expected_indices[6] = { 0, 1, 2, 3, 0, 2 };
		{ // positions only
			float expected_data[18] = {
				-2, -1, 0,	 2, -1, 0,	2, 1, 0,  -2, 1, 0
			};

			auto md = load_mesh_wavefront<Vertex_attribs::vertex_p>(Filenames::wavefront_rect_negative_indices_p);

			Assert::IsTrue(std::equal(
				md.vertex_data().cbegin(), md.vertex_data().cend(),
				std::cbegin(expected_data),
				[](float a, float b) { return approx_equal<float>(a, b); }));

			Assert::IsTrue(std::equal(
				md.index_data().cbegin(), md.index_data().cend(), std::cbegin(expected_indices)));
		}

		{ // positions & normals
			float expected_data[24] = {
				-2, -1, 0,	0, 0, 1,
				2, -1, 0,	0, 0, 1,
				2, 1, 0,	0, 0, 1,
				-2, 1, 0,	0, 0, 1
			};

			auto md = load_mesh_wavefront<Vertex_attribs::vertex_p_n>(Filenames::wavefront_rect_negative_indices_pn);

			Assert::IsTrue(std::equal(
				md.vertex_data().cbegin(), md.vertex_data().cend(),
				std::cbegin(expected_data),
				[](float a, float b) { return approx_equal<float>(a, b); }));

			Assert::IsTrue(std::equal(
				md.index_data().cbegin(), md.index_data().cend(), std::cbegin(expected_indices)));
		}

		{ // positions & tex_coords
			float expected_data[20] = {
				-2, -1, 0,	0, 0,
				2, -1, 0,	1, 0,
				2, 1, 0,	1, 1,
				-2, 1, 0,	0, 1
			};

			auto md = load_mesh_wavefront<Vertex_attribs::vertex_p_tc>(Filenames::wavefront_rect_negative_indices_ptc);

			Assert::IsTrue(std::equal(
				md.vertex_data().cbegin(), md.vertex_data().cend(),
				std::cbegin(expected_data),
				[](float a, float b) { return approx_equal<float>(a, b); }));

			Assert::IsTrue(std::equal(
				md.index_data().cbegin(), md.index_data().cend(), std::cbegin(expected_indices)));
		}

		{ // positions, normal & tex_coords
			float expected_data[32] = {
				-2, -1, 0,	0, 0, 1,	0, 0,
				2, -1, 0,	0, 0, 1,	1, 0,
				2, 1, 0,	0, 0, 1,	1, 1,
				-2, 1, 0,	0, 0, 1,	0, 1
			};

			auto md = load_mesh_wavefront<Vertex_attribs::vertex_p_n_tc>(Filenames::wavefront_rect_negative_indices_pntc);

			Assert::IsTrue(std::equal(
				md.vertex_data().cbegin(), md.vertex_data().cend(),
				std::cbegin(expected_data),
				[](float a, float b) { return approx_equal<float>(a, b); }));

			Assert::IsTrue(std::equal(
				md.index_data().cbegin(), md.index_data().cend(), std::cbegin(expected_indices)));
		}

		{ // positions, normal, tex_coords, tangent_h
			float expected_data[48] = {
				-2, -1, 0,	0, 0, 1,	0, 0,	1, 0, 0, 1,
				2, -1, 0,	0, 0, 1,	1, 0,	1, 0, 0, 1,
				2, 1, 0,	0, 0, 1,	1, 1,	1, 0, 0, 1,
				-2, 1, 0,	0, 0, 1,	0, 1,	1, 0, 0, 1
			};

			auto md = load_mesh_wavefront<Vertex_attribs::vertex_ts>(Filenames::wavefront_rect_negative_indices_pntc);

			Assert::IsTrue(std::equal(
				md.vertex_data().cbegin(), md.vertex_data().cend(),
				std::cbegin(expected_data),
				[](float a, float b) { return approx_equal<float>(a, b); }));

			Assert::IsTrue(std::equal(
				md.index_data().cbegin(), md.index_data().cend(), std::cbegin(expected_indices)));
		}
	}

	TEST_METHOD(load_mesh_wavefront_rect_positive_vertex_indices)
	{
		using cg::approx_equal;
		using cg::data::Interleaved_mesh_data;
		using cg::data::Vertex_attribs;
		using cg::data::load_mesh_wavefront;

		uint32_t expected_indices[6] = { 0, 1, 2, 3, 0, 2 };
		{ // positions only
			float expected_data[12] = {
				-2, -1, 0,	 2, -1, 0,	2, 1, 0,  -2, 1, 0
			};
			auto md = load_mesh_wavefront<Vertex_attribs::position>(Filenames::wavefront_rect_positive_indices_p);

			Assert::IsTrue(std::equal(
				md.vertex_data().cbegin(), md.vertex_data().cend(),
				std::cbegin(expected_data),
				[](float a, float b) { return approx_equal<float>(a, b); }));

			Assert::IsTrue(std::equal(
				md.index_data().cbegin(), md.index_data().cend(), std::cbegin(expected_indices)));
		}

		{ // positions & normals
			float expected_data[24] = {
				-2, -1, 0,	0, 0, 1,
				2, -1, 0,	0, 0, 1,
				2, 1, 0,	0, 0, 1,
				-2, 1, 0,	0, 0, 1
			};

			auto md = load_mesh_wavefront<Vertex_attribs::vertex_p_n>(Filenames::wavefront_rect_positive_indices_pn);

			Assert::IsTrue(std::equal(
				md.vertex_data().cbegin(), md.vertex_data().cend(),
				std::cbegin(expected_data),
				[](float a, float b) { return approx_equal<float>(a, b); }));

			Assert::IsTrue(std::equal(
				md.index_data().cbegin(), md.index_data().cend(), std::cbegin(expected_indices)));
		}

		{ // positions & tex_coords
			float expected_data[20] = {
				-2, -1, 0,	0, 0,
				2, -1, 0,	1, 0,
				2, 1, 0,	1, 1,
				-2, 1, 0,	0, 1
			};

			auto md = load_mesh_wavefront<Vertex_attribs::vertex_p_tc>(Filenames::wavefront_rect_positive_indices_ptc);

			Assert::IsTrue(std::equal(
				md.vertex_data().cbegin(), md.vertex_data().cend(),
				std::cbegin(expected_data),
				[](float a, float b) { return approx_equal<float>(a, b); }));

			Assert::IsTrue(std::equal(
				md.index_data().cbegin(), md.index_data().cend(), std::cbegin(expected_indices)));
		}

		{ // positions, normal & tex_coords
			float expected_data[32] = {
				-2, -1, 0,	0, 0, 1,	0, 0,
				2, -1, 0,	0, 0, 1,	1, 0,
				2, 1, 0,	0, 0, 1,	1, 1,
				-2, 1, 0,	0, 0, 1,	0, 1
			};

			auto md = load_mesh_wavefront<Vertex_attribs::vertex_p_n_tc>(Filenames::wavefront_rect_positive_indices_pntc);

			Assert::IsTrue(std::equal(
				md.vertex_data().cbegin(), md.vertex_data().cend(),
				std::cbegin(expected_data),
				[](float a, float b) { return approx_equal<float>(a, b); }));

			Assert::IsTrue(std::equal(
				md.index_data().cbegin(), md.index_data().cend(), std::cbegin(expected_indices)));
		}

		{ // positions, normal, tex_coords, tangent_h
			float expected_data[48] = {
				-2, -1, 0,	0, 0, 1,	0, 0,	1, 0, 0, 1,
				2, -1, 0,	0, 0, 1,	1, 0,	1, 0, 0, 1,
				2, 1, 0,	0, 0, 1,	1, 1,	1, 0, 0, 1,
				-2, 1, 0,	0, 0, 1,	0, 1,	1, 0, 0, 1
			};

			auto md = load_mesh_wavefront<Vertex_attribs::vertex_ts>(Filenames::wavefront_rect_positive_indices_pntc);

			Assert::IsTrue(std::equal(
				md.vertex_data().cbegin(), md.vertex_data().cend(),
				std::cbegin(expected_data),
				[](float a, float b) { return approx_equal<float>(a, b); }));

			Assert::IsTrue(std::equal(
				md.index_data().cbegin(), md.index_data().cend(), std::cbegin(expected_indices)));
		}
	}


	TEST_METHOD(load_mesh_wavefront_old_rect_negative_vertex_indices)
	{
		using cg::approx_equal;
		using cg::data::Interleaved_mesh_data_old;
		using cg::data::Vertex_attribs;
		using cg::data::load_mesh_wavefront_old;

		uint32_t expected_indices[6] = { 0, 1, 2, 3, 4, 5 };
		{ // positions only
			float expected_data[18] = {
				-2, -1, 0,	 2, -1, 0,	2, 1, 0,
				-2,	 1, 0,	-2, -1, 0,	2, 1, 0
			};
			auto md = load_mesh_wavefront_old(Filenames::wavefront_rect_negative_indices_p, Vertex_attribs::position);

			Assert::IsTrue(std::equal(
				md.data().cbegin(), md.data().cend(),
				std::cbegin(expected_data),
				[](float a, float b) { return approx_equal<float>(a, b); }));

			Assert::IsTrue(std::equal(
				md.indices().cbegin(), md.indices().cend(), std::cbegin(expected_indices)));
		}

		{ // positions & normals
			float expected_data[36] = {
				-2, -1, 0, 0, 0, 1,	 2, -1, 0, 0, 0, 1,  2, 1, 0, 0, 0, 1,
				-2,	 1, 0, 0, 0, 1,	-2, -1, 0, 0, 0, 1,  2, 1, 0, 0, 0, 1
			};

			auto md = load_mesh_wavefront_old(Filenames::wavefront_rect_negative_indices_pn,
				Vertex_attribs::position | Vertex_attribs::normal);

			Assert::IsTrue(std::equal(
				md.data().cbegin(), md.data().cend(),
				std::cbegin(expected_data),
				[](float a, float b) { return approx_equal<float>(a, b); }));

			Assert::IsTrue(std::equal(
				md.indices().cbegin(), md.indices().cend(), std::cbegin(expected_indices)));
		}

		{ // positions & tex_coords
			float expected_data[30] = {
				-2, -1, 0, 0, 0,   2, -1, 0, 1, 0,  2, 1, 0, 1, 1,
				-2,	 1, 0, 0, 1,  -2, -1, 0, 0, 0,  2, 1, 0, 1, 1
			};

			auto md = load_mesh_wavefront_old(Filenames::wavefront_rect_negative_indices_ptc, Vertex_attribs::vertex_p_tc);

			Assert::IsTrue(std::equal(
				md.data().cbegin(), md.data().cend(),
				std::cbegin(expected_data),
				[](float a, float b) { return approx_equal<float>(a, b); }));

			Assert::IsTrue(std::equal(
				md.indices().cbegin(), md.indices().cend(), std::cbegin(expected_indices)));
		}

		{ // positions, normal & tex_coords
			float expected_data[48] = {
				-2, -1, 0, 0, 0, 1, 0, 0,   2, -1, 0, 0, 0, 1, 1, 0,  2, 1, 0, 0, 0, 1, 1, 1,
				-2,	 1, 0, 0, 0, 1, 0, 1,  -2, -1, 0, 0, 0, 1, 0, 0,  2, 1, 0, 0, 0, 1, 1, 1
			};

			auto md = load_mesh_wavefront_old(Filenames::wavefront_rect_negative_indices_pntc,
				Vertex_attribs::vertex_p_tc | Vertex_attribs::normal);

			Assert::IsTrue(std::equal(
				md.data().cbegin(), md.data().cend(),
				std::cbegin(expected_data),
				[](float a, float b) { return approx_equal<float>(a, b); }));

			Assert::IsTrue(std::equal(
				md.indices().cbegin(), md.indices().cend(), std::cbegin(expected_indices)));
		}

		{ // positions, normal, tex_coords, tangent_h
			float expected_data[72] = {
				-2, -1, 0,	0, 0, 1,	0, 0,	1, 0, 0, 1,
				2, -1, 0,	0, 0, 1,	1, 0,	1, 0, 0, 1,
				2, 1, 0,	0, 0, 1,	1, 1,	1, 0, 0, 1,
				-2,	 1, 0,	0, 0, 1,	0, 1,	1, 0, 0, 1,
				-2, -1, 0,	0, 0, 1,	0, 0,	1, 0, 0, 1,
				2, 1, 0,	0, 0, 1,	1, 1,	1, 0, 0, 1
			};

			auto md = load_mesh_wavefront_old(Filenames::wavefront_rect_negative_indices_pntc, Vertex_attribs::vertex_ts);

			Assert::IsTrue(std::equal(
				md.data().cbegin(), md.data().cend(),
				std::cbegin(expected_data),
				[](float a, float b) { return approx_equal<float>(a, b); }));

			Assert::IsTrue(std::equal(
				md.indices().cbegin(), md.indices().cend(), std::cbegin(expected_indices)));
		}
	}

	TEST_METHOD(load_mesh_wavefront_old_rect_positive_vertex_indices)
	{
		using cg::approx_equal;
		using cg::data::Interleaved_mesh_data_old;
		using cg::data::Vertex_attribs;
		using cg::data::load_mesh_wavefront_old;

		uint32_t expected_indices[6] = { 0, 1, 2, 3, 4, 5 };
		{ // positions only
			float expected_data[18] = {
				-2, -1, 0,	 2, -1, 0,	2, 1, 0,
				-2,	 1, 0,	-2, -1, 0,	2, 1, 0
			};
			auto md = load_mesh_wavefront_old(Filenames::wavefront_rect_positive_indices_p, Vertex_attribs::position);

			Assert::IsTrue(std::equal(
				md.data().cbegin(), md.data().cend(),
				std::cbegin(expected_data),
				[](float a, float b) { return approx_equal<float>(a, b); }));

			Assert::IsTrue(std::equal(
				md.indices().cbegin(), md.indices().cend(), std::cbegin(expected_indices)));
		}

		{ // positions & normals
			float expected_data[36] = {
				-2, -1, 0, 0, 0, 1,	 2, -1, 0, 0, 0, 1,  2, 1, 0, 0, 0, 1,
				-2,	 1, 0, 0, 0, 1,	-2, -1, 0, 0, 0, 1,  2, 1, 0, 0, 0, 1
			};

			auto md = load_mesh_wavefront_old(Filenames::wavefront_rect_positive_indices_pn,
				Vertex_attribs::position | Vertex_attribs::normal);

			Assert::IsTrue(std::equal(
				md.data().cbegin(), md.data().cend(),
				std::cbegin(expected_data),
				[](float a, float b) { return approx_equal<float>(a, b); }));

			Assert::IsTrue(std::equal(
				md.indices().cbegin(), md.indices().cend(), std::cbegin(expected_indices)));
		}

		{ // positions & tex_coords
			float expected_data[30] = {
				-2, -1, 0, 0, 0,   2, -1, 0, 1, 0,  2, 1, 0, 1, 1,
				-2,	 1, 0, 0, 1,  -2, -1, 0, 0, 0,  2, 1, 0, 1, 1
			};

			auto md = load_mesh_wavefront_old(Filenames::wavefront_rect_positive_indices_ptc, Vertex_attribs::vertex_p_tc);

			Assert::IsTrue(std::equal(
				md.data().cbegin(), md.data().cend(),
				std::cbegin(expected_data),
				[](float a, float b) { return approx_equal<float>(a, b); }));

			Assert::IsTrue(std::equal(
				md.indices().cbegin(), md.indices().cend(), std::cbegin(expected_indices)));
		}

		{ // positions, normal & tex_coords
			float expected_data[48] = {
				-2, -1, 0, 0, 0, 1, 0, 0,   2, -1, 0, 0, 0, 1, 1, 0,  2, 1, 0, 0, 0, 1, 1, 1,
				-2,	 1, 0, 0, 0, 1, 0, 1,  -2, -1, 0, 0, 0, 1, 0, 0,  2, 1, 0, 0, 0, 1, 1, 1
			};

			auto md = load_mesh_wavefront_old(Filenames::wavefront_rect_positive_indices_pntc,
				Vertex_attribs::vertex_p_tc | Vertex_attribs::normal);

			Assert::IsTrue(std::equal(
				md.data().cbegin(), md.data().cend(),
				std::cbegin(expected_data),
				[](float a, float b) { return approx_equal<float>(a, b); }));

			Assert::IsTrue(std::equal(
				md.indices().cbegin(), md.indices().cend(), std::cbegin(expected_indices)));
		}

		{ // positions, normal, tex_coords, tangent_h
			float expected_data[72] = {
				-2, -1, 0,	0, 0, 1,	0, 0,	1, 0, 0, 1,
				2, -1, 0,	0, 0, 1,	1, 0,	1, 0, 0, 1,
				2, 1, 0,	0, 0, 1,	1, 1,	1, 0, 0, 1,
				-2,	 1, 0,	0, 0, 1,	0, 1,	1, 0, 0, 1,
				-2, -1, 0,	0, 0, 1,	0, 0,	1, 0, 0, 1,
				2, 1, 0,	0, 0, 1,	1, 1,	1, 0, 0, 1
			};

			auto md = load_mesh_wavefront_old(Filenames::wavefront_rect_positive_indices_pntc, Vertex_attribs::vertex_ts);

			Assert::IsTrue(std::equal(
				md.data().cbegin(), md.data().cend(),
				std::cbegin(expected_data),
				[](float a, float b) { return approx_equal<float>(a, b); }));

			Assert::IsTrue(std::equal(
				md.indices().cbegin(), md.indices().cend(), std::cbegin(expected_indices)));
		}
	}

};

} // namespace unittest