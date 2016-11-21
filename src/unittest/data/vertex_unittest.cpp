#include "cg/data/vertex.h"

#include "unittest/math/common_math.h"
#include "CppUnitTest.h"

using cg::float2;
using cg::float3;
using cg::float4;
using cg::data::Vertex;
using cg::data::Vertex_ts;
using cg::data::Vertex_old;
using cg::data::Vertex_attribs;
using cg::data::Vertex_interleaved_format;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace Microsoft { namespace VisualStudio { namespace CppUnitTestFramework {

template<> inline std::wstring ToString<Vertex>(const Vertex& t) { RETURN_WIDE_STRING(t); }
template<> inline std::wstring ToString<Vertex_ts>(const Vertex_ts& t) { RETURN_WIDE_STRING(t); }
template<> inline std::wstring ToString<Vertex_old>(const Vertex_old& t) { RETURN_WIDE_STRING(t); }
template<> inline std::wstring ToString<Vertex_attribs>(const Vertex_attribs& t) { RETURN_WIDE_STRING(t); }

}}} // namespace Microsoft::VisualStudio::CppUnitTestFramework


namespace unittest {

TEST_CLASS(cg_data_vertex_Vertex_attribs) {

	TEST_METHOD(assignment_operatos)
	{
		auto a = Vertex_attribs::position;
		a |= Vertex_attribs::normal;

		Assert::AreEqual(Vertex_attribs::position, a & Vertex_attribs::position);
		Assert::AreEqual(Vertex_attribs::normal, a & Vertex_attribs::normal);
		Assert::AreNotEqual(Vertex_attribs::tex_coord, a & Vertex_attribs::tex_coord);
		Assert::AreNotEqual(Vertex_attribs::tangent_space, a & Vertex_attribs::tangent_space);

		auto origin = Vertex_attribs::position | Vertex_attribs::normal;

		a = origin;
		a &= Vertex_attribs::position;
		Assert::AreEqual(Vertex_attribs::position, a & Vertex_attribs::position);
		Assert::AreNotEqual(Vertex_attribs::normal, a & Vertex_attribs::normal);
		Assert::AreNotEqual(Vertex_attribs::tex_coord, a & Vertex_attribs::tex_coord);
		Assert::AreNotEqual(Vertex_attribs::tangent_space, a & Vertex_attribs::tangent_space);

		a = origin;
		a &= Vertex_attribs::normal;
		Assert::AreNotEqual(Vertex_attribs::position, a & Vertex_attribs::position);
		Assert::AreEqual(Vertex_attribs::normal, a & Vertex_attribs::normal);
		Assert::AreNotEqual(Vertex_attribs::tex_coord, a & Vertex_attribs::tex_coord);
		Assert::AreNotEqual(Vertex_attribs::tangent_space, a & Vertex_attribs::tangent_space);

		a = origin;
		a &= Vertex_attribs::tex_coord;
		Assert::AreNotEqual(Vertex_attribs::position, a & Vertex_attribs::position);
		Assert::AreNotEqual(Vertex_attribs::normal, a & Vertex_attribs::normal);
		Assert::AreNotEqual(Vertex_attribs::tex_coord, a & Vertex_attribs::tex_coord);
		Assert::AreNotEqual(Vertex_attribs::tangent_space, a & Vertex_attribs::tangent_space);

		a = origin;
		a &= Vertex_attribs::tangent_space;
		Assert::AreNotEqual(Vertex_attribs::position, a & Vertex_attribs::position);
		Assert::AreNotEqual(Vertex_attribs::normal, a & Vertex_attribs::normal);
		Assert::AreNotEqual(Vertex_attribs::tex_coord, a & Vertex_attribs::tex_coord);
		Assert::AreNotEqual(Vertex_attribs::tangent_space, a & Vertex_attribs::tangent_space);
	}
};

TEST_CLASS(cg_data_vertex_Vertex_interleaved_format) {
public:
		
	TEST_METHOD(Vertex_attrib_none)
	{
		using Fmt = Vertex_interleaved_format<Vertex_attribs::none>;
		Assert::AreEqual(Vertex_attribs::none, Fmt::attribs);
		Assert::AreEqual(size_t(0), Fmt::vertex_component_count);
		Assert::AreEqual(size_t(0), Fmt::vertex_byte_count);
	}

	TEST_METHOD(Vertex_attrib_p)
	{
		using Fmt = Vertex_interleaved_format<Vertex_attribs::vertex_p>;
		Assert::AreEqual(Vertex_attribs::vertex_p, Fmt::attribs);

		Assert::AreEqual(size_t(3), Fmt::position_component_count);
		Assert::AreEqual(size_t(0), Fmt::position_byte_offset);
		
		Assert::AreEqual(size_t(3), Fmt::vertex_component_count);
		Assert::AreEqual(3 * sizeof(float), Fmt::vertex_byte_count);
	}

	TEST_METHOD(Vertex_attrib_p_n)
	{
		using Fmt = Vertex_interleaved_format<Vertex_attribs::vertex_p_n>;
		Assert::AreEqual(Vertex_attribs::vertex_p_n, Fmt::attribs);

		Assert::AreEqual(size_t(3), Fmt::position_component_count);
		Assert::AreEqual(size_t(0), Fmt::position_byte_offset);
		
		Assert::AreEqual(size_t(3), Fmt::normal_component_count);
		Assert::AreEqual(3 * sizeof(float), Fmt::normal_byte_offset);
		
		Assert::AreEqual(size_t(6), Fmt::vertex_component_count);
		Assert::AreEqual(6 * sizeof(float), Fmt::vertex_byte_count);
	}

	TEST_METHOD(Vertex_attrib_p_n_tc)
	{
		using Fmt = Vertex_interleaved_format<Vertex_attribs::vertex_p_n_tc>;
		Assert::AreEqual(Vertex_attribs::vertex_p_n_tc, Fmt::attribs);
		
		Assert::AreEqual(size_t(3), Fmt::position_component_count);
		Assert::AreEqual(size_t(0), Fmt::position_byte_offset);
		
		Assert::AreEqual(size_t(3), Fmt::normal_component_count);
		Assert::AreEqual(3 * sizeof(float), Fmt::normal_byte_offset);
		
		Assert::AreEqual(size_t(2), Fmt::tex_coord_component_count);
		Assert::AreEqual(6 * sizeof(float), Fmt::tex_coord_byte_offset);
		
		Assert::AreEqual(size_t(8), Fmt::vertex_component_count);
		Assert::AreEqual(8 * sizeof(float), Fmt::vertex_byte_count);
	}

	TEST_METHOD(Vertex_attrib_p_tc)
	{
		using Fmt = Vertex_interleaved_format<Vertex_attribs::vertex_p_tc>;
		Assert::AreEqual(Vertex_attribs::vertex_p_tc, Fmt::attribs);

		Assert::AreEqual(size_t(3), Fmt::position_component_count);
		Assert::AreEqual(size_t(0), Fmt::position_byte_offset);

		Assert::AreEqual(size_t(2), Fmt::tex_coord_component_count);
		Assert::AreEqual(3 * sizeof(float), Fmt::tex_coord_byte_offset);

		Assert::AreEqual(size_t(5), Fmt::vertex_component_count);
		Assert::AreEqual(5 * sizeof(float), Fmt::vertex_byte_count);
	}

	TEST_METHOD(Vertex_attrib_ts)
	{
		using Fmt = Vertex_interleaved_format<Vertex_attribs::vertex_ts>;
		Assert::AreEqual(Vertex_attribs::vertex_ts, Fmt::attribs);

		Assert::AreEqual(size_t(3), Fmt::position_component_count);
		Assert::AreEqual(size_t(0), Fmt::position_byte_offset);

		Assert::AreEqual(size_t(3), Fmt::normal_component_count);
		Assert::AreEqual(3 * sizeof(float), Fmt::normal_byte_offset);

		Assert::AreEqual(size_t(2), Fmt::tex_coord_component_count);
		Assert::AreEqual(6 * sizeof(float), Fmt::tex_coord_byte_offset);

		Assert::AreEqual(size_t(4), Fmt::tangent_space_component_count);
		Assert::AreEqual(8 * sizeof(float), Fmt::tangent_space_byte_offset);

		Assert::AreEqual(size_t(12), Fmt::vertex_component_count);
		Assert::AreEqual(12 * sizeof(float), Fmt::vertex_byte_count);
	}
};

TEST_CLASS(cg_data_vertex_Vertex) {
public:

	TEST_METHOD(ctors)
	{
		// default
		Vertex v0;
		Assert::AreEqual(float3::zero, v0.position);
		Assert::AreEqual(float3::zero, v0.normal);
		Assert::AreEqual(float2::zero, v0.tex_coord);

		// position
		Vertex v1(float3(1));
		Assert::AreEqual(float3(1), v1.position);
		Assert::AreEqual(float3::zero, v1.normal);
		Assert::AreEqual(float2::zero, v1.tex_coord);

		// position & normal
		Vertex v2_0(float3(1), float3(2));
		Assert::AreEqual(float3(1), v2_0.position);
		Assert::AreEqual(float3(2), v2_0.normal);
		Assert::AreEqual(float2::zero, v2_0.tex_coord);

		// position & tex_coord
		Vertex v2_1(float3(1), float2(3));
		Assert::AreEqual(float3(1), v2_1.position);
		Assert::AreEqual(float3::zero, v2_1.normal);
		Assert::AreEqual(float2(3), v2_1.tex_coord);

		// position, normal & tex_coord
		Vertex v3(float3(1), float3(2), float2(3));
		Assert::AreEqual(float3(1), v3.position);
		Assert::AreEqual(float3(2), v3.normal);
		Assert::AreEqual(float2(3), v3.tex_coord);

		// copy ctor
		Vertex v4_c = v3;
		Assert::AreEqual(v3.position, v4_c.position);
		Assert::AreEqual(v3.normal, v4_c.normal);
		Assert::AreEqual(v3.tex_coord, v4_c.tex_coord);

		// move ctor
		Vertex v4_m = std::move(v4_c);
		Assert::AreEqual(v3.position, v4_m.position);
		Assert::AreEqual(v3.normal, v4_m.normal);
		Assert::AreEqual(v3.tex_coord, v4_m.tex_coord);
	}

	TEST_METHOD(operator_equal)
	{
		Vertex v(float3(1), float3(2), float2(3));

		Assert::AreNotEqual(v, Vertex(float3(10), float3(2), float2(3)));
		Assert::AreNotEqual(v, Vertex(float3(1), float3(20), float2(3)));
		Assert::AreNotEqual(v, Vertex(float3(1), float3(2), float2(30)));

		Assert::AreEqual(v, Vertex(float3(1), float3(2), float2(3)));
	}
};

TEST_CLASS(cg_data_vertex_Vertex_ts) {
public:

	TEST_METHOD(ctors)
	{
		// default
		Vertex_ts v0;
		Assert::AreEqual(float3::zero, v0.position);
		Assert::AreEqual(float3::zero, v0.normal);
		Assert::AreEqual(float2::zero, v0.tex_coord);
		Assert::AreEqual(float3::zero, v0.tangent);
		Assert::AreEqual(float3::zero, v0.bitangent);

		// position
		Vertex_ts v1(float3(1));
		Assert::AreEqual(float3(1), v1.position);
		Assert::AreEqual(float3::zero, v1.normal);
		Assert::AreEqual(float2::zero, v1.tex_coord);
		Assert::AreEqual(float3::zero, v1.tangent);
		Assert::AreEqual(float3::zero, v1.bitangent);

		// position & normal
		Vertex_ts v2_0(float3(1), float3(2));
		Assert::AreEqual(float3(1), v2_0.position);
		Assert::AreEqual(float3(2), v2_0.normal);
		Assert::AreEqual(float2::zero, v2_0.tex_coord);
		Assert::AreEqual(float3::zero, v2_0.tangent);
		Assert::AreEqual(float3::zero, v2_0.bitangent);

		// position & tex_coord
		Vertex_ts v2_1(float3(1), float2(3));
		Assert::AreEqual(float3(1), v2_1.position);
		Assert::AreEqual(float3::zero, v2_1.normal);
		Assert::AreEqual(float2(3), v2_1.tex_coord);
		Assert::AreEqual(float3::zero, v2_1.tangent);
		Assert::AreEqual(float3::zero, v2_1.bitangent);

		// position, normal & tex_coord
		Vertex_ts v3(float3(1), float3(2), float2(3));
		Assert::AreEqual(float3(1), v3.position);
		Assert::AreEqual(float3(2), v3.normal);
		Assert::AreEqual(float2(3), v3.tex_coord);
		Assert::AreEqual(float3::zero, v3.tangent);
		Assert::AreEqual(float3::zero, v3.bitangent);

		// position, normal, tex_coord & tangent_h
		Vertex_ts v4(float3(1), float3(2), float2(3), float3(4), float3(5));
		Assert::AreEqual(float3(1), v4.position);
		Assert::AreEqual(float3(2), v4.normal);
		Assert::AreEqual(float2(3), v4.tex_coord);
		Assert::AreEqual(float3(4), v4.tangent);
		Assert::AreEqual(float3(5), v4.bitangent);

		// copy ctor
		Vertex_ts v4_c = v4;
		Assert::AreEqual(v4.position, v4_c.position);
		Assert::AreEqual(v4.normal, v4_c.normal);
		Assert::AreEqual(v4.tex_coord, v4_c.tex_coord);
		Assert::AreEqual(v4.tangent, v4_c.tangent);
		Assert::AreEqual(v4.bitangent, v4_c.bitangent);

		// move ctor
		Vertex_ts v4_m = std::move(v4_c);
		Assert::AreEqual(v4.position, v4_m.position);
		Assert::AreEqual(v4.normal, v4_m.normal);
		Assert::AreEqual(v4.tex_coord, v4_m.tex_coord);
		Assert::AreEqual(v4.tangent, v4_m.tangent);
		Assert::AreEqual(v4.bitangent, v4_m.bitangent);
	}

	TEST_METHOD(operator_equal)
	{
		Vertex_ts v(float3(1), float3(2), float2(3), float3(4), float3(5));

		Assert::AreNotEqual(v, Vertex_ts(float3(10), float3(2), float2(3), float3(4), float3(5)));
		Assert::AreNotEqual(v, Vertex_ts(float3(1), float3(20), float2(3), float3(4), float3(5)));
		Assert::AreNotEqual(v, Vertex_ts(float3(1), float3(2), float2(30), float3(4), float3(5)));
		Assert::AreNotEqual(v, Vertex_ts(float3(1), float3(2), float2(3), float3(40), float3(5)));
		Assert::AreNotEqual(v, Vertex_ts(float3(1), float3(2), float2(3), float3(4), float3(50)));

		Assert::AreEqual(v, Vertex_ts(float3(1), float3(2), float2(3), float3(4), float3(5)));
	}
};

TEST_CLASS(cg_data_vertex_Funcs) {
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

	TEST_METHOD(has_vertex_attrib)
	{
		using cg::data::has_position;

		// position
		Assert::IsTrue(has_position(Vertex_attribs::position));
		Assert::IsFalse(has_position(Vertex_attribs::tex_coord));
		Assert::IsFalse(has_position(Vertex_attribs::normal));
		Assert::IsFalse(has_position(Vertex_attribs::tangent_space));

		Assert::IsTrue(has_position(Vertex_attribs::vertex_p));
		Assert::IsTrue(has_position(Vertex_attribs::vertex_p_tc));
		Assert::IsTrue(has_position(Vertex_attribs::vertex_ts));

		// tex_coord
		Assert::IsFalse(has_tex_coord(Vertex_attribs::position));
		Assert::IsTrue(has_tex_coord(Vertex_attribs::tex_coord));
		Assert::IsFalse(has_tex_coord(Vertex_attribs::normal));
		Assert::IsFalse(has_tex_coord(Vertex_attribs::tangent_space));

		Assert::IsFalse(has_tex_coord(Vertex_attribs::vertex_p));
		Assert::IsTrue(has_tex_coord(Vertex_attribs::vertex_p_tc));
		Assert::IsTrue(has_tex_coord(Vertex_attribs::vertex_ts));

		// normal
		Assert::IsFalse(has_normal(Vertex_attribs::position));
		Assert::IsFalse(has_normal(Vertex_attribs::tex_coord));
		Assert::IsTrue(has_normal(Vertex_attribs::normal));
		Assert::IsFalse(has_normal(Vertex_attribs::tangent_space));

		Assert::IsFalse(has_normal(Vertex_attribs::vertex_p));
		Assert::IsFalse(has_normal(Vertex_attribs::vertex_p_tc));
		Assert::IsTrue(has_normal(Vertex_attribs::vertex_ts));

		// tangent_h
		Assert::IsFalse(has_tangent_space(Vertex_attribs::position));
		Assert::IsFalse(has_tangent_space(Vertex_attribs::tex_coord));
		Assert::IsFalse(has_tangent_space(Vertex_attribs::normal));
		Assert::IsTrue(has_tangent_space(Vertex_attribs::tangent_space));

		Assert::IsFalse(has_tangent_space(Vertex_attribs::vertex_p));
		Assert::IsFalse(has_tangent_space(Vertex_attribs::vertex_p_tc));
		Assert::IsTrue(has_tangent_space(Vertex_attribs::vertex_ts));
	}

	TEST_METHOD(is_superset_of)
	{
		using cg::data::is_superset_of;

		Assert::IsTrue(is_superset_of(Vertex_attribs::vertex_p, Vertex_attribs::position));
		Assert::IsFalse(is_superset_of(Vertex_attribs::vertex_p, Vertex_attribs::tex_coord));
		Assert::IsFalse(is_superset_of(Vertex_attribs::vertex_p, Vertex_attribs::normal));
		Assert::IsFalse(is_superset_of(Vertex_attribs::vertex_p, Vertex_attribs::tangent_space));
		Assert::IsTrue(is_superset_of(Vertex_attribs::vertex_p, Vertex_attribs::vertex_p));
		Assert::IsFalse(is_superset_of(Vertex_attribs::vertex_p, Vertex_attribs::vertex_p_tc));
		Assert::IsFalse(is_superset_of(Vertex_attribs::vertex_p, Vertex_attribs::vertex_ts));

		Assert::IsTrue(is_superset_of(Vertex_attribs::vertex_p_tc, Vertex_attribs::position));
		Assert::IsTrue(is_superset_of(Vertex_attribs::vertex_p_tc, Vertex_attribs::tex_coord));
		Assert::IsFalse(is_superset_of(Vertex_attribs::vertex_p_tc, Vertex_attribs::normal));
		Assert::IsFalse(is_superset_of(Vertex_attribs::vertex_p_tc, Vertex_attribs::tangent_space));
		Assert::IsTrue(is_superset_of(Vertex_attribs::vertex_p_tc, Vertex_attribs::vertex_p));
		Assert::IsTrue(is_superset_of(Vertex_attribs::vertex_p_tc, Vertex_attribs::vertex_p_tc));
		Assert::IsFalse(is_superset_of(Vertex_attribs::vertex_p_tc, Vertex_attribs::vertex_ts));

		Assert::IsTrue(is_superset_of(Vertex_attribs::vertex_ts, Vertex_attribs::position));
		Assert::IsTrue(is_superset_of(Vertex_attribs::vertex_ts, Vertex_attribs::tex_coord));
		Assert::IsTrue(is_superset_of(Vertex_attribs::vertex_ts, Vertex_attribs::normal));
		Assert::IsTrue(is_superset_of(Vertex_attribs::vertex_ts, Vertex_attribs::tangent_space));
		Assert::IsTrue(is_superset_of(Vertex_attribs::vertex_ts, Vertex_attribs::vertex_p));
		Assert::IsTrue(is_superset_of(Vertex_attribs::vertex_ts, Vertex_attribs::vertex_p_tc));
		Assert::IsTrue(is_superset_of(Vertex_attribs::vertex_ts, Vertex_attribs::vertex_ts));
	}

	TEST_METHOD(to_array)
	{
		using cg::approx_equal;
		using cg::data::to_array;
		using cg::data::to_array_p;
		using cg::data::to_array_p_n;
		using cg::data::to_array_p_n_tc;
		using cg::data::to_array_p_tc;


		Vertex_ts v(float3(1, 2, 3), float3::unit_z, float2(4, 5), float3(2, 0, 0), float3(0, 2, 0));
		
		{ // position attribute
			float expected_arr[3] = { 1, 2, 3 };
			
			float arr[3];
			to_array_p(v, arr);
			
			Assert::IsTrue(std::equal(
				std::cbegin(arr), std::cend(arr),
				std::cbegin(expected_arr),
				[](float a, float b) { return approx_equal<float>(a, b); }));
		}

		{ // position & normal
			float expected_arr[6] = {
				1, 2, 3,
				0, 0, 1
			};

			float arr[6];
			to_array_p_n(v, arr);

			Assert::IsTrue(std::equal(
				std::cbegin(arr), std::cend(arr),
				std::cbegin(expected_arr),
				[](float a, float b) { return approx_equal<float>(a, b); }));
		}

		{ // position, normal & tex_coord attributes
			float expected_arr[8] = {
				1, 2, 3,
				0, 0, 1,
				4, 5
			};

			float arr[8];
			to_array_p_n_tc(v, arr);

			Assert::IsTrue(std::equal(
				std::cbegin(arr), std::cend(arr),
				std::cbegin(expected_arr),
				[](float a, float b) { return approx_equal<float>(a, b); }));
		}

		{ // position & tex_coord attributes
			float expected_arr[5] = {
				1, 2, 3,
				4, 5
			};

			float arr[5];
			to_array_p_tc(v, arr);

			Assert::IsTrue(std::equal(
				std::cbegin(arr), std::cend(arr),
				std::cbegin(expected_arr),
				[](float a, float b) { return approx_equal<float>(a, b); }));
		}

		{ // all the attributes
			float expected_arr[12] = {
				1, 2, 3,
				0, 0, 1,
				4, 5,
				1, 0, 0, 1
			};

			float arr[12];
			to_array(v, arr);
			
			Assert::IsTrue(std::equal(
				std::cbegin(arr), std::cend(arr),
				std::cbegin(expected_arr),
				[](float a, float b) { return approx_equal<float>(a, b); }));
		}
	}

	TEST_METHOD(to_array_func_templte)
	{
		using cg::approx_equal;
		using cg::data::to_array;
		constexpr float end_guard = -100;

		Vertex_ts v(float3(1, 2, 3), float3::unit_z, float2(4, 5), float3(2, 0, 0), float3(0, 2, 0));

		{ // position attribute
			float expected_arr[4] = { 1, 2, 3, end_guard };

			float arr[4]; arr[3] = end_guard;
			to_array<Vertex_attribs::vertex_p>(v, arr);

			Assert::IsTrue(std::equal(
				std::cbegin(arr), std::cend(arr),
				std::cbegin(expected_arr),
				[](float a, float b) { return approx_equal<float>(a, b); }));
		}

		{ // position & normal
			float expected_arr[7] = {
				1, 2, 3,
				0, 0, 1,
				end_guard
			};

			float arr[7]; arr[6] = end_guard;
			to_array<Vertex_attribs::vertex_p_n>(v, arr);

			Assert::IsTrue(std::equal(
				std::cbegin(arr), std::cend(arr),
				std::cbegin(expected_arr),
				[](float a, float b) { return approx_equal<float>(a, b); }));
		}

		{ // position, normal & tex_coord attributes
			float expected_arr[9] = {
				1, 2, 3,
				0, 0, 1,
				4, 5,
				end_guard
			};

			float arr[9]; arr[8] = end_guard;
			to_array<Vertex_attribs::vertex_p_n_tc>(v, arr);

			Assert::IsTrue(std::equal(
				std::cbegin(arr), std::cend(arr),
				std::cbegin(expected_arr),
				[](float a, float b) { return approx_equal<float>(a, b); }));
		}

		{ // position & tex_coord attributes
			float expected_arr[6] = {
				1, 2, 3,
				4, 5,
				end_guard
			};

			float arr[6]; arr[5] = end_guard;
			to_array<Vertex_attribs::vertex_p_tc>(v, arr);

			Assert::IsTrue(std::equal(
				std::cbegin(arr), std::cend(arr),
				std::cbegin(expected_arr),
				[](float a, float b) { return approx_equal<float>(a, b); }));
		}

		{ // all the attributes
			float expected_arr[13] = {
				1, 2, 3,
				0, 0, 1,
				4, 5,
				1, 0, 0, 1,
				end_guard
			};

			float arr[13]; arr[12] = end_guard;
			to_array<Vertex_attribs::vertex_ts>(v, arr);

			Assert::IsTrue(std::equal(
				std::cbegin(arr), std::cend(arr),
				std::cbegin(expected_arr),
				[](float a, float b) { return approx_equal<float>(a, b); }));
		}
	}
};

} // namespace unittest
