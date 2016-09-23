#include "unittest/opengl/common_opengl.h"

using cg::opengl::DE_cmd;
using cg::opengl::DE_base_vertex_params;


namespace unittest {

TEST_CLASS(cg_opengl_vertex_spec_DE_base_vertex_params) {
public:

	TEST_METHOD(ctors)
	{
		GLenum mode = GL_TRIANGLES;
		GLsizei count = 3;
		GLenum type = GL_UNSIGNED_INT;
		void* offset = reinterpret_cast<void*>(4 * sizeof(GL_UNSIGNED_INT));
		GLint base_vertex = 24;

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

TEST_CLASS(cg_opengl_vertex_spec_DE_cmd) {
public:

	TEST_METHOD(ctors)
	{
		DE_cmd cmd0(1, 2, 3, 4);
		Assert::AreEqual<GLuint>(1, cmd0.vao_id());
		Assert::AreEqual<size_t>(2, cmd0.index_count());
		Assert::AreEqual<size_t>(3, cmd0.offset_indices());
		Assert::AreEqual<size_t>(4, cmd0.base_vertex());
		Assert::AreEqual<size_t>(1, cmd0.instance_count());
		Assert::AreEqual<size_t>(0, cmd0.base_instance());

		DE_cmd cmd1(1, 2, 3, 4, 5, 6);
		Assert::AreEqual<GLuint>(1, cmd1.vao_id());
		Assert::AreEqual<size_t>(2, cmd1.index_count());
		Assert::AreEqual<size_t>(3, cmd1.offset_indices());
		Assert::AreEqual<size_t>(4, cmd1.base_vertex());
		Assert::AreEqual<size_t>(5, cmd1.instance_count());
		Assert::AreEqual<size_t>(6, cmd1.base_instance());
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

} // namespace unittest