#include "cg/opengl/shader.h"

#include "cg/opengl/utility.h"
#include "CppUnitTest.h"

using cg::opengl::Invalid;
using cg::opengl::Vertex_attrib_layout;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace unittest {

TEST_CLASS(cg_opengl_shader_Vertex_attrib_layout) {
public:

	TEST_METHOD(ctors)
	{
		Vertex_attrib_layout al0;
		Assert::AreEqual(Invalid::vertex_attrib_location, al0.position_location);
		Assert::AreEqual(Invalid::vertex_attrib_location, al0.normal_location);
		Assert::AreEqual(Invalid::vertex_attrib_location, al0.tex_coord_location);
		Assert::AreEqual(Invalid::vertex_attrib_location, al0.tangent_h_location);

		Vertex_attrib_layout al1(1, 2, 3, 4);
		Assert::AreEqual(1, al1.position_location);
		Assert::AreEqual(2, al1.normal_location);
		Assert::AreEqual(3, al1.tex_coord_location);
		Assert::AreEqual(4, al1.tangent_h_location);
	}

	TEST_METHOD(attribs)
	{
		using cg::data::has_position;
		using cg::data::has_normal;
		using cg::data::has_tex_coord;
		using cg::data::has_tangent_h;

		Vertex_attrib_layout val0;
		auto attribs0 = val0.attribs();
		Assert::IsFalse(has_position(attribs0));
		Assert::IsFalse(has_normal(attribs0));
		Assert::IsFalse(has_tex_coord(attribs0));
		Assert::IsFalse(has_tangent_h(attribs0));

		Vertex_attrib_layout val_p(100, Invalid::vertex_attrib_location, Invalid::vertex_attrib_location, Invalid::vertex_attrib_location);
		auto attribs_p = val_p.attribs();
		Assert::IsTrue(has_position(attribs_p));
		Assert::IsFalse(has_normal(attribs_p));
		Assert::IsFalse(has_tex_coord(attribs_p));
		Assert::IsFalse(has_tangent_h(attribs_p));

		Vertex_attrib_layout val_n(Invalid::vertex_attrib_location, 100, Invalid::vertex_attrib_location, Invalid::vertex_attrib_location);
		auto attribs_n = val_n.attribs();
		Assert::IsFalse(has_position(attribs_n));
		Assert::IsTrue(has_normal(attribs_n));
		Assert::IsFalse(has_tex_coord(attribs_n));
		Assert::IsFalse(has_tangent_h(attribs_n));

		Vertex_attrib_layout val_tc(Invalid::vertex_attrib_location, Invalid::vertex_attrib_location, 100, Invalid::vertex_attrib_location);
		auto attribs_tc = val_tc.attribs();
		Assert::IsFalse(has_position(attribs_tc));
		Assert::IsFalse(has_normal(attribs_tc));
		Assert::IsTrue(has_tex_coord(attribs_tc));
		Assert::IsFalse(has_tangent_h(attribs_tc));

		Vertex_attrib_layout val_th(Invalid::vertex_attrib_location, Invalid::vertex_attrib_location, Invalid::vertex_attrib_location, 100);
		auto attribs_th = val_th.attribs();
		Assert::IsFalse(has_position(attribs_th));
		Assert::IsFalse(has_normal(attribs_th));
		Assert::IsFalse(has_tex_coord(attribs_th));
		Assert::IsTrue(has_tangent_h(attribs_th));

		Vertex_attrib_layout val_all(1, 2, 3, 4);
		auto attribs_all = val_all.attribs();
		Assert::IsTrue(has_position(attribs_all));
		Assert::IsTrue(has_normal(attribs_all));
		Assert::IsTrue(has_tex_coord(attribs_all));
		Assert::IsTrue(has_tangent_h(attribs_all));
	}
};

} // namespace unittest