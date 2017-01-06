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

Transform_feedback::Transform_feedback(Transform_feedback&& tf) noexcept
	: varying_names(std::move(tf.varying_names)),
	interleaved_buffer_mode(tf.interleaved_buffer_mode)
{
	tf.interleaved_buffer_mode = false;
}

Transform_feedback& Transform_feedback::operator=(Transform_feedback&& tf) noexcept
{
	if (this == &tf) return *this;

	varying_names = std::move(tf.varying_names);
	interleaved_buffer_mode = tf.interleaved_buffer_mode;

	tf.interleaved_buffer_mode = false;

	return *this;
}

// ----- funcs -----

bool operator==(const Transform_feedback& l, const Transform_feedback& r) noexcept
{
	return (l.interleaved_buffer_mode == r.interleaved_buffer_mode)
		&& (l.varying_names.size() == r.varying_names.size())
		&& std::equal(l.varying_names.cbegin(), l.varying_names.cend(), r.varying_names.cbegin());
}

std::ostream& operator<<(std::ostream& out, const Transform_feedback& tf)
{
	out << "Transform_feedback(" << tf.interleaved_buffer_mode;
	
	for (const std::string& var_name : tf.varying_names) {
		out << ", " << var_name;
	}

	out << ')';
	return out;
}

std::wostream& operator<<(std::wostream& out, const Transform_feedback& tf)
{
	std::wstring_convert<std::codecvt<wchar_t, char, std::mbstate_t>> converter;
	
	out << "Transform_feedback(" << tf.interleaved_buffer_mode;
	
	for (const std::string& var_name : tf.varying_names) {
		out << ", " << converter.from_bytes(var_name);
	}

	out << ')';
	return out;
}

Glsl_compute_data load_glsl_compute_data(const char* filename)
{
	ENFORCE(exists(filename),
		EXCEPTION_MSG("The specified glsl file '", filename, "' does not exist."));

	Glsl_compute_data compute_data;
	compute_data.compute_shader_source_code = load_text(filename);

	return compute_data;
}

Glsl_program_data load_glsl_program_data(const char* filename)
{
	if (!filename) return Glsl_program_data{};

	std::string fn;
	Glsl_program_data glsl_data;

	// vertex shader
	fn.append(filename);
	fn.append(".vertex.glsl");
	ENFORCE(exists(fn), EXCEPTION_MSG("The specified glsl file '", fn, "' does not exist."));
	glsl_data.vertex_shader_source_code = load_text(fn);

	// pixel shader
	fn.clear();
	fn.append(filename);
	fn.append(".fragment.glsl");
	if (exists(fn)) 
		glsl_data.fragment_shader_source_code = load_text(fn);

	return glsl_data;
}

Glsl_program_data load_glsl_program_data(const char* vertex_shader_filename, const char* fragment_shader_filename)
{
	ENFORCE(exists(vertex_shader_filename),
		EXCEPTION_MSG("The specified glsl file '", vertex_shader_filename, "' does not exist."));

	Glsl_program_data glsl_data;
	glsl_data.vertex_shader_source_code = load_text(vertex_shader_filename);

	if (strlen(fragment_shader_filename) > 0) {
		ENFORCE(exists(fragment_shader_filename),
			EXCEPTION_MSG("The specified glsl file '", fragment_shader_filename, "' does not exist."));
		glsl_data.fragment_shader_source_code = load_text(fragment_shader_filename);
	}

	return glsl_data;
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