#include "cg/data/shader.h"

#include <cassert>
#include <cstring>
#include <algorithm>
#include <codecvt>
#include <locale>
#include "cg/base/base.h"
#include "cg/data/file.h"

using cg::data::exists;
using cg::data::load_text;


namespace cg {
namespace data {

Glsl_compute_desc load_glsl_compute_desc(const char* name, const char* filename)
{
	ENFORCE(exists(filename),
		EXCEPTION_MSG("The specified glsl file '", filename, "' does not exist."));

	Glsl_compute_desc compute_desc;
	compute_desc.name = name;
	compute_desc.compute_shader_source_code = load_text(filename);

	return compute_desc;
}

Glsl_program_desc load_glsl_program_desc(const char* name, const char* filename)
{
	if (!filename) return Glsl_program_desc{};

	std::string fn;
	Glsl_program_desc program_desc;
	program_desc.name = name;

	// vertex shader
	fn.append(filename);
	fn.append(".vertex.glsl");
	ENFORCE(exists(fn), EXCEPTION_MSG("The specified glsl file '", fn, "' does not exist."));
	program_desc.vertex_shader_source_code = load_text(fn);

	// pixel shader
	fn.clear();
	fn.append(filename);
	fn.append(".fragment.glsl");
	if (exists(fn))
		program_desc.fragment_shader_source_code = load_text(fn);

	return program_desc;
}

Glsl_program_desc load_glsl_program_desc(const char* name, const char* vertex_filename, const char* fragment_filename)
{
	ENFORCE(exists(vertex_filename),
		EXCEPTION_MSG("The specified glsl file '", vertex_filename, "' does not exist."));

	Glsl_program_desc program_desc;
	program_desc.name = name;
	program_desc.vertex_shader_source_code = load_text(vertex_filename);

	if (fragment_filename && (strlen(fragment_filename) > 0)) {
		ENFORCE(exists(fragment_filename),
			EXCEPTION_MSG("The specified glsl file '", fragment_filename, "' does not exist."));
		program_desc.fragment_shader_source_code = load_text(fragment_filename);
	}

	return program_desc;
}

} // namespace data
} // namespace cg