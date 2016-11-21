#include "cg/data/shader.h"

#include <cassert>
#include <cstring>
#include "cg/base/base.h"
#include "cg/data/file.h"

using cg::data::exists;
using cg::data::load_text;


namespace cg {
namespace data {

Hlsl_shader_set_data load_hlsl_shader_set_data(const char* filename)
{
	ENFORCE(exists(filename), EXCEPTION_MSG("The specified hlsl file '", filename, "' does not exist."));
	Hlsl_shader_set_data data;

	data.source_code = load_text(filename);
	data.source_filename = filename;

	return data;
}

Glsl_program_data load_glsl_program_data(const char* filename)
{
	if (!filename) return Glsl_program_data{};

	std::string fn;
	Glsl_program_data glsl_data;

	// vertex shader
	fn.append(filename);
	fn.append(".vertex.glsl");;
	ENFORCE(exists(fn), EXCEPTION_MSG("The specified glsl file '", fn, "' does not exist."));
	glsl_data.vertex_shader_source_code = load_text(fn);

	// pixel shader
	fn.clear();
	fn.append(filename);
	fn.append(".pixel.glsl");
	ENFORCE(exists(fn), EXCEPTION_MSG("The specified glsl file '", fn, "' does not exist."));
	glsl_data.fragment_shader_source_code = load_text(fn);

	return glsl_data;
}

Glsl_program_data load_glsl_program_data(const char* vertex_shader_filename, const char* fragment_shader_filename)
{
	ENFORCE(exists(vertex_shader_filename),
		EXCEPTION_MSG("The specified glsl file '", vertex_shader_filename, "' does not exist."));

	ENFORCE(exists(fragment_shader_filename),
		EXCEPTION_MSG("The specified glsl file '", fragment_shader_filename, "' does not exist."));

	Glsl_program_data glsl_data;
	glsl_data.vertex_shader_source_code = load_text(vertex_shader_filename);
	glsl_data.fragment_shader_source_code = load_text(fragment_shader_filename);

	return glsl_data;
}

} // namespace data
} // namespace cg