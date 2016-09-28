#include "cg/file/file.h"

#include <algorithm>
#include <array>
#include <iterator>
#include <type_traits>
#include <utility>
#include "cg/data/mesh.h"
#include "cg/data/shader.h"
#include "unittest/file/common_file.h"

using cg::file::By_line_iterator;
using cg::file::File;
using cg::file::File_seek_origin;


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

			unsigned char buffer_id[8];
			size_t count = f.read_bytes(buffer_id, std::extent<decltype(buffer_id)>::value);
			buffer_id[count] = '\0';
			Assert::AreEqual(6u, count);
			Assert::IsTrue(f.eof());
			Assert::AreEqual("abc123", reinterpret_cast<char*>(buffer_id));
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

	TEST_METHOD(seek)
	{
		{ // empty file
			char ch;
			File f(Filenames::ascii_single_line);

			f.seek(3, File_seek_origin::file_start);
			f.read_byte(&ch);
			Assert::AreEqual('1', ch);
			
			f.seek(1, File_seek_origin::current_position);
			f.read_byte(&ch);
			Assert::AreEqual('3', ch);
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

	TEST_METHOD(exists)
	{
		using cg::file::exists;

		Assert::IsTrue(exists(Filenames::empty_file));
		Assert::IsTrue(exists(Filenames::ascii_single_line));
		Assert::IsTrue(exists(Filenames::wavefront_triangle_p.c_str()));
		Assert::IsFalse(exists(""));
		Assert::IsFalse(exists("unknown_director/unknown_file.some_ext"));
	}

	TEST_METHOD(load_glsl_program_source)
	{
		using cg::data::Shader_program_source_code;
		using cg::file::load_glsl_program_source;
		using cg::file::load_text;

		auto src0 = load_glsl_program_source("unknown_file");
		Assert::IsTrue(src0.vertex_source.empty());
		Assert::IsTrue(src0.pixel_source.empty());

		auto expected_vertex_source = load_text(Filenames::not_real_vertex_glsl);
		auto expected_pixel_source = load_text(Filenames::not_real_pixel_glsl);
		auto src1 = load_glsl_program_source(Filenames::not_real_shader_program_name);
		Assert::AreEqual(expected_vertex_source, src1.vertex_source);
		Assert::AreEqual(expected_pixel_source, src1.pixel_source);
	}

	TEST_METHOD(load_image_tga)
	{
		using cg::uint2;
		using cg::data::Image_2d;
		using cg::data::Image_format;
		using cg::file::load_image_tga;

		{ // grayscale, red_8, no compression
			// image data:
			// black (r: 0x00), dark gray (r: 0x3f)
			// white (r: 0xff), light gray (r: 0x7f)
			// image's pixel sequence: white, light-gray, black, dark-gray
			std::array<unsigned char, 4> expected_data = {
				0xff,	// white
				0x7f,	// light gray
				0x00,	// black
				0x3f	// dark gray
			};

			Image_2d img = load_image_tga(Filenames::tga_grayscale_r_square_2x2);
			Assert::IsTrue(uint2(2) == img.size());
			Assert::IsTrue(Image_format::red_8 == img.format());
			Assert::IsTrue(std::equal(expected_data.cbegin(), expected_data.cend(), img.data()));
		}

		{ // true color, bgr_8, no compression
			// image data:
			// white (rgb/bgr: 0xffffff), red (rgb: 0xff0000 -> bgr: 0x0000ff)
			// green (rgb/bgr: 0x00ff00), blue(rgb: 0x0000ff -> bgr: 0xff0000)
			// image's pixel sequence: green, blue, white, red
			std::array<unsigned char, 4 * 3> expected_data = { 
				0x00, 0xff, 0x00,	// green
				0xff, 0x00, 0x00,	// blue
				0xff, 0xff, 0xff,	// white
				0x00, 0x00, 0xff	// red
			};

			Image_2d img = load_image_tga(Filenames::tga_true_color_rgb_square_2x2);
			Assert::IsTrue(uint2(2) == img.size());
			Assert::IsTrue(Image_format::bgr_8 == img.format());
			Assert::IsTrue(std::equal(expected_data.cbegin(), expected_data.cend(), img.data()));
		}

		{ // grayscale, red_8, rle compression
		  // image data:
		  // black (r: 0x00), black (r: 0x00), dark gray (r: 0x3f)
		  // white (r: 0xff), white (r: 0xff), light gray (r: 0x7f)
		  // image's pixel sequence: white, white, light-gray, black, black, dark-gray
			std::array<unsigned char, 6> expected_data = {
				0xff,	// white
				0xff,	// white
				0x7f,	// light gray
				0x00,	// black
				0x00,	// black
				0x3f	// dark gray
			};

			Image_2d img = load_image_tga(Filenames::tga_gayscale_r_compressed_rect_3x2);
			Assert::IsTrue(uint2(3, 2) == img.size());
			Assert::IsTrue(Image_format::red_8 == img.format());
			Assert::IsTrue(std::equal(expected_data.cbegin(), expected_data.cend(), img.data()));
		}

		{ // true color, bgr_8, rle compression
			// image data:
			// white (rgb/bgr: 0xffffff), white (rgb/bgr: 0xffffff), red (rgb: 0xff0000 -> bgr: 0x0000ff)
			// green (rgb/bgr: 0x00ff00), green (rgb/bgr: 0x00ff00), blue(rgb: 0x0000ff -> bgr: 0xff0000)
			// image's pixel sequence: green, green, blue, white, white, red
			std::array<unsigned char, 3 * 2 * 3> expected_data = {
				0x00, 0xff, 0x00,	// green
				0x00, 0xff, 0x00,	// green
				0xff, 0x00, 0x00,	// blue
				0xff, 0xff, 0xff,	// white
				0xff, 0xff, 0xff,	// white
				0x00, 0x00, 0xff	// red
			};

			Image_2d img = load_image_tga(Filenames::tga_true_color_rgb_compressed_rect_3x2);
			Assert::IsTrue(uint2(3, 2) == img.size());
			Assert::IsTrue(Image_format::bgr_8 == img.format());
			Assert::IsTrue(std::equal(expected_data.cbegin(), expected_data.cend(), img.data()));
		}
	}

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
