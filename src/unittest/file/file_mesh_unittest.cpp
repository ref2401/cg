#include "cg/file/file_mesh.h"

#include <algorithm>
#include <iterator>
#include "cg/data/mesh.h"
#include "unittest/file/common_file.h"


namespace unittest {

TEST_CLASS(cg_file_mesh_Funcs) {
public:

	TEST_METHOD(load_mesh_wavefront_old_triangle_negative_vertex_indices)
	{
		using cg::approx_equal;
		using cg::data::Interleaved_mesh_data_old;
		using cg::data::Vertex_attribs;
		using cg::file::load_mesh_wavefront_old;

		uint32_t expected_indices[6] = { 0, 1, 2, 3, 4, 5 };
		{ // positions only
			float expected_data[18] = {
				-2, -1, 0,	 2, -1, 0,	2, 1, 0,
				-2,	 1, 0,	-2, -1, 0,	2, 1, 0
			};
			auto md = load_mesh_wavefront_old(Filenames::wavefront_rect_negative_indices_p, Vertex_attribs::position);

			Assert::IsTrue(std::equal(
				md.data().cbegin(), md.data().cend(),
				std::cbegin(expected_data),
				[](float a, float b) { return approx_equal<float>(a, b); }));

			Assert::IsTrue(std::equal(
				md.indices().cbegin(), md.indices().cend(), std::cbegin(expected_indices)));
		}

		{ // positions & normals
			float expected_data[36] = {
				-2, -1, 0, 0, 0, 1,	 2, -1, 0, 0, 0, 1,  2, 1, 0, 0, 0, 1,
				-2,	 1, 0, 0, 0, 1,	-2, -1, 0, 0, 0, 1,  2, 1, 0, 0, 0, 1
			};

			auto md = load_mesh_wavefront_old(Filenames::wavefront_rect_negative_indices_pn,
				Vertex_attribs::position | Vertex_attribs::normal);

			Assert::IsTrue(std::equal(
				md.data().cbegin(), md.data().cend(),
				std::cbegin(expected_data),
				[](float a, float b) { return approx_equal<float>(a, b); }));

			Assert::IsTrue(std::equal(
				md.indices().cbegin(), md.indices().cend(), std::cbegin(expected_indices)));
		}

		{ // positions & tex_coords
			float expected_data[30] = {
				-2, -1, 0, 1, 0,   2, -1, 0, 1, 1,  2, 1, 0, 0, 1,
				-2,	 1, 0, 0, 0,  -2, -1, 0, 1, 0,  2, 1, 0, 0, 1
			};

			auto md = load_mesh_wavefront_old(Filenames::wavefront_rect_negative_indices_ptc, Vertex_attribs::vertex_p_tc);

			Assert::IsTrue(std::equal(
				md.data().cbegin(), md.data().cend(),
				std::cbegin(expected_data),
				[](float a, float b) { return approx_equal<float>(a, b); }));

			Assert::IsTrue(std::equal(
				md.indices().cbegin(), md.indices().cend(), std::cbegin(expected_indices)));
		}

		{ // positions, normal & tex_coords
			float expected_data[48] = {
				-2, -1, 0, 0, 0, 1, 1, 0,   2, -1, 0, 0, 0, 1, 1, 1,  2, 1, 0, 0, 0, 1, 0, 1,
				-2,	 1, 0, 0, 0, 1, 0, 0,  -2, -1, 0, 0, 0, 1, 1, 0,  2, 1, 0, 0, 0, 1, 0, 1
			};

			auto md = load_mesh_wavefront_old(Filenames::wavefront_rect_negative_indices_pntc,
				Vertex_attribs::vertex_p_tc | Vertex_attribs::normal);

			Assert::IsTrue(std::equal(
				md.data().cbegin(), md.data().cend(),
				std::cbegin(expected_data),
				[](float a, float b) { return approx_equal<float>(a, b); }));

			Assert::IsTrue(std::equal(
				md.indices().cbegin(), md.indices().cend(), std::cbegin(expected_indices)));
		}

		{ // positions, normal, tex_coords, tangent_h
			float expected_data[72] = {
				-2, -1, 0, 0, 0, 1, 1, 0, 0, -1, 0, 1,   2, -1, 0, 0, 0, 1, 1, 1, 0, -1, 0, 1,  2, 1, 0, 0, 0, 1, 0, 1, 0, -1, 0, 1,
				-2,	 1, 0, 0, 0, 1, 0, 0, 0, -1, 0, 1,  -2, -1, 0, 0, 0, 1, 1, 0, 0, -1, 0, 1,  2, 1, 0, 0, 0, 1, 0, 1, 0, -1, 0, 1
			};

			auto md = load_mesh_wavefront_old(Filenames::wavefront_rect_negative_indices_pntc, Vertex_attribs::vertex_ts);

			Assert::IsTrue(std::equal(
				md.data().cbegin(), md.data().cend(),
				std::cbegin(expected_data),
				[](float a, float b) { return approx_equal<float>(a, b); }));

			Assert::IsTrue(std::equal(
				md.indices().cbegin(), md.indices().cend(), std::cbegin(expected_indices)));
		}
	}

	TEST_METHOD(load_mesh_wavefront_old_triangle_positive_vertex_indices)
	{
		using cg::approx_equal;
		using cg::data::Interleaved_mesh_data_old;
		using cg::data::Vertex_attribs;
		using cg::file::load_mesh_wavefront_old;

		uint32_t expected_indices[6] = { 0, 1, 2, 3, 4, 5 };
		{ // positions only
			float expected_data[18] = {
				-2, -1, 0,	 2, -1, 0,	2, 1, 0,
				-2,	 1, 0,	-2, -1, 0,	2, 1, 0
			};
			auto md = load_mesh_wavefront_old(Filenames::wavefront_rect_positive_indices_p, Vertex_attribs::position);

			Assert::IsTrue(std::equal(
				md.data().cbegin(), md.data().cend(),
				std::cbegin(expected_data),
				[](float a, float b) { return approx_equal<float>(a, b); }));

			Assert::IsTrue(std::equal(
				md.indices().cbegin(), md.indices().cend(), std::cbegin(expected_indices)));
		}

		{ // positions & normals
			float expected_data[36] = {
				-2, -1, 0, 0, 0, 1,	 2, -1, 0, 0, 0, 1,  2, 1, 0, 0, 0, 1,
				-2,	 1, 0, 0, 0, 1,	-2, -1, 0, 0, 0, 1,  2, 1, 0, 0, 0, 1
			};

			auto md = load_mesh_wavefront_old(Filenames::wavefront_rect_positive_indices_pn,
				Vertex_attribs::position | Vertex_attribs::normal);

			Assert::IsTrue(std::equal(
				md.data().cbegin(), md.data().cend(),
				std::cbegin(expected_data),
				[](float a, float b) { return approx_equal<float>(a, b); }));

			Assert::IsTrue(std::equal(
				md.indices().cbegin(), md.indices().cend(), std::cbegin(expected_indices)));
		}

		{ // positions & tex_coords
			float expected_data[30] = {
				-2, -1, 0, 1, 0,   2, -1, 0, 1, 1,  2, 1, 0, 0, 1,
				-2,	 1, 0, 0, 0,  -2, -1, 0, 1, 0,  2, 1, 0, 0, 1
			};

			auto md = load_mesh_wavefront_old(Filenames::wavefront_rect_positive_indices_ptc, Vertex_attribs::vertex_p_tc);

			Assert::IsTrue(std::equal(
				md.data().cbegin(), md.data().cend(),
				std::cbegin(expected_data),
				[](float a, float b) { return approx_equal<float>(a, b); }));

			Assert::IsTrue(std::equal(
				md.indices().cbegin(), md.indices().cend(), std::cbegin(expected_indices)));
		}

		{ // positions, normal & tex_coords
			float expected_data[48] = {
				-2, -1, 0, 0, 0, 1, 1, 0,   2, -1, 0, 0, 0, 1, 1, 1,  2, 1, 0, 0, 0, 1, 0, 1,
				-2,	 1, 0, 0, 0, 1, 0, 0,  -2, -1, 0, 0, 0, 1, 1, 0,  2, 1, 0, 0, 0, 1, 0, 1
			};

			auto md = load_mesh_wavefront_old(Filenames::wavefront_rect_positive_indices_pntc,
				Vertex_attribs::vertex_p_tc | Vertex_attribs::normal);

			Assert::IsTrue(std::equal(
				md.data().cbegin(), md.data().cend(),
				std::cbegin(expected_data),
				[](float a, float b) { return approx_equal<float>(a, b); }));

			Assert::IsTrue(std::equal(
				md.indices().cbegin(), md.indices().cend(), std::cbegin(expected_indices)));
		}

		{ // positions, normal, tex_coords, tangent_h
			float expected_data[72] = {
				-2, -1, 0, 0, 0, 1, 1, 0, 0, -1, 0, 1,   2, -1, 0, 0, 0, 1, 1, 1, 0, -1, 0, 1,  2, 1, 0, 0, 0, 1, 0, 1, 0, -1, 0, 1,
				-2,	 1, 0, 0, 0, 1, 0, 0, 0, -1, 0, 1,  -2, -1, 0, 0, 0, 1, 1, 0, 0, -1, 0, 1,  2, 1, 0, 0, 0, 1, 0, 1, 0, -1, 0, 1
			};

			auto md = load_mesh_wavefront_old(Filenames::wavefront_rect_positive_indices_pntc, Vertex_attribs::vertex_ts);

			Assert::IsTrue(std::equal(
				md.data().cbegin(), md.data().cend(),
				std::cbegin(expected_data),
				[](float a, float b) { return approx_equal<float>(a, b); }));

			Assert::IsTrue(std::equal(
				md.indices().cbegin(), md.indices().cend(), std::cbegin(expected_indices)));
		}
	}

};

} // namespace unittest