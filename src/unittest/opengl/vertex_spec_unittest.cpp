#include "unittest/opengl/common_opengl.h"

using cg::opengl::DE_cmd;
using cg::opengl::DE_base_vertex_params;


namespace unittest {

TEST_CLASS(cg_opengl_vertex_spec_DE_base_vertex_params) {

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

} // namespace unittest