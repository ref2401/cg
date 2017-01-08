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

// ----- Transform_feedback -----

Transform_feedback_desc::Transform_feedback_desc(Transform_feedback_desc&& tf) noexcept
	: varying_names(std::move(tf.varying_names)),
	interleaved_buffer_mode(tf.interleaved_buffer_mode)
{
	tf.interleaved_buffer_mode = false;
}

Transform_feedback_desc& Transform_feedback_desc::operator=(Transform_feedback_desc&& tf) noexcept
{
	if (this == &tf) return *this;

	varying_names = std::move(tf.varying_names);
	interleaved_buffer_mode = tf.interleaved_buffer_mode;

	tf.interleaved_buffer_mode = false;

	return *this;
}

// ----- funcs -----

bool operator==(const Transform_feedback_desc& l, const Transform_feedback_desc& r) noexcept
{
	return (l.interleaved_buffer_mode == r.interleaved_buffer_mode)
		&& (l.varying_names.size() == r.varying_names.size())
		&& std::equal(l.varying_names.cbegin(), l.varying_names.cend(), r.varying_names.cbegin());
}

std::ostream& operator<<(std::ostream& out, const Transform_feedback_desc& tf)
{
	out << "Transform_feedback(" << tf.interleaved_buffer_mode;
	
	for (const std::string& var_name : tf.varying_names) {
		out << ", " << var_name;
	}

	out << ')';
	return out;
}

std::wostream& operator<<(std::wostream& out, const Transform_feedback_desc& tf)
{
	std::wstring_convert<std::codecvt<wchar_t, char, std::mbstate_t>> converter;
	
	out << "Transform_feedback(" << tf.interleaved_buffer_mode;
	
	for (const std::string& var_name : tf.varying_names) {
		out << ", " << converter.from_bytes(var_name);
	}

	out << ')';
	return out;
}

Glsl_compute_desc load_glsl_compute_desc(const char* filename)
{
	ENFORCE(exists(filename),
		EXCEPTION_MSG("The specified glsl file '", filename, "' does not exist."));

	Glsl_compute_desc compute_desc;
	compute_desc.compute_shader_source_code = load_text(filename);

	return compute_desc;
}

Glsl_program_desc load_glsl_program_desc(const char* filename)
{
	if (!filename) return Glsl_program_desc{};

	std::string fn;
	Glsl_program_desc glsl_desc;

	// vertex shader
	fn.append(filename);
	fn.append(".vertex.glsl");
	ENFORCE(exists(fn), EXCEPTION_MSG("The specified glsl file '", fn, "' does not exist."));
	glsl_desc.vertex_shader_source_code = load_text(fn);

	// pixel shader
	fn.clear();
	fn.append(filename);
	fn.append(".fragment.glsl");
	if (exists(fn))
		glsl_desc.fragment_shader_source_code = load_text(fn);

	return glsl_desc;
}

Glsl_program_desc load_glsl_program_desc(const char* vertex_filename, const char* fragment_filename)
{
	ENFORCE(exists(vertex_filename),
		EXCEPTION_MSG("The specified glsl file '", vertex_filename, "' does not exist."));

	Glsl_program_desc glsl_desc;
	glsl_desc.vertex_shader_source_code = load_text(vertex_filename);

	if (fragment_filename && (strlen(fragment_filename) > 0)) {
		ENFORCE(exists(fragment_filename),
			EXCEPTION_MSG("The specified glsl file '", fragment_filename, "' does not exist."));
		glsl_desc.fragment_shader_source_code = load_text(fragment_filename);
	}

	return glsl_desc;
}

Hlsl_shader_set_data load_hlsl_shader_set_data(const char* filename)
{
	ENFORCE(exists(filename), EXCEPTION_MSG("The specified hlsl file '", filename, "' does not exist."));
	Hlsl_shader_set_data data;

	data.source_code = load_text(filename);
	data.source_filename = filename;

	return data;
}

} // namespace data
} // namespace cg