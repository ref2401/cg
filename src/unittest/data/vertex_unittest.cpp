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
		Assert::AreNotEqual(Vertex_attribs::tangent_h, a & Vertex_attribs::tangent_h);

		auto origin = Vertex_attribs::position | Vertex_attribs::normal;

		a = origin;
		a &= Vertex_attribs::position;
		Assert::AreEqual(Vertex_attribs::position, a & Vertex_attribs::position);
		Assert::AreNotEqual(Vertex_attribs::normal, a & Vertex_attribs::normal);
		Assert::AreNotEqual(Vertex_attribs::tex_coord, a & Vertex_attribs::tex_coord);
		Assert::AreNotEqual(Vertex_attribs::tangent_h, a & Vertex_attribs::tangent_h);

		a = origin;
		a &= Vertex_attribs::normal;
		Assert::AreNotEqual(Vertex_attribs::position, a & Vertex_attribs::position);
		Assert::AreEqual(Vertex_attribs::normal, a & Vertex_attribs::normal);
		Assert::AreNotEqual(Vertex_attribs::tex_coord, a & Vertex_attribs::tex_coord);
		Assert::AreNotEqual(Vertex_attribs::tangent_h, a & Vertex_attribs::tangent_h);

		a = origin;
		a &= Vertex_attribs::tex_coord;
		Assert::AreNotEqual(Vertex_attribs::position, a & Vertex_attribs::position);
		Assert::AreNotEqual(Vertex_attribs::normal, a & Vertex_attribs::normal);
		Assert::AreNotEqual(Vertex_attribs::tex_coord, a & Vertex_attribs::tex_coord);
		Assert::AreNotEqual(Vertex_attribs::tangent_h, a & Vertex_attribs::tangent_h);

		a = origin;
		a &= Vertex_attribs::tangent_h;
		Assert::AreNotEqual(Vertex_attribs::position, a & Vertex_attribs::position);
		Assert::AreNotEqual(Vertex_attribs::normal, a & Vertex_attribs::normal);
		Assert::AreNotEqual(Vertex_attribs::tex_coord, a & Vertex_attribs::tex_coord);
		Assert::AreNotEqual(Vertex_attribs::tangent_h, a & Vertex_attribs::tangent_h);
	}

	TEST_METHOD(has_vertex_attrib)
	{
		using cg::data::has_position;

		// position
		Assert::IsTrue(has_position(Vertex_attribs::position));
		Assert::IsFalse(has_position(Vertex_attribs::tex_coord));
		Assert::IsFalse(has_position(Vertex_attribs::normal));
		Assert::IsFalse(has_position(Vertex_attribs::tangent_h));

		Assert::IsTrue(has_position(Vertex_attribs::mesh_position));
		Assert::IsTrue(has_position(Vertex_attribs::mesh_textured));
		Assert::IsTrue(has_position(Vertex_attribs::mesh_tangent_space));

		// tex_coord
		Assert::IsFalse(has_tex_coord(Vertex_attribs::position));
		Assert::IsTrue(has_tex_coord(Vertex_attribs::tex_coord));
		Assert::IsFalse(has_tex_coord(Vertex_attribs::normal));
		Assert::IsFalse(has_tex_coord(Vertex_attribs::tangent_h));

		Assert::IsFalse(has_tex_coord(Vertex_attribs::mesh_position));
		Assert::IsTrue(has_tex_coord(Vertex_attribs::mesh_textured));
		Assert::IsTrue(has_tex_coord(Vertex_attribs::mesh_tangent_space));

		// normal
		Assert::IsFalse(has_normal(Vertex_attribs::position));
		Assert::IsFalse(has_normal(Vertex_attribs::tex_coord));
		Assert::IsTrue(has_normal(Vertex_attribs::normal));
		Assert::IsFalse(has_normal(Vertex_attribs::tangent_h));

		Assert::IsFalse(has_normal(Vertex_attribs::mesh_position));
		Assert::IsFalse(has_normal(Vertex_attribs::mesh_textured));
		Assert::IsTrue(has_normal(Vertex_attribs::mesh_tangent_space));

		// tangent_h
		Assert::IsFalse(has_tangent_space(Vertex_attribs::position));
		Assert::IsFalse(has_tangent_space(Vertex_attribs::tex_coord));
		Assert::IsFalse(has_tangent_space(Vertex_attribs::normal));
		Assert::IsTrue(has_tangent_space(Vertex_attribs::tangent_h));

		Assert::IsFalse(has_tangent_space(Vertex_attribs::mesh_position));
		Assert::IsFalse(has_tangent_space(Vertex_attribs::mesh_textured));
		Assert::IsTrue(has_tangent_space(Vertex_attribs::mesh_tangent_space));
	}

	TEST_METHOD(is_superset_of)
	{
		using cg::data::is_superset_of;

		Assert::IsTrue(is_superset_of(Vertex_attribs::mesh_position, Vertex_attribs::position));
		Assert::IsFalse(is_superset_of(Vertex_attribs::mesh_position, Vertex_attribs::tex_coord));
		Assert::IsFalse(is_superset_of(Vertex_attribs::mesh_position, Vertex_attribs::normal));
		Assert::IsFalse(is_superset_of(Vertex_attribs::mesh_position, Vertex_attribs::tangent_h));
		Assert::IsTrue(is_superset_of(Vertex_attribs::mesh_position, Vertex_attribs::mesh_position));
		Assert::IsFalse(is_superset_of(Vertex_attribs::mesh_position, Vertex_attribs::mesh_textured));
		Assert::IsFalse(is_superset_of(Vertex_attribs::mesh_position, Vertex_attribs::mesh_tangent_space));

		Assert::IsTrue(is_superset_of(Vertex_attribs::mesh_textured, Vertex_attribs::position));
		Assert::IsTrue(is_superset_of(Vertex_attribs::mesh_textured, Vertex_attribs::tex_coord));
		Assert::IsFalse(is_superset_of(Vertex_attribs::mesh_textured, Vertex_attribs::normal));
		Assert::IsFalse(is_superset_of(Vertex_attribs::mesh_textured, Vertex_attribs::tangent_h));
		Assert::IsTrue(is_superset_of(Vertex_attribs::mesh_textured, Vertex_attribs::mesh_position));
		Assert::IsTrue(is_superset_of(Vertex_attribs::mesh_textured, Vertex_attribs::mesh_textured));
		Assert::IsFalse(is_superset_of(Vertex_attribs::mesh_textured, Vertex_attribs::mesh_tangent_space));

		Assert::IsTrue(is_superset_of(Vertex_attribs::mesh_tangent_space, Vertex_attribs::position));
		Assert::IsTrue(is_superset_of(Vertex_attribs::mesh_tangent_space, Vertex_attribs::tex_coord));
		Assert::IsTrue(is_superset_of(Vertex_attribs::mesh_tangent_space, Vertex_attribs::normal));
		Assert::IsTrue(is_superset_of(Vertex_attribs::mesh_tangent_space, Vertex_attribs::tangent_h));
		Assert::IsTrue(is_superset_of(Vertex_attribs::mesh_tangent_space, Vertex_attribs::mesh_position));
		Assert::IsTrue(is_superset_of(Vertex_attribs::mesh_tangent_space, Vertex_attribs::mesh_textured));
		Assert::IsTrue(is_superset_of(Vertex_attribs::mesh_tangent_space, Vertex_attribs::mesh_tangent_space));
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

} // namespace unittest
