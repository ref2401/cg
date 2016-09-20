#include "cg/file/file.h"

#include <algorithm>
#include <iterator>
#include <type_traits>
#include <utility>
#include "unittest/file/common_file.h"

using cg::file::By_line_iterator;
using cg::file::File;


namespace unittest {

TEST_CLASS(cg_file_File) {

	TEST_METHOD(ctor_open_file)
	{
		File fe;
		Assert::IsTrue(fe.filename().empty());
		Assert::IsFalse(fe.is_open());

		Assert::ExpectException<std::runtime_error>([] { File f("unknown-file"); });

		File f(Filenames::empty_file);
		Assert::AreEqual(Filenames::empty_file, f.filename());
		Assert::IsTrue(f.is_open());

		File fm = std::move(f);
		Assert::AreEqual(Filenames::empty_file, fm.filename());
		Assert::IsTrue(fm.is_open());
		// f is empty
		Assert::IsTrue(f.filename().empty());
		Assert::IsFalse(f.is_open());
	}

	TEST_METHOD(move_assignment)
	{
		File f0(Filenames::empty_file);
		File f1(Filenames::ascii_single_line);

		f1 = std::move(f0);
		Assert::AreEqual(Filenames::empty_file, f1.filename());
		Assert::IsTrue(f1.is_open());
		// f is empty
		Assert::IsTrue(f0.filename().empty());
		Assert::IsFalse(f0.is_open());
	}

	TEST_METHOD(open_close)
	{
		File f;
		f.close(); // does not throw

		Assert::ExpectException<std::runtime_error>([&] { f.open("unknown-file"); });
		Assert::IsTrue(f.filename().empty());
		Assert::IsFalse(f.is_open());

		f.open(Filenames::empty_file);
		Assert::IsFalse(f.eof());
		Assert::AreEqual(Filenames::empty_file, f.filename());
		Assert::IsTrue(f.is_open());

		f.close();
		Assert::IsTrue(f.filename().empty());
		Assert::IsFalse(f.is_open());
	}

	TEST_METHOD(read_byte_and_eof)
	{
		{ // empty file
			File f(Filenames::empty_file);
			Assert::IsFalse(f.eof());

			char ch;
			bool res = f.read_byte(&ch);
			Assert::IsFalse(res);
			Assert::IsTrue(f.eof());
		}

		{ // file content: abc123
			File f(Filenames::ascii_single_line);
			Assert::IsFalse(f.eof());

			char ch;
			const char* expected_chars = "abc123";
			for (size_t i = 0; i < 6; ++i) {
				bool res = f.read_byte(&ch);
				Assert::IsTrue(res);
				Assert::AreEqual(expected_chars[i], ch);
			}

			bool res = f.read_byte(&ch);
			Assert::IsFalse(res);
			Assert::IsTrue(f.eof());
		}

		{ // attempt to read from moved File object
			File f0(Filenames::ascii_single_line);
			Assert::IsFalse(f0.eof());

			File f = std::move(f0);
			Assert::ExpectException<std::runtime_error>([&]
			{
				char ch;
				f0.read_byte(&ch);
			});
		}
	}

	TEST_METHOD(read_bytes_and_eof)
	{
		{ // empty file
			File f(Filenames::empty_file);
			Assert::IsFalse(f.eof());

			char ch;
			size_t count = f.read_bytes(&ch, 1);
			Assert::AreEqual(0u, count);
			Assert::IsTrue(f.eof());
		}

		
		{ // file content: abc123
			File f(Filenames::ascii_single_line);
			Assert::IsFalse(f.eof());

			unsigned char buffer[8];
			size_t count = f.read_bytes(buffer, std::extent<decltype(buffer)>::value);
			buffer[count] = '\0';
			Assert::AreEqual(6u, count);
			Assert::IsTrue(f.eof());
			Assert::AreEqual("abc123", reinterpret_cast<char*>(buffer));
		}

		{ // attempt to read from moved File object
			File f0(Filenames::ascii_single_line);
			Assert::IsFalse(f0.eof());

			File f = std::move(f0);
			Assert::ExpectException<std::runtime_error>([&]
			{ 
				char ch;
				f0.read_bytes(&ch, 1);  
			});
		}
	}

};

TEST_CLASS(cg_file_By_line_iterator) {
public:

	TEST_METHOD(ctors_and_end_iterator)
	{
		// no file iterator (the end iterator)
		By_line_iterator it0;
		Assert::IsTrue(it0.filename().empty());
		Assert::IsFalse(it0.is_open());
		Assert::IsTrue(it0 == By_line_iterator::end);

		// empty file iterator
		By_line_iterator it1(Filenames::empty_file);
		Assert::AreEqual(Filenames::empty_file, it1.filename());
		Assert::IsTrue(it1.is_open());
		Assert::IsTrue((*it1).empty()); // empty file has no content
		Assert::IsTrue(it1->empty());  // empty file has no content
		Assert::IsTrue(it1 != By_line_iterator::end);

		{ // single line file iterator
			std::string expected_first_line = "abc123";
			By_line_iterator it(Filenames::ascii_single_line);
			Assert::AreEqual(Filenames::ascii_single_line, it.filename());
			Assert::IsTrue(it.is_open());
			Assert::AreEqual(expected_first_line, *it);
			Assert::AreEqual(expected_first_line.size(), it->size());
			Assert::IsTrue(it != By_line_iterator::end);
		}

		{ // multiline file iterator
			std::string expected_first_line = "123";
			By_line_iterator it(Filenames::ascii_multiline);
			Assert::AreEqual(Filenames::ascii_multiline, it.filename());
			Assert::IsTrue(it.is_open());
			Assert::AreEqual(expected_first_line, *it);
			Assert::AreEqual(expected_first_line.size(), it->size());
			Assert::IsTrue(it != By_line_iterator::end);
		}
	}

	TEST_METHOD(iterate_no_line_feeds)
	{
		By_line_iterator it0;
		Assert::ExpectException<std::runtime_error>([&] { ++it0; });

		// empty file iterator
		By_line_iterator it_empty(Filenames::empty_file, false);
		Assert::IsTrue((*it_empty).empty()); // empty file has no content
		++it_empty;
		Assert::IsTrue(it_empty == By_line_iterator::end);

		// single line file iterator
		std::string expected_line = "abc123";
		By_line_iterator it_single(Filenames::ascii_single_line, false);
		Assert::AreEqual(expected_line, *it_single);
		++it_single;
		Assert::IsTrue(it_single == By_line_iterator::end);

		// multiline file iterator
		std::string expected_lines[] = { "123", "abc", "", "last_line" };
		By_line_iterator it_multi(Filenames::ascii_multiline, false);
		for (size_t i = 0; it_multi != By_line_iterator::end; ++it_multi, ++i) {
			Assert::AreEqual(expected_lines[i], *it_multi);
		}
		Assert::IsTrue(it_multi == By_line_iterator::end);
	}

	TEST_METHOD(iterate_with_line_feeds)
	{
		By_line_iterator it0;
		Assert::ExpectException<std::runtime_error>([&] { ++it0; });

		{ // empty file iterator
			std::string expected_line = "\n";
			By_line_iterator it_empty(Filenames::empty_file, true);
			Assert::AreEqual(expected_line, *it_empty);
			++it_empty;
			Assert::IsTrue(it_empty == By_line_iterator::end);
		}

		{ // single line file iterator
			std::string expected_line = "abc123\n";
			By_line_iterator it_single(Filenames::ascii_single_line, true);
			Assert::AreEqual(expected_line, *it_single);
			++it_single;
			Assert::IsTrue(it_single == By_line_iterator::end);
		}

		// multiline file iterator
		std::string expected_lines[] = { "123\n", "abc\n", "\n", "last_line\n" };
		By_line_iterator it_multi(Filenames::ascii_multiline, true);
		for (size_t i = 0; it_multi != By_line_iterator::end; ++it_multi, ++i) {
			Assert::AreEqual(expected_lines[i], *it_multi);
		}
		Assert::IsTrue(it_multi == By_line_iterator::end);
	}

	TEST_METHOD(post_increment)
	{
		std::string expected_lines[] = { "123", "abc", "", "last_line" };
		By_line_iterator it(Filenames::ascii_multiline, false);
		for (size_t i = 0; it != By_line_iterator::end; ++i) {
			Assert::AreEqual(expected_lines[i], *it++);
		}
	}
};

TEST_CLASS(cg_file_Funcs) {
public:

	TEST_METHOD(load_mesh_triangle)
	{
		using cg::approx_equal;
		using cg::data::Interleaved_mesh_data;
		using cg::data::Vertex_attribs;
		using cg::file::load_mesh_wavefront;

		uint32_t expected_indices[3] = { 0, 1, 2 };
		{ // positions only
			float expected_data[9] = { -1, 0, 1, 1, 0, 1, 1, 0, -1 };
			auto md = load_mesh_wavefront(Filenames::wavefront_triangle_p, Vertex_attribs::position);

			Assert::IsTrue(std::equal(
				md.data().cbegin(), md.data().cend(),
				std::cbegin(expected_data),
				[](float a, float b) { return approx_equal<float>(a, b); }));
			
			Assert::IsTrue(std::equal(
				md.indices().cbegin(), md.indices().cend(), std::cbegin(expected_indices)));
		}

		{ // positions & normals
			float expected_data[18] = { 
				-1, 0, 1, 0, 1, 0,
				1, 0, 1, 0, 1, 0,
				1, 0, -1, 0, 1, 0
			};

			auto md = load_mesh_wavefront(Filenames::wavefront_triangle_pn, 
				Vertex_attribs::position | Vertex_attribs::normal);

			Assert::IsTrue(std::equal(
				md.data().cbegin(), md.data().cend(),
				std::cbegin(expected_data),
				[](float a, float b) { return approx_equal<float>(a, b); }));

			Assert::IsTrue(std::equal(
				md.indices().cbegin(), md.indices().cend(), std::cbegin(expected_indices)));
		}

		{ // positions & tex_coords
			float expected_data[15] = {
				-1, 0, 1, 0, 1,
				1, 0, 1, 0, 0,
				1, 0, -1, 1, 0
			};

			auto md = load_mesh_wavefront(Filenames::wavefront_triangle_ptc, Vertex_attribs::mesh_textured);

			Assert::IsTrue(std::equal(
				md.data().cbegin(), md.data().cend(),
				std::cbegin(expected_data),
				[](float a, float b) { return approx_equal<float>(a, b); }));

			Assert::IsTrue(std::equal(
				md.indices().cbegin(), md.indices().cend(), std::cbegin(expected_indices)));
		}

		{ // positions, normal & tex_coords
			float expected_data[24] = {
				-1, 0, 1,  0, 1, 0,  0, 1,
				1, 0, 1,  0, 1, 0,  0, 0,
				1, 0, -1,  0, 1, 0,  1, 0
			};

			auto md = load_mesh_wavefront(Filenames::wavefront_triangle_pntc, 
				Vertex_attribs::mesh_textured | Vertex_attribs::normal);

			Assert::IsTrue(std::equal(
				md.data().cbegin(), md.data().cend(),
				std::cbegin(expected_data),
				[](float a, float b) { return approx_equal<float>(a, b); }));

			Assert::IsTrue(std::equal(
				md.indices().cbegin(), md.indices().cend(), std::cbegin(expected_indices)));
		}

		{ // positions, normal, tex_coords, tangent_h
			float expected_data[36] = {
				-1, 0, 1,  0, 1, 0,  0, 1,  0, 0, -1, 1,
				1, 0, 1,  0, 1, 0,  0, 0,  0, 0, -1, 1,
				1, 0, -1,  0, 1, 0,  1, 0,  0, 0, -1, 1
			};

			auto md = load_mesh_wavefront(Filenames::wavefront_triangle_pntc, Vertex_attribs::mesh_tangent_h);

			Assert::IsTrue(std::equal(
				md.data().cbegin(), md.data().cend(),
				std::cbegin(expected_data),
				[](float a, float b) { return approx_equal<float>(a, b); }));

			Assert::IsTrue(std::equal(
				md.indices().cbegin(), md.indices().cend(), std::cbegin(expected_indices)));
		}
	}

	TEST_METHOD(load_text)
	{
		using cg::file::load_text;

		{ // empty file
			std::string text = load_text(Filenames::empty_file);
			Assert::IsTrue(text.empty());
		}

		{ // single line file
			std::string text = load_text(Filenames::ascii_single_line);
			Assert::AreEqual("abc123", text.c_str());
		}

		{ // multiline file
			std::string text = load_text(Filenames::ascii_multiline);
			Assert::AreEqual("123\nabc\n\nlast_line", text.c_str());
		}
	}
};

} // namespace unittest
