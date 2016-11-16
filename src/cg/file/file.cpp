#include "cg/file/file.h"

#include <cassert>
#include <cstring>

using cg::data::Shader_program_source_code;
using cg::file::exists;
using cg::file::load_text;


namespace {

Shader_program_source_code load_program_source_impl(const char* filename, const char* file_extension)
{
	assert((std::strcmp(file_extension, "hlsl") == 0)
		|| (std::strcmp(file_extension, "glsl") == 0));

	if (!filename) return Shader_program_source_code{};

	std::string fn;
	Shader_program_source_code src;

	// vertex shader
	fn.append(filename);
	fn.append(".vertex.");
	fn.append(file_extension);
	if (exists(fn))
		src.vertex_source = load_text(fn);

	// pixel shader
	fn.clear();
	fn.append(filename);
	fn.append(".pixel.");
	fn.append(file_extension);
	if (exists(fn))
		src.pixel_source = load_text(fn);

	return src;
}

} // namespace


namespace cg {
namespace file {

Shader_program_source_code load_glsl_program_source(const char* filename)
{
	return load_program_source_impl(filename, "glsl");
}

Shader_program_source_code load_hlsl_program_source(const char* filename)
{
	return load_program_source_impl(filename, "hlsl");
}

Shader_program_source_code load_shader_program_source(const char* vertex_filename, const char* pixel_filename)
{
	Shader_program_source_code src;

	if (exists(vertex_filename))
		src.vertex_source = load_text(vertex_filename);

	if (exists(pixel_filename))
		src.pixel_source = load_text(pixel_filename);

	return src;
}


} // namespace file
} // namespace cg
