#include "cg/data/mesh.h"

#include "cg/math/math.h"
#include "unittest/common.h"
#include "CppUnitTest.h"

using cg::float2;
using cg::float3;
using cg::float4;
using cg::data::Vertex;
using cg::data::Vertex_attribs;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace unittest {

TEST_CLASS(cg_data_mesh_Vertex) {
public:

	TEST_METHOD(ctors)
	{
		// default
		Vertex v0;
		Assert::AreEqual(float3::zero, v0.position);
		Assert::AreEqual(float3::zero, v0.normal);
		Assert::AreEqual(float2::zero, v0.tex_coord);
		Assert::AreEqual(float4::zero, v0.tangent_h);

		// position
		Vertex v1(float3(1));
		Assert::AreEqual(float3(1), v1.position);
		Assert::AreEqual(float3::zero, v1.normal);
		Assert::AreEqual(float2::zero, v1.tex_coord);
		Assert::AreEqual(float4::zero, v1.tangent_h);

		// position & normal
		Vertex v2_0(float3(1), float3(2));
		Assert::AreEqual(float3(1), v2_0.position);
		Assert::AreEqual(float3(2), v2_0.normal);
		Assert::AreEqual(float2::zero, v2_0.tex_coord);
		Assert::AreEqual(float4::zero, v2_0.tangent_h);

		// position & tex_coord
		Vertex v2_1(float3(1), float2(3));
		Assert::AreEqual(float3(1), v2_1.position);
		Assert::AreEqual(float3::zero, v2_1.normal);
		Assert::AreEqual(float2(3), v2_1.tex_coord);
		Assert::AreEqual(float4::zero, v2_1.tangent_h);

		// position, normal & tex_coord
		Vertex v3(float3(1), float3(2), float2(3));
		Assert::AreEqual(float3(1), v3.position);
		Assert::AreEqual(float3(2), v3.normal);
		Assert::AreEqual(float2(3), v3.tex_coord);
		Assert::AreEqual(float4::zero, v3.tangent_h);

		// position, normal, tex_coord & tangent_h
		Vertex v4(float3(1), float3(2), float2(3), float4(4));
		Assert::AreEqual(float3(1), v4.position);
		Assert::AreEqual(float3(2), v4.normal);
		Assert::AreEqual(float2(3), v4.tex_coord);
		Assert::AreEqual(float4(4), v4.tangent_h);

		// copy ctor
		Vertex v4_c = v4;
		Assert::AreEqual(v4.position, v4_c.position);
		Assert::AreEqual(v4.normal, v4_c.normal);
		Assert::AreEqual(v4.tex_coord, v4_c.tex_coord);
		Assert::AreEqual(v4.tangent_h, v4_c.tangent_h);

		// move ctor
		Vertex v4_m = std::move(v4);
		Assert::AreEqual(v4.position, v4_m.position);
		Assert::AreEqual(v4.normal, v4_m.normal);
		Assert::AreEqual(v4.tex_coord, v4_m.tex_coord);
		Assert::AreEqual(v4.tangent_h, v4_m.tangent_h);
	}

	TEST_METHOD(operator_equal)
	{
		Vertex v(float3(1), float3(2), float2(3), float4(4));

		Assert::AreNotEqual(v, Vertex(float3(10), float3(2), float2(3), float4(4)));
		Assert::AreNotEqual(v, Vertex(float3(1), float3(20), float2(3), float4(4)));
		Assert::AreNotEqual(v, Vertex(float3(1), float3(2), float2(30), float4(4)));
		Assert::AreNotEqual(v, Vertex(float3(1), float3(2), float2(3), float4(40)));

		Assert::AreEqual(v, Vertex(float3(1), float3(2), float2(3), float4(4)));
	}
};

TEST_CLASS(cg_data_mesh_Vertex_attribs) {

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
		Assert::IsTrue(has_position(Vertex_attribs::mesh_tangent_h));

		// tex_coord
		Assert::IsFalse(has_tex_coord(Vertex_attribs::position));
		Assert::IsTrue(has_tex_coord(Vertex_attribs::tex_coord));
		Assert::IsFalse(has_tex_coord(Vertex_attribs::normal));
		Assert::IsFalse(has_tex_coord(Vertex_attribs::tangent_h));

		Assert::IsFalse(has_tex_coord(Vertex_attribs::mesh_position));
		Assert::IsTrue(has_tex_coord(Vertex_attribs::mesh_textured));
		Assert::IsTrue(has_tex_coord(Vertex_attribs::mesh_tangent_h));

		// normal
		Assert::IsFalse(has_normal(Vertex_attribs::position));
		Assert::IsFalse(has_normal(Vertex_attribs::tex_coord));
		Assert::IsTrue(has_normal(Vertex_attribs::normal));
		Assert::IsFalse(has_normal(Vertex_attribs::tangent_h));

		Assert::IsFalse(has_normal(Vertex_attribs::mesh_position));
		Assert::IsFalse(has_normal(Vertex_attribs::mesh_textured));
		Assert::IsTrue(has_normal(Vertex_attribs::mesh_tangent_h));

		// tangent_h
		Assert::IsFalse(has_tangent_h(Vertex_attribs::position));
		Assert::IsFalse(has_tangent_h(Vertex_attribs::tex_coord));
		Assert::IsFalse(has_tangent_h(Vertex_attribs::normal));
		Assert::IsTrue(has_tangent_h(Vertex_attribs::tangent_h));

		Assert::IsFalse(has_tangent_h(Vertex_attribs::mesh_position));
		Assert::IsFalse(has_tangent_h(Vertex_attribs::mesh_textured));
		Assert::IsTrue(has_tangent_h(Vertex_attribs::mesh_tangent_h));
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
		Assert::IsFalse(is_superset_of(Vertex_attribs::mesh_position, Vertex_attribs::mesh_tangent_h));

		Assert::IsTrue(is_superset_of(Vertex_attribs::mesh_textured, Vertex_attribs::position));
		Assert::IsTrue(is_superset_of(Vertex_attribs::mesh_textured, Vertex_attribs::tex_coord));
		Assert::IsFalse(is_superset_of(Vertex_attribs::mesh_textured, Vertex_attribs::normal));
		Assert::IsFalse(is_superset_of(Vertex_attribs::mesh_textured, Vertex_attribs::tangent_h));
		Assert::IsTrue(is_superset_of(Vertex_attribs::mesh_textured, Vertex_attribs::mesh_position));
		Assert::IsTrue(is_superset_of(Vertex_attribs::mesh_textured, Vertex_attribs::mesh_textured));
		Assert::IsFalse(is_superset_of(Vertex_attribs::mesh_textured, Vertex_attribs::mesh_tangent_h));

		Assert::IsTrue(is_superset_of(Vertex_attribs::mesh_tangent_h, Vertex_attribs::position));
		Assert::IsTrue(is_superset_of(Vertex_attribs::mesh_tangent_h, Vertex_attribs::tex_coord));
		Assert::IsTrue(is_superset_of(Vertex_attribs::mesh_tangent_h, Vertex_attribs::normal));
		Assert::IsTrue(is_superset_of(Vertex_attribs::mesh_tangent_h, Vertex_attribs::tangent_h));
		Assert::IsTrue(is_superset_of(Vertex_attribs::mesh_tangent_h, Vertex_attribs::mesh_position));
		Assert::IsTrue(is_superset_of(Vertex_attribs::mesh_tangent_h, Vertex_attribs::mesh_textured));
		Assert::IsTrue(is_superset_of(Vertex_attribs::mesh_tangent_h, Vertex_attribs::mesh_tangent_h));
	}
};

TEST_CLASS(cg_data_mesh_Funcs) {
public:

	TEST_METHOD(compute_tangent_h)
	{
		using cg::data::compute_tanget_h;

		// right-handed basis
		float3 normal_positive = float3::unit_z;
		Vertex lb0 = Vertex(float3(-1, -1, 0), normal_positive, float2::zero); // left-bottom
		Vertex rb0 = Vertex(float3(1, -1, 0), normal_positive, float2(1, 0)); // right-bottom
		Vertex rt0 = Vertex(float3(1, 1, 0), normal_positive, float2::unit_xy); // right-top

		float4 tan_right = compute_tanget_h(lb0, rb0, rt0);
		Assert::AreEqual(float4(float3::unit_x, 1.f), tan_right);

		// left-handed basis
		float3 normal_negative = -float3::unit_z;
		Vertex lb1 = Vertex(float3(-1, -1, 0), normal_negative, float2::zero); // left-bottom
		Vertex rb1 = Vertex(float3(1, -1, 0), normal_negative, float2(1, 0)); // right-bottom
		Vertex rt1 = Vertex(float3(1, 1, 0), normal_negative, float2::unit_xy); // right-top

		float4 tan_left = compute_tanget_h(lb1, rb1, rt1);
		Assert::AreEqual(float4(float3::unit_x, -1.f), tan_left);
	}
};

} // namespace unittest