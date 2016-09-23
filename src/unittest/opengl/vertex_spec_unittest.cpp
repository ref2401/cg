#include "unittest/opengl/common_opengl.h"

using cg::opengl::DE_cmd;
using cg::opengl::DE_base_vertex_params;
using cg::opengl::Vertex_attrib_layout;


namespace unittest {

TEST_CLASS(cg_opengl_vertex_spec_DE_cmd) {
public:

	TEST_METHOD(ctors)
	{
		DE_cmd cmd0;
		Assert::AreEqual<GLuint>(cg::opengl::invalid_vao_id, cmd0.vao_id());
		Assert::AreEqual<size_t>(0, cmd0.index_count());
		Assert::AreEqual<size_t>(0, cmd0.offset_indices());
		Assert::AreEqual<size_t>(0, cmd0.base_vertex());
		Assert::AreEqual<size_t>(1, cmd0.instance_count());
		Assert::AreEqual<size_t>(0, cmd0.base_instance());

		DE_cmd cmd1(1, 2, 3, 4);
		Assert::AreEqual<GLuint>(1, cmd1.vao_id());
		Assert::AreEqual<size_t>(2, cmd1.index_count());
		Assert::AreEqual<size_t>(3, cmd1.offset_indices());
		Assert::AreEqual<size_t>(4, cmd1.base_vertex());
		Assert::AreEqual<size_t>(1, cmd1.instance_count());
		Assert::AreEqual<size_t>(0, cmd1.base_instance());

		DE_cmd cmd2(1, 2, 3, 4, 5, 6);
		Assert::AreEqual<GLuint>(1, cmd2.vao_id());
		Assert::AreEqual<size_t>(2, cmd2.index_count());
		Assert::AreEqual<size_t>(3, cmd2.offset_indices());
		Assert::AreEqual<size_t>(4, cmd2.base_vertex());
		Assert::AreEqual<size_t>(5, cmd2.instance_count());
		Assert::AreEqual<size_t>(6, cmd2.base_instance());
	}

	TEST_METHOD(get_base_vertex_params)
	{
		size_t index_count = 3;
		size_t offset_index = 100;
		size_t base_vertex = 11;
		DE_base_vertex_params expected_params(GL_TRIANGLES, index_count, GL_UNSIGNED_INT,
			reinterpret_cast<void*>(offset_index * sizeof(GL_UNSIGNED_INT)), base_vertex);

		DE_cmd cmd(24, index_count, offset_index, base_vertex);
		Assert::AreEqual(expected_params, cmd.get_base_vertex_params());
	}

	TEST_METHOD(equal_operator)
	{
		DE_cmd cmd(1, 2, 3, 4, 5, 6);

		Assert::AreNotEqual(cmd, DE_cmd(100, 2, 3, 4, 5, 6));
		Assert::AreNotEqual(cmd, DE_cmd(1, 200, 3, 4, 5, 6));
		Assert::AreNotEqual(cmd, DE_cmd(1, 2, 300, 4, 5, 6));
		Assert::AreNotEqual(cmd, DE_cmd(1, 2, 3, 400, 5, 6));
		Assert::AreNotEqual(cmd, DE_cmd(1, 2, 3, 4, 500, 6));
		Assert::AreNotEqual(cmd, DE_cmd(1, 2, 3, 4, 5, 600));

		Assert::AreEqual(cmd, DE_cmd(1, 2, 3, 4, 5, 6));
	}

};

TEST_CLASS(cg_opengl_vertex_spec_DE_base_vertex_params) {
public:

	TEST_METHOD(ctors)
	{
		GLenum mode = GL_TRIANGLES;
		GLsizei count = 3;
		GLenum type = GL_UNSIGNED_INT;
		void* offset = reinterpret_cast<void*>(4 * sizeof(GL_UNSIGNED_INT));
		GLint base_vertex = 24;

		DE_base_vertex_params p0;
		Assert::AreEqual(mode, p0.mode);
		Assert::AreEqual(0, p0.index_count);
		Assert::AreEqual(type, p0.index_type);
		Assert::AreEqual<void*>(nullptr, p0.offset_bytes);
		Assert::AreEqual(0, p0.base_vertex);

		DE_base_vertex_params p(mode, count, type, offset, base_vertex);
		Assert::AreEqual(mode, p.mode);
		Assert::AreEqual(count, p.index_count);
		Assert::AreEqual(type, p.index_type);
		Assert::AreEqual(offset, p.offset_bytes);
		Assert::AreEqual(base_vertex, p.base_vertex);
	}

	TEST_METHOD(equal_operator)
	{
		GLenum mode = GL_TRIANGLES;
		GLsizei count = 3;
		GLenum type = GL_UNSIGNED_INT;
		void* offset = reinterpret_cast<void*>(4 * sizeof(GL_UNSIGNED_INT));
		GLint base_vertex = 24;

		DE_base_vertex_params p(mode, count, type, offset, base_vertex);

		// mode & type tests are skipped
		Assert::AreNotEqual(p, DE_base_vertex_params(mode, 0, type, offset, base_vertex));
		Assert::AreNotEqual(p, DE_base_vertex_params(mode, count, type, nullptr, base_vertex));
		Assert::AreNotEqual(p, DE_base_vertex_params(mode, count, type, offset, 0));

		Assert::AreEqual(p, DE_base_vertex_params(mode, count, type, offset, base_vertex));
	}

};

TEST_CLASS(cg_opengl_vertex_spec_Vertex_attrib_layout) {
public:

	TEST_METHOD(ctors)
	{
		Vertex_attrib_layout al0;
		Assert::AreEqual(Vertex_attrib_layout::invalid_location, al0.position_location);
		Assert::AreEqual(Vertex_attrib_layout::invalid_location, al0.normal_location);
		Assert::AreEqual(Vertex_attrib_layout::invalid_location, al0.tex_coord_location);
		Assert::AreEqual(Vertex_attrib_layout::invalid_location, al0.tangent_h_location);

		Vertex_attrib_layout al1(1, 2, 3, 4);
		Assert::AreEqual(1, al1.position_location);
		Assert::AreEqual(2, al1.normal_location);
		Assert::AreEqual(3, al1.tex_coord_location);
		Assert::AreEqual(4, al1.tangent_h_location);
	}
};

} // namespace unittest