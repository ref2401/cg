#include "cg/file/file.h"



namespace cg {
namespace file {

cg::data::Shader_program_source_code load_glsl_program_source(const char* filename)
{
	using cg::data::Shader_program_source_code;

	if (!filename) return Shader_program_source_code{};

	std::string fn;
	Shader_program_source_code src;

	// vertex shader
	fn.append(filename);
	fn.append(".vertex.glsl");
	if (exists(fn))
		src.vertex_source = load_text(fn);

	// pixel shader
	fn.clear();
	fn.append(filename);
	fn.append(".pixel.glsl");
	if (exists(fn))
		src.pixel_source = load_text(fn);

	return src;
}

cg::data::Shader_program_source_code load_glsl_program_source(const char* vertex_filename, const char* pixel_filename)
{
	cg::data::Shader_program_source_code src;

	if (exists(vertex_filename))
		src.vertex_source = load_text(vertex_filename);

	if (exists(pixel_filename))
		src.pixel_source = load_text(pixel_filename);

	return src;
}


} // namespace file
} // namespace cg
