#include "cg/data/vertex.h"

#include "unittest/base/common_math.h"
#include "CppUnitTest.h"

using cg::data::Vertex_attribs;
using cg::data::Vertex_interleaved_format;
using cg::data::Vertex_interleaved_format_desc;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace Microsoft { namespace VisualStudio { namespace CppUnitTestFramework {

template<> inline std::wstring ToString<Vertex_attribs>(const Vertex_attribs& t) { RETURN_WIDE_STRING(t); }

}}} // namespace Microsoft::VisualStudio::CppUnitTestFramework


namespace unittest {

TEST_CLASS(cg_data_vertex_Vertex_interleaved_format) {
public:

	TEST_METHOD(Vertex_attribs_p)
	{
		using Fmt = Vertex_interleaved_format<Vertex_attribs::p>;
		Assert::AreEqual(Vertex_attribs::p, Fmt::attribs);

		Assert::AreEqual(size_t(3), Fmt::position_component_count);
		Assert::AreEqual(3 * sizeof(float), Fmt::position_byte_count);
		Assert::AreEqual(size_t(0), Fmt::position_byte_offset);

		Assert::AreEqual(size_t(3), Fmt::vertex_component_count);
		Assert::AreEqual(3 * sizeof(float), Fmt::vertex_byte_count);
	}

	TEST_METHOD(Vertex_attribs_p_n)
	{
		using Fmt = Vertex_interleaved_format<Vertex_attribs::p_n>;
		Assert::AreEqual(Vertex_attribs::p_n, Fmt::attribs);

		Assert::AreEqual(size_t(3), Fmt::position_component_count);
		Assert::AreEqual(3 * sizeof(float), Fmt::position_byte_count);
		Assert::AreEqual(size_t(0), Fmt::position_byte_offset);

		Assert::AreEqual(size_t(3), Fmt::normal_component_count);
		Assert::AreEqual(3 * sizeof(float), Fmt::normal_byte_count);
		Assert::AreEqual(Fmt::position_byte_count, Fmt::normal_byte_offset);

		Assert::AreEqual(size_t(6), Fmt::vertex_component_count);
		Assert::AreEqual(6 * sizeof(float), Fmt::vertex_byte_count);
	}

	TEST_METHOD(Vertex_attribs_p_n_tc)
	{
		using Fmt = Vertex_interleaved_format<Vertex_attribs::p_n_tc>;
		Assert::AreEqual(Vertex_attribs::p_n_tc, Fmt::attribs);

		Assert::AreEqual(size_t(3), Fmt::position_component_count);
		Assert::AreEqual(3 * sizeof(float), Fmt::position_byte_count);
		Assert::AreEqual(size_t(0), Fmt::position_byte_offset);

		Assert::AreEqual(size_t(3), Fmt::normal_component_count);
		Assert::AreEqual(3 * sizeof(float), Fmt::normal_byte_count);
		Assert::AreEqual(Fmt::position_byte_count, Fmt::normal_byte_offset);

		Assert::AreEqual(size_t(2), Fmt::tex_coord_component_count);
		Assert::AreEqual(2 * sizeof(float), Fmt::tex_coord_byte_count);
		Assert::AreEqual(Fmt::normal_byte_offset + Fmt::normal_byte_count, Fmt::tex_coord_byte_offset);

		Assert::AreEqual(size_t(8), Fmt::vertex_component_count);
		Assert::AreEqual(8 * sizeof(float), Fmt::vertex_byte_count);
	}

	TEST_METHOD(Vertex_attrib_p_tc)
	{
		using Fmt = Vertex_interleaved_format<Vertex_attribs::p_tc>;
		Assert::AreEqual(Vertex_attribs::p_tc, Fmt::attribs);

		Assert::AreEqual(size_t(3), Fmt::position_component_count);
		Assert::AreEqual(3 * sizeof(float), Fmt::position_byte_count);
		Assert::AreEqual(size_t(0), Fmt::position_byte_offset);

		Assert::AreEqual(size_t(2), Fmt::tex_coord_component_count);
		Assert::AreEqual(2 * sizeof(float), Fmt::tex_coord_byte_count);
		Assert::AreEqual(Fmt::position_byte_count + Fmt::position_byte_offset, Fmt::tex_coord_byte_offset);

		Assert::AreEqual(size_t(5), Fmt::vertex_component_count);
		Assert::AreEqual(5 * sizeof(float), Fmt::vertex_byte_count);
	}

	TEST_METHOD(Vertex_attribs_p_n_tc_ts)
	{
		using Fmt = Vertex_interleaved_format<Vertex_attribs::p_n_tc_ts>;
		Assert::AreEqual(Vertex_attribs::p_n_tc_ts, Fmt::attribs);

		Assert::AreEqual(size_t(3), Fmt::position_component_count);
		Assert::AreEqual(3 * sizeof(float), Fmt::position_byte_count);
		Assert::AreEqual(size_t(0), Fmt::position_byte_offset);

		Assert::AreEqual(size_t(3), Fmt::normal_component_count);
		Assert::AreEqual(3 * sizeof(float), Fmt::normal_byte_count);
		Assert::AreEqual(Fmt::position_byte_count, Fmt::normal_byte_offset);

		Assert::AreEqual(size_t(2), Fmt::tex_coord_component_count);
		Assert::AreEqual(2 * sizeof(float), Fmt::tex_coord_byte_count);
		Assert::AreEqual(Fmt::normal_byte_offset + Fmt::normal_byte_count, Fmt::tex_coord_byte_offset);

		Assert::AreEqual(size_t(4), Fmt::tangent_space_component_count);
		Assert::AreEqual(4 * sizeof(float), Fmt::tangent_space_byte_count);
		Assert::AreEqual(Fmt::tex_coord_byte_count + Fmt::tex_coord_byte_offset, Fmt::tangent_space_byte_offset);

		Assert::AreEqual(size_t(12), Fmt::vertex_component_count);
		Assert::AreEqual(12 * sizeof(float), Fmt::vertex_byte_count);
	}
};

TEST_CLASS(cg_data_vertex_Vertex_interleaved_format_dest) {
public:

	TEST_METHOD(ctors)
	{
		using Format_p = Vertex_interleaved_format<Vertex_attribs::p>;
		Vertex_interleaved_format_desc format_p(Format_p::attribs);
		Assert::AreEqual(Format_p::position_component_count,	format_p.position_component_count);
		Assert::AreEqual(Format_p::position_byte_count,			format_p.position_byte_count);
		Assert::AreEqual(Format_p::position_byte_offset,		format_p.position_byte_offset);
		Assert::AreEqual(Format_p::vertex_component_count,		format_p.vertex_component_count);
		Assert::AreEqual(Format_p::vertex_byte_count,			format_p.vertex_byte_count);

		using Format_p_n = Vertex_interleaved_format<Vertex_attribs::p_n>;
		Vertex_interleaved_format_desc format_p_n(Format_p_n::attribs);
		Assert::AreEqual(Format_p_n::position_component_count,	format_p_n.position_component_count);
		Assert::AreEqual(Format_p_n::position_byte_count,		format_p_n.position_byte_count);
		Assert::AreEqual(Format_p_n::position_byte_offset,		format_p_n.position_byte_offset);
		Assert::AreEqual(Format_p_n::normal_component_count,	format_p_n.normal_component_count);
		Assert::AreEqual(Format_p_n::normal_byte_count,			format_p_n.normal_byte_count);
		Assert::AreEqual(Format_p_n::normal_byte_offset,		format_p_n.normal_byte_offset);
		Assert::AreEqual(Format_p_n::vertex_component_count,	format_p_n.vertex_component_count);
		Assert::AreEqual(Format_p_n::vertex_byte_count,			format_p_n.vertex_byte_count);

		using Format_p_n_tc = Vertex_interleaved_format<Vertex_attribs::p_n_tc>;
		Vertex_interleaved_format_desc format_p_n_tc(Format_p_n_tc::attribs);
		Assert::AreEqual(Format_p_n_tc::position_component_count,	format_p_n_tc.position_component_count);
		Assert::AreEqual(Format_p_n_tc::position_byte_count,		format_p_n_tc.position_byte_count);
		Assert::AreEqual(Format_p_n_tc::position_byte_offset,		format_p_n_tc.position_byte_offset);
		Assert::AreEqual(Format_p_n_tc::normal_component_count,		format_p_n_tc.normal_component_count);
		Assert::AreEqual(Format_p_n_tc::normal_byte_count,			format_p_n_tc.normal_byte_count);
		Assert::AreEqual(Format_p_n_tc::normal_byte_offset,			format_p_n_tc.normal_byte_offset);
		Assert::AreEqual(Format_p_n_tc::tex_coord_component_count,	format_p_n_tc.tex_coord_component_count);
		Assert::AreEqual(Format_p_n_tc::tex_coord_byte_count,		format_p_n_tc.tex_coord_byte_count);
		Assert::AreEqual(Format_p_n_tc::tex_coord_byte_offset,		format_p_n_tc.tex_coord_byte_offset);
		Assert::AreEqual(Format_p_n_tc::vertex_component_count,		format_p_n_tc.vertex_component_count);
		Assert::AreEqual(Format_p_n_tc::vertex_byte_count,			format_p_n_tc.vertex_byte_count);

		using Format_p_tc = Vertex_interleaved_format<Vertex_attribs::p_tc>;
		Vertex_interleaved_format_desc format_p_tc(Format_p_tc::attribs);
		Assert::AreEqual(Format_p_tc::position_component_count,		format_p_tc.position_component_count);
		Assert::AreEqual(Format_p_tc::position_byte_count,			format_p_tc.position_byte_count);
		Assert::AreEqual(Format_p_tc::position_byte_offset,			format_p_tc.position_byte_offset);
		Assert::AreEqual(Format_p_tc::tex_coord_component_count,	format_p_tc.tex_coord_component_count);
		Assert::AreEqual(Format_p_tc::tex_coord_byte_count,			format_p_tc.tex_coord_byte_count);
		Assert::AreEqual(Format_p_tc::tex_coord_byte_offset,		format_p_tc.tex_coord_byte_offset);
		Assert::AreEqual(Format_p_tc::vertex_component_count,		format_p_tc.vertex_component_count);
		Assert::AreEqual(Format_p_tc::vertex_byte_count,			format_p_tc.vertex_byte_count);

		using Format_p_n_tc_ts = Vertex_interleaved_format<Vertex_attribs::p_n_tc_ts>;
		Vertex_interleaved_format_desc format_p_n_tc_ts(Format_p_n_tc_ts::attribs);
		Assert::AreEqual(Format_p_n_tc_ts::position_component_count,		format_p_n_tc_ts.position_component_count);
		Assert::AreEqual(Format_p_n_tc_ts::position_byte_count,				format_p_n_tc_ts.position_byte_count);
		Assert::AreEqual(Format_p_n_tc_ts::position_byte_offset,			format_p_n_tc_ts.position_byte_offset);
		Assert::AreEqual(Format_p_n_tc_ts::normal_component_count,			format_p_n_tc_ts.normal_component_count);
		Assert::AreEqual(Format_p_n_tc_ts::normal_byte_count,				format_p_n_tc_ts.normal_byte_count);
		Assert::AreEqual(Format_p_n_tc_ts::normal_byte_offset,				format_p_n_tc_ts.normal_byte_offset);
		Assert::AreEqual(Format_p_n_tc_ts::tex_coord_component_count,		format_p_n_tc_ts.tex_coord_component_count);
		Assert::AreEqual(Format_p_n_tc_ts::tex_coord_byte_count,			format_p_n_tc_ts.tex_coord_byte_count);
		Assert::AreEqual(Format_p_n_tc_ts::tex_coord_byte_offset,			format_p_n_tc_ts.tex_coord_byte_offset);
		Assert::AreEqual(Format_p_n_tc_ts::tangent_space_component_count,	format_p_n_tc_ts.tangent_space_component_count);
		Assert::AreEqual(Format_p_n_tc_ts::tangent_space_byte_count,		format_p_n_tc_ts.tangent_space_byte_count);
		Assert::AreEqual(Format_p_n_tc_ts::tangent_space_byte_offset,		format_p_n_tc_ts.tangent_space_byte_offset);
		Assert::AreEqual(Format_p_n_tc_ts::vertex_component_count,			format_p_n_tc_ts.vertex_component_count);
		Assert::AreEqual(Format_p_n_tc_ts::vertex_byte_count,				format_p_n_tc_ts.vertex_byte_count);
	}
};

TEST_CLASS(cg_data_vertex_Funcs) {
public:

	TEST_METHOD(compute_tangent_bitangent)
	{
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
		using cg::data::compute_tangent_handedness;

		float3 tangent = normalize(float3(1, 0, 1)); // z component is set to 1 to check Gram-Schmidt orthogonalization
		float3 bitangnet = float3::unit_y;
		float3 normal_positive = float3::unit_z;
		float3 normal_negative = -float3::unit_z;

		// right-handed basis
		auto th0 = compute_tangent_handedness(tangent, bitangnet, normal_positive);
		Assert::IsTrue(approx_equal(th0, float4(float3::unit_x, 1.0f)));

		// left-handed basis
		auto th1 = compute_tangent_handedness(tangent, bitangnet, normal_negative);
		Assert::IsTrue(approx_equal(th1, float4(float3::unit_x, -1.0f)));
	}

	TEST_METHOD(has_vertex_attribs)
	{
		using cg::data::has_normal;
		using cg::data::has_tangent_space;
		using cg::data::has_tex_coord;

		Assert::IsFalse(has_normal(Vertex_attribs::p));
		Assert::IsFalse(has_tex_coord(Vertex_attribs::p));
		Assert::IsFalse(has_tangent_space(Vertex_attribs::p));

		Assert::IsTrue(has_normal(Vertex_attribs::p_n));
		Assert::IsFalse(has_tex_coord(Vertex_attribs::p_n));
		Assert::IsFalse(has_tangent_space(Vertex_attribs::p_n));

		Assert::IsFalse(has_normal(Vertex_attribs::p_tc));
		Assert::IsTrue(has_tex_coord(Vertex_attribs::p_tc));
		Assert::IsFalse(has_tangent_space(Vertex_attribs::p_tc));

		Assert::IsTrue(has_normal(Vertex_attribs::p_n_tc));
		Assert::IsTrue(has_tex_coord(Vertex_attribs::p_n_tc));
		Assert::IsFalse(has_tangent_space(Vertex_attribs::p_n_tc));

		Assert::IsTrue(has_normal(Vertex_attribs::p_n_tc_ts));
		Assert::IsTrue(has_tex_coord(Vertex_attribs::p_n_tc_ts));
		Assert::IsTrue(has_tangent_space(Vertex_attribs::p_n_tc_ts));
	}

	TEST_METHOD(is_superset_of)
	{
		using cg::data::is_superset_of;

		Assert::IsTrue(is_superset_of(Vertex_attribs::p, Vertex_attribs::p));
		Assert::IsFalse(is_superset_of(Vertex_attribs::p, Vertex_attribs::p_n));
		Assert::IsFalse(is_superset_of(Vertex_attribs::p, Vertex_attribs::p_tc));
		Assert::IsFalse(is_superset_of(Vertex_attribs::p, Vertex_attribs::p_n_tc));
		Assert::IsFalse(is_superset_of(Vertex_attribs::p, Vertex_attribs::p_n_tc_ts));

		Assert::IsTrue(is_superset_of(Vertex_attribs::p_n, Vertex_attribs::p));
		Assert::IsTrue(is_superset_of(Vertex_attribs::p_n, Vertex_attribs::p_n));
		Assert::IsFalse(is_superset_of(Vertex_attribs::p_n, Vertex_attribs::p_tc));
		Assert::IsFalse(is_superset_of(Vertex_attribs::p_n, Vertex_attribs::p_n_tc));
		Assert::IsFalse(is_superset_of(Vertex_attribs::p_n, Vertex_attribs::p_n_tc_ts));

		Assert::IsTrue(is_superset_of(Vertex_attribs::p_tc, Vertex_attribs::p));
		Assert::IsFalse(is_superset_of(Vertex_attribs::p_tc, Vertex_attribs::p_n));
		Assert::IsTrue(is_superset_of(Vertex_attribs::p_tc, Vertex_attribs::p_tc));
		Assert::IsFalse(is_superset_of(Vertex_attribs::p_tc, Vertex_attribs::p_n_tc));
		Assert::IsFalse(is_superset_of(Vertex_attribs::p_tc, Vertex_attribs::p_n_tc_ts));

		Assert::IsTrue(is_superset_of(Vertex_attribs::p_n_tc, Vertex_attribs::p));
		Assert::IsTrue(is_superset_of(Vertex_attribs::p_n_tc, Vertex_attribs::p_n));
		Assert::IsTrue(is_superset_of(Vertex_attribs::p_n_tc, Vertex_attribs::p_tc));
		Assert::IsTrue(is_superset_of(Vertex_attribs::p_n_tc, Vertex_attribs::p_n_tc));
		Assert::IsFalse(is_superset_of(Vertex_attribs::p_n_tc, Vertex_attribs::p_n_tc_ts));

		Assert::IsTrue(is_superset_of(Vertex_attribs::p_n_tc_ts, Vertex_attribs::p));
		Assert::IsTrue(is_superset_of(Vertex_attribs::p_n_tc_ts, Vertex_attribs::p_n));
		Assert::IsTrue(is_superset_of(Vertex_attribs::p_n_tc_ts, Vertex_attribs::p_tc));
		Assert::IsTrue(is_superset_of(Vertex_attribs::p_n_tc_ts, Vertex_attribs::p_n_tc));
		Assert::IsTrue(is_superset_of(Vertex_attribs::p_n_tc_ts, Vertex_attribs::p_n_tc_ts));
	}

};

} // namespace unittest
