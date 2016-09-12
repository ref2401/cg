#include "cg/data/mesh.h"

#include "cg/math/math.h"
#include "CppUnitTest.h"

using cg::data::Vertex_attribs;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace unittest {

TEST_CLASS(Mesh_unittest) {

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

} // namespace unittest