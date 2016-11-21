#include "cg/data/image.h"

#include <cstdint>
#include <type_traits>
#include "cg/base/base.h"
#include "cg/data/file.h"

namespace {

using cg::uint2;
using cg::greater_than;
using cg::data::Image_2d;
using cg::data::Image_format;
using cg::data::byte_count;
using cg::data::File;
using cg::data::File_seek_origin;


enum class Tga_image_type {
	invalid,
	no_data,
	color_map,
	true_color,
	grayscale,
	color_map_compressed,
	true_color_compressed,
	grayscale_compressed
};

enum class Tga_origin_corner {
	invalid,
	bottom_left,
	bottom_right,
	top_left,
	top_right
};

// Describes an tga image data.
struct Tga_header final {
	uint2 size;
	Image_format format;
	bool compressed;
};

// ----- funcs -----

inline uint16_t bytes_to_uint16(unsigned char high, unsigned char low) noexcept
{
	return uint16_t(high) << 8 | low;
}

inline Tga_image_type parse_image_type(unsigned char type) noexcept
{
	switch (type) {
		case 0: return Tga_image_type::no_data;
		case 1: return Tga_image_type::color_map;
		case 2: return Tga_image_type::true_color;
		case 3: return Tga_image_type::grayscale;
		case 9: return Tga_image_type::color_map_compressed;
		case 10: return Tga_image_type::true_color_compressed;
		case 11: return Tga_image_type::grayscale_compressed;

		default: return Tga_image_type::invalid;
	}
}

// Parses Tga_origin_corner enumeration value from bits 5 & 4 which are used to indicate the order in 
// which pixel data is transferred from the file to the screen.
// 0 - bottom left, 1 - bottom right, 2 - top left, 3 - top right.
inline Tga_origin_corner parse_origin_corner(unsigned char descriptor) noexcept
{
	unsigned char corner = (descriptor >> 4) & 0b11;

	switch (corner) {
		case 0: return Tga_origin_corner::bottom_left;
		case 1: return Tga_origin_corner::bottom_right;
		case 2: return Tga_origin_corner::top_left;
		case 3: return Tga_origin_corner::top_right;
		default: return Tga_origin_corner::invalid;
	}
}

// Reads first 18 bytes from file which are used to represent tga image header.
Tga_header read_header(File& file)
{
	Tga_header header;
	unsigned char header_buffer[18];
	file.read_bytes(header_buffer, std::extent<decltype(header_buffer)>::value);

	// ID field
	if (header_buffer[0] > 0)
		file.seek(header_buffer[0], File_seek_origin::current_position); // skip id field if any

																		 // Color map type
																		 // 0 if image file contains no color map, 1 if present.
	ENFORCE(header_buffer[1] == 0, "Reading tga image error. Tga image mustn't have a color map.", file.filename());

	// Image type
	auto type = parse_image_type(header_buffer[2]);
	ENFORCE(type != Tga_image_type::invalid, "Reading tga image error. Unknown tga image type.", file.filename());
	ENFORCE(type != Tga_image_type::no_data
		&& type != Tga_image_type::color_map
		&& type != Tga_image_type::color_map_compressed,
		"Reading tga image error. Forbidden tga image type.", file.filename());

	header.compressed = (type == Tga_image_type::true_color_compressed
		|| type == Tga_image_type::grayscale_compressed);

	// Image specification (x/y origin)
	uint2 origin(bytes_to_uint16(header_buffer[9], header_buffer[8]),
		bytes_to_uint16(header_buffer[11], header_buffer[10]));
	ENFORCE(origin == uint2::zero, "Reading tga image error. Unexpected origin coordinates. ", file.filename());

	// Image specification (width/height) 
	header.size.width = bytes_to_uint16(header_buffer[13], header_buffer[12]);
	header.size.height = bytes_to_uint16(header_buffer[15], header_buffer[14]);
	ENFORCE(greater_than(header.size, 0), "Reading tga image error. Image size must be greater than 0.", file.filename());

	// Image specification (pixel depth)
	unsigned char bits_per_pixel = header_buffer[16];

	// Image specification (descriptor)
	auto origin_corner = parse_origin_corner(header_buffer[17]);
	ENFORCE(origin_corner == Tga_origin_corner::bottom_left,
		"Reading tga image error. Unexpected origin corner. ", file.filename());

	unsigned char alpha_bits = header_buffer[17] & 0xF;
	ENFORCE(alpha_bits == 0, "Reading tga image error. Alpha depth must equal to 0. ", file.filename());

	// determine the image format
	switch (type) {
		case Tga_image_type::grayscale:
		case Tga_image_type::grayscale_compressed:
			ENFORCE(bits_per_pixel == 8,
				"Reading tga image error. Only 8 bits pixel depth is supported for grayscale images", file.filename());
			header.format = Image_format::red_8;
			break;

		case Tga_image_type::true_color:
		case Tga_image_type::true_color_compressed:
			ENFORCE(bits_per_pixel == 24 || bits_per_pixel == 32,
				"Reading tga image error. Only 24 or 32 bits pixel depth is supported for true color images", file.filename());
			header.format = (bits_per_pixel == 24) ? Image_format::bgr_8 : Image_format::bgra_8;
			break;
	}

	return header;
}

void read_image_data(File& file, Image_2d& image, bool compressed) noexcept
{
	if (!compressed) {
		file.read_bytes(image.data(), image.byte_count());
	}
	else {
		size_t curr_byte = 0;
		size_t pixel_byte_count = byte_count(image.format());
		unsigned char pixel_buffer[4 * sizeof(unsigned char)]; // 4 stands for bgra - the maximum possible component count of a pixel.

		while (curr_byte < image.byte_count()) {
			// run_info (ri_)
			unsigned char run_info;
			file.read_byte(&run_info);

			bool ri_rle_compression = (run_info & 0b1000'0000) != 0;
			size_t ri_pixel_count = (run_info & 0b0111'1111) + 1;

			if (ri_rle_compression) {
				file.read_bytes(pixel_buffer, pixel_byte_count);

				// repeat 'pixel_buffer' value 'ri_pixel_count' times
				for (size_t i = 0; i < ri_pixel_count; ++i) {
					curr_byte = image.write(curr_byte, pixel_buffer, pixel_byte_count);
				}
			}
			else {
				size_t byte_count_to_read = ri_pixel_count * pixel_byte_count;
				file.read_bytes(image.data() + curr_byte, byte_count_to_read);
				curr_byte += byte_count_to_read;
			}
		} // while
	} // else
}

Image_2d load_image_tga(File file)
{
	auto header = read_header(file);
	Image_2d image(header.size, header.format);
	read_image_data(file, image, header.compressed);
	// tga footer is omitted
	return image;
}

} // namespace


namespace cg {
namespace data {

cg::data::Image_2d load_image_tga(const std::string& filename)
{
	return ::load_image_tga(File(filename));
}

cg::data::Image_2d load_image_tga(const char* filename)
{
	return ::load_image_tga(File(filename));
}

} // namespace data
} // namespace cg