#include "cg/data/mesh.h"

#include <algorithm>
#include <iterator>
#include "cg/math/math.h"
#include "unittest/math/common_math.h"
#include "CppUnitTest.h"

using cg::float2;
using cg::float3;
using cg::float4;
using cg::approx_equal;
using cg::data::Interleaved_mesh_data;
using cg::data::Interleaved_vertex_format;
using cg::data::Vertex_ts;
using cg::data::Vertex_old;
using cg::data::Vertex_attribs;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace Microsoft { namespace VisualStudio { namespace CppUnitTestFramework {

template<> inline std::wstring ToString<Interleaved_vertex_format>(const Interleaved_vertex_format& t) { RETURN_WIDE_STRING(t); }
template<> inline std::wstring ToString<Vertex_ts>(const Vertex_ts& t) { RETURN_WIDE_STRING(t); }
template<> inline std::wstring ToString<Vertex_old>(const Vertex_old& t) { RETURN_WIDE_STRING(t); }
template<> inline std::wstring ToString<Vertex_attribs>(const Vertex_attribs& t) { RETURN_WIDE_STRING(t); }

}}} // namespace Microsoft::VisualStudio::CppUnitTestFramework


namespace unittest {

TEST_CLASS(cg_data_mesh_Interleaved_mesh_data) {
public:

	TEST_METHOD(ctors)
	{
		Interleaved_mesh_data md0(Vertex_attribs::position);
		Assert::AreEqual(Vertex_attribs::position, md0.attribs());
		Assert::AreEqual(0u, md0.data().capacity());
		Assert::AreEqual(0u, md0.indices().capacity());

		Interleaved_mesh_data md1(Vertex_attribs::mesh_textured, 4, 6);
		Assert::AreEqual(Vertex_attribs::mesh_textured, md1.attribs());
		Assert::AreEqual(md1.data().capacity(),
			4 * (Interleaved_vertex_format::component_count_position + Interleaved_vertex_format::component_count_tex_coord));
		Assert::AreEqual(6u, md1.indices().capacity());
	}

	TEST_METHOD(index_count)
	{
		Interleaved_mesh_data md(Vertex_attribs::position);
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

		Interleaved_mesh_data md(Vertex_attribs::position);
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

			Interleaved_mesh_data md(Vertex_attribs::position);
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

			Interleaved_mesh_data md(Vertex_attribs::position | Vertex_attribs::normal);
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

			Interleaved_mesh_data md(Vertex_attribs::position | Vertex_attribs::normal | Vertex_attribs::tex_coord);
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
				v0.tex_coord.u, v0.tex_coord.v, v0.tangent_h.x, v0.tangent_h.y, v0.tangent_h.z, v0.tangent_h.w,
				
				v1.position.x, v1.position.y, v1.position.z, v1.normal.x, v1.normal.y, v1.normal.z, 
				v1.tex_coord.u, v1.tex_coord.v, v1.tangent_h.x, v1.tangent_h.y, v1.tangent_h.z, v1.tangent_h.w,

				v2.position.x, v2.position.y, v2.position.z, v2.normal.x, v2.normal.y, v2.normal.z, 
				v2.tex_coord.u, v2.tex_coord.v, v2.tangent_h.x, v2.tangent_h.y, v2.tangent_h.z, v2.tangent_h.w,
			};

			Interleaved_mesh_data md(Vertex_attribs::mesh_tangent_space);
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
		Interleaved_mesh_data md(Vertex_attribs::position);
		Assert::AreEqual<size_t>(0, md.vertex_count());

		md.push_back_vertex(Vertex_old());
		Assert::AreEqual<size_t>(1, md.vertex_count());
		
		md.push_back_vertex(Vertex_old());
		Assert::AreEqual<size_t>(2, md.vertex_count());

		md.push_back_vertices(Vertex_old(), Vertex_old(), Vertex_old());
		Assert::AreEqual<size_t>(5, md.vertex_count());
	}
};

TEST_CLASS(cg_data_Interleaved_vertex_fotmat) {
public:

	TEST_METHOD(byte_count)
	{
		Interleaved_vertex_format fmt0(Vertex_attribs::position);
		Assert::AreEqual(fmt0.component_count() * sizeof(float), fmt0.byte_count());

		Interleaved_vertex_format fmt1(Vertex_attribs::mesh_textured);
		Assert::AreEqual(fmt1.component_count() * sizeof(float), fmt1.byte_count());

		Interleaved_vertex_format fmt2(Vertex_attribs::mesh_tangent_space);
		Assert::AreEqual(fmt2.component_count() * sizeof(float), fmt2.byte_count());
	}

	TEST_METHOD(byte_offset)
	{
		Interleaved_vertex_format fmt(Vertex_attribs::mesh_tangent_space);
		
		Assert::AreEqual(fmt.byte_offset_position(), 0u);
		
		Assert::AreEqual(fmt.byte_offset_normal(), 
			Interleaved_vertex_format::component_count_position * sizeof(float));
		
		Assert::AreEqual(fmt.byte_offset_tex_coord(), 
			(Interleaved_vertex_format::component_count_position 
			+ Interleaved_vertex_format::component_count_normal) * sizeof(float));

		Assert::AreEqual(fmt.byte_offset_tangent_h(),
			(Interleaved_vertex_format::component_count_position
				+ Interleaved_vertex_format::component_count_normal
				+ Interleaved_vertex_format::component_count_tex_coord) * sizeof(float));
	}

	TEST_METHOD(ctors)
	{
		Interleaved_vertex_format fmt0;
		Assert::AreEqual(Vertex_attribs::none, fmt0.attribs);

		Interleaved_vertex_format fmt1(Vertex_attribs::normal);
		Assert::AreEqual(Vertex_attribs::normal, fmt1.attribs);

		Interleaved_vertex_format fmt2(Vertex_attribs::mesh_tangent_space);
		Assert::AreEqual(Vertex_attribs::mesh_tangent_space, fmt2.attribs);
	}

	TEST_METHOD(component_count)
	{
		Assert::AreEqual(Interleaved_vertex_format::component_count_position, 
			Interleaved_vertex_format(Vertex_attribs::position).component_count());
		
		Assert::AreEqual(Interleaved_vertex_format::component_count_normal,
			Interleaved_vertex_format(Vertex_attribs::normal).component_count());
		
		Assert::AreEqual(Interleaved_vertex_format::component_count_tex_coord,
			Interleaved_vertex_format(Vertex_attribs::tex_coord).component_count());
		
		Assert::AreEqual(Interleaved_vertex_format::component_count_tangent_h,
			Interleaved_vertex_format(Vertex_attribs::tangent_h).component_count());

		// position & normal
		auto attribs_pn = Vertex_attribs::position | Vertex_attribs::normal;
		size_t expected_pn = Interleaved_vertex_format::component_count_position
			+ Interleaved_vertex_format::component_count_normal;
		Assert::AreEqual(expected_pn, Interleaved_vertex_format(attribs_pn).component_count());

		// position & tex_coord
		size_t expected_ptc = Interleaved_vertex_format::component_count_position
			+ Interleaved_vertex_format::component_count_tex_coord;
		Assert::AreEqual(expected_ptc, Interleaved_vertex_format(Vertex_attribs::mesh_textured).component_count());

		// position, normal & tex_coord
		auto attribs_pntc = Vertex_attribs::position | Vertex_attribs::normal | Vertex_attribs::tex_coord;
		size_t expected_pntc = Interleaved_vertex_format::component_count_position
			+ Interleaved_vertex_format::component_count_normal
			+ Interleaved_vertex_format::component_count_tex_coord;
		Assert::AreEqual(expected_pntc, Interleaved_vertex_format(attribs_pntc).component_count());

		// position, normal, tex_coord & tangnet_h
		size_t expected_pntcth = Interleaved_vertex_format::component_count_position
			+ Interleaved_vertex_format::component_count_normal
			+ Interleaved_vertex_format::component_count_tex_coord
			+ Interleaved_vertex_format::component_count_tangent_h;
		Assert::AreEqual(expected_pntcth, Interleaved_vertex_format(Vertex_attribs::mesh_tangent_space).component_count());

		// tex_coord & tangent_h
		auto attribs_tcth = Vertex_attribs::tex_coord | Vertex_attribs::tangent_h;
		size_t expected_tcth = Interleaved_vertex_format::component_count_tex_coord
			+ Interleaved_vertex_format::component_count_tangent_h;
		Assert::AreEqual(expected_tcth, Interleaved_vertex_format(attribs_tcth).component_count());
	}

	TEST_METHOD(component_offset_all)
	{
		Interleaved_vertex_format p_n_tc_th(Vertex_attribs::mesh_tangent_space);
		Assert::AreEqual(p_n_tc_th.component_offset_position(),	0u);
		Assert::AreEqual(p_n_tc_th.component_offset_normal(), Interleaved_vertex_format::component_count_position);
		Assert::AreEqual(p_n_tc_th.component_offset_tex_coord(), 
			Interleaved_vertex_format::component_count_position + Interleaved_vertex_format::component_count_normal);
		Assert::AreEqual(p_n_tc_th.component_offset_tangent_h(),
			Interleaved_vertex_format::component_count_position + Interleaved_vertex_format::component_count_normal
			+ Interleaved_vertex_format::component_count_tex_coord);

		Interleaved_vertex_format n_tc_th(Vertex_attribs::normal
			| Vertex_attribs::tex_coord | Vertex_attribs::tangent_h);
		Assert::AreEqual(n_tc_th.component_offset_position(), 0u);
		Assert::AreEqual(n_tc_th.component_offset_normal(), 0u);
		Assert::AreEqual(n_tc_th.component_offset_tex_coord(), Interleaved_vertex_format::component_count_normal);
		Assert::AreEqual(n_tc_th.component_offset_tangent_h(), Interleaved_vertex_format::component_count_normal 
			+ Interleaved_vertex_format::component_count_tex_coord);

		Interleaved_vertex_format tc_th(Vertex_attribs::tex_coord | Vertex_attribs::tangent_h);
		Assert::AreEqual(tc_th.component_offset_position(), 0u);
		Assert::AreEqual(tc_th.component_offset_normal(), 0u);
		Assert::AreEqual(tc_th.component_offset_tex_coord(), 0u);
		Assert::AreEqual(tc_th.component_offset_tangent_h(), Interleaved_vertex_format::component_count_tex_coord);

		Interleaved_vertex_format th(Vertex_attribs::tangent_h);
		Assert::AreEqual(th.component_offset_position(), 0u);
		Assert::AreEqual(th.component_offset_normal(), 0u);
		Assert::AreEqual(th.component_offset_tex_coord(), 0u);
		Assert::AreEqual(th.component_offset_tangent_h(), 0u);

		Interleaved_vertex_format p_tc(Vertex_attribs::mesh_textured);
		Assert::AreEqual(p_tc.component_offset_position(), 0u);
		Assert::AreEqual(p_tc.component_offset_normal(), Interleaved_vertex_format::component_count_position);
		Assert::AreEqual(p_tc.component_offset_tex_coord(), Interleaved_vertex_format::component_count_position);
		Assert::AreEqual(p_tc.component_offset_tangent_h(), Interleaved_vertex_format::component_count_position
			+ Interleaved_vertex_format::component_count_tex_coord);

		Interleaved_vertex_format n_th(Vertex_attribs::normal | Vertex_attribs::tangent_h);
		Assert::AreEqual(n_th.component_offset_position(), 0u);
		Assert::AreEqual(n_th.component_offset_normal(), 0u);
		Assert::AreEqual(n_th.component_offset_tex_coord(), Interleaved_vertex_format::component_count_normal);
		Assert::AreEqual(n_th.component_offset_tangent_h(), Interleaved_vertex_format::component_count_normal);
	}

	TEST_METHOD(equal_operator)
	{
		Interleaved_vertex_format fmt0(Vertex_attribs::mesh_textured);

		Assert::AreNotEqual(fmt0, Interleaved_vertex_format(Vertex_attribs::position));
		Assert::AreEqual(fmt0, Interleaved_vertex_format(Vertex_attribs::mesh_textured));
	}
};

TEST_CLASS(cg_data_mesh_Funcs) {
public:

	TEST_METHOD(compute_tangent_h)
	{
		using cg::data::compute_tangent_h;

		// right-handed basis
		float3 normal_positive = float3::unit_z;
		Vertex_old lb0 = Vertex_old(float3(-1, -1, 0), normal_positive, float2::zero); // left-bottom
		Vertex_old rb0 = Vertex_old(float3(1, -1, 0), normal_positive, float2(1, 0)); // right-bottom
		Vertex_old rt0 = Vertex_old(float3(1, 1, 0), normal_positive, float2::unit_xy); // right-top

		float4 tan_right = compute_tangent_h(lb0, rb0, rt0);
		Assert::AreEqual(float4(float3::unit_x, 1.0f), tan_right);

		// left-handed basis
		float3 normal_negative = -float3::unit_z;
		Vertex_old lb1 = Vertex_old(float3(-1, -1, 0), normal_negative, float2::zero); // left-bottom
		Vertex_old rb1 = Vertex_old(float3(1, -1, 0), normal_negative, float2(1, 0)); // right-bottom
		Vertex_old rt1 = Vertex_old(float3(1, 1, 0), normal_negative, float2::unit_xy); // right-top

		float4 tan_left = compute_tangent_h(lb1, rb1, rt1);
		Assert::AreEqual(float4(float3::unit_x, -1.0f), tan_left);
	}

	TEST_METHOD(compute_tangent_bitangent)
	{
		using cg::normalize;
		using cg::data::compute_tangent_bitangent;

		float3 lb_pos0(-1, -1, 0);	float2 lb_tc0 = float2::zero;		// left-bottom
		float3 lb_pos1(1, -1, 0);	float2 lb_tc1 = float2::unit_x;		// right-bottom
		float3 lb_pos2(1, 1, 0);	float2 lb_tc2 = float2::unit_xy;	// right-top
		
		auto tb0 = compute_tangent_bitangent(lb_pos0, lb_tc0, lb_pos1, lb_tc1, lb_pos2, lb_tc2);
		auto expected_tb0 = std::make_pair(float3::unit_x, float3::unit_y);
		Assert::AreEqual(float3::unit_x, normalize(tb0.first));		// tangent
		Assert::AreEqual(float3::unit_y, normalize(tb0.second));	// bitangent
	}

	TEST_METHOD(compute_tangent_handedness)
	{
		using cg::normalize;
		using cg::data::compute_tangent_handedness;
		
		float3 tangent = normalize(float3(1, 0, 1)); // z component is set to 1 to check Gram-Schmidt orthogonalization
		float3 bitangnet = float3::unit_y;
		float3 normal_positive = float3::unit_z;
		float3 normal_negative = -float3::unit_z;

		// right-handed basis
		auto th0 = compute_tangent_handedness(tangent, bitangnet, normal_positive);
		Assert::AreEqual(float4(float3::unit_x, 1.0f), th0);

		// left-handed basis
		auto th1 = compute_tangent_handedness(tangent, bitangnet, normal_negative);
		Assert::AreEqual(float4(float3::unit_x, -1.0f), th1);
	}
};

} // namespace unittest