#ifndef CG_DATA_SHADER_H_
#define CG_DATA_SHADER_H_

#include <cassert>
#include <cstdint>
#include <ostream>
#include <string>
#include <vector>


namespace cg {
namespace data {

struct Glsl_compute_desc final {

	Glsl_compute_desc() = default;

	Glsl_compute_desc(const std::string& name, const std::string& compute_shader_source_code)
		: name(name), compute_shader_source_code(compute_shader_source_code)
	{}

	Glsl_compute_desc(const char* name, const char* compute_shader_source_code)
		: name(name), compute_shader_source_code(compute_shader_source_code)
	{}


	std::string name;
	std::string compute_shader_source_code;
};

struct Transform_feedback_desc final {

	Transform_feedback_desc() noexcept = default;

	Transform_feedback_desc(const Transform_feedback_desc&) = default;

	Transform_feedback_desc(Transform_feedback_desc&& tf) noexcept;


	Transform_feedback_desc& operator=(const Transform_feedback_desc&) = default;

	Transform_feedback_desc& operator=(Transform_feedback_desc&& tf) noexcept;


	bool is_used() const noexcept
	{
		return varying_names.size() > 0;
	}


	std::vector<std::string> varying_names;
	bool interleaved_buffer_mode = false;
};

// Glsl_program_desc struct stores all required params 
// which are used in glsl shader creation process.
struct Glsl_program_desc {

	bool has_vertex_shader() const noexcept
	{
		return !vertex_shader_source_code.empty();
	}

	bool has_fragment_shader() const noexcept
	{
		return !fragment_shader_source_code.empty();
	}


	std::string name;
	std::string vertex_shader_source_code;
	std::string fragment_shader_source_code;
	Transform_feedback_desc transform_feedback;
};

// Hlsl_shader_set_data struct stores all required and optional params
// which are used in hlsl shader creation process.
struct Hlsl_shader_set_data final {

	Hlsl_shader_set_data() = default;


	// Returns true if vertex shader's entry point is specified.
	bool has_vertex_shader() const noexcept
	{
		return !vertex_shader_entry_point.empty();
	}

	// Returns true if hull shader's entry point is specified.
	bool has_hull_shader() const noexcept
	{
		return !hull_shader_entry_point.empty();
	}

	// Returns true if domain shader's entry point is specified.
	bool has_domain_shader() const noexcept
	{
		return !domain_shader_entry_point.empty();
	}

	// Returns true if pixel shader's entry point is specified.
	bool has_pixel_shader() const noexcept
	{
		return !pixel_shader_entry_point.empty();
	}


	// Hlsl source code.
	std::string source_code;

	// Hlsl source code filename. Use in error messages.
	std::string source_filename;

	// The name of a function where vertex shader execution begins.
	std::string vertex_shader_entry_point;

	// The name of a function where hull shader execution begins.
	std::string hull_shader_entry_point;

	// The name of a function where domain shader execution begins.
	std::string domain_shader_entry_point;

	// The name of a function where pixel shader execution begins.
	std::string pixel_shader_entry_point;

	// The D3DCOMPILE constants specify how the compiler compiles the HLSL code.
	uint32_t compile_flags = 0;
};


bool operator==(const Transform_feedback_desc& l, const Transform_feedback_desc& r) noexcept;

inline bool operator!=(const Transform_feedback_desc& l, const Transform_feedback_desc& r) noexcept
{
	return !(l == r);
}

std::ostream& operator<<(std::ostream& out, const Transform_feedback_desc& tf);

std::wostream& operator<<(std::wostream& out, const Transform_feedback_desc& tf);

Glsl_compute_desc load_glsl_compute_desc(const char* name, const char* filename);

inline Glsl_compute_desc load_glsl_compute_desc(const std::string& name, const std::string& filename)
{
	return load_glsl_compute_desc(name.c_str(), filename.c_str());
}

namespace internal {

inline void puch_back_varying_name(const Transform_feedback_desc& tf)
{
	return;
}

template<typename... Args>
void puch_back_varying_name(Transform_feedback_desc& tf, const char* name, const Args... varying_names)
{
	assert(strlen(name) > 0);
	tf.varying_names.emplace_back(name);
	puch_back_varying_name(tf, varying_names...);
}

template<typename... Args>
void puch_back_varying_name(Transform_feedback_desc& tf, const std::string& name, const Args... varying_names)
{
	assert(name.size() > 0);
	tf.varying_names.push_back(name);
	puch_back_varying_name(tf, varying_names...);
}

} // namespace internal

// Loads all found glsl shader source code files.
// Each file name is constructed as: filename + .<shader_type> + .glsl
// Only vertex shader ia required. If vertex ource code file does not exist the function will throw.  
// Won't throw if tesselation or geometry source code files do not exist.
// Example:
//		load_glsl_program_source(../data/shader/blinn_phong);
//		The constructed filenames are:
//		- ../data/shader/blinn_phong.vertex.glsl
//		- ../data/shader/blinn_phong.pixel.glsl
Glsl_program_desc load_glsl_program_desc(const char* name, const char* filename);

// ditto
inline Glsl_program_desc load_glsl_program_desc(const std::string& name, const std::string& filename)
{
	return load_glsl_program_desc(name.c_str(), filename.c_str());
}

// ditto
template<typename... Args>
Glsl_program_desc load_glsl_program_desc(const char* name, const char* filename,
	bool interleaved_buffer_mode, const Args&... varying_names)
{
	Glsl_program_desc desc = load_glsl_program_desc(name, filename);

	desc.transform_feedback.interleaved_buffer_mode = interleaved_buffer_mode;
	internal::puch_back_varying_name(desc.transform_feedback, varying_names...);

	return desc;
}

// ditto
template<typename... Args>
inline Glsl_program_desc load_glsl_program_desc(const std::string& name, const std::string& filename,
	bool interleaved_buffer_mode, const Args&... varying_names)
{
	return load_glsl_program_desc(name.c_str(), filename.c_str(), interleaved_buffer_mode, varying_names...);
}

// Loads the specified glsl shader source code files.
// Will not throw if fragment_filename is empty.
Glsl_program_desc load_glsl_program_desc(const char* name, const char* vertex_filename, const char* fragment_filename);

// ditto
inline Glsl_program_desc load_glsl_program_desc(const std::string& name, 
	const std::string& vertex_filename, const std::string& fragment_filename)
{
	return load_glsl_program_desc(name.c_str(), vertex_filename.c_str(), fragment_filename.c_str());
}

// ditto
template<typename... Args>
Glsl_program_desc load_glsl_program_desc(const char* name, const char* vertex_filename, const char* fragment_filename,
	bool interleaved_buffer_mode, const Args&... varying_names)
{
	Glsl_program_desc desc = load_glsl_program_desc(name, vertex_filename, fragment_filename);

	desc.transform_feedback.interleaved_buffer_mode = interleaved_buffer_mode;
	internal::puch_back_varying_name(desc.transform_feedback, varying_names...);

	return desc;
}

// ditto
template<typename... Args>
inline Glsl_program_desc load_glsl_program_desc(const std::string& name, const std::string& vertex_filename, 
	const std::string& fragment_filename, bool interleaved_buffer_mode, const Args&... varying_names)
{
	return load_glsl_program_desc(name.c_str(), vertex_filename.c_str(), 
		fragment_filename.c_str(), interleaved_buffer_mode, varying_names...);
}

// Loads the specified hlsl shader source code file.
// If the specified file does not exist the function will throw. 
Hlsl_shader_set_data load_hlsl_shader_set_data(const char* filename);

// Loads the specified hlsl shader source code file.
// If the specified file does not exist the function will throw. 
inline Hlsl_shader_set_data load_hlsl_shader_set_data(const std::string& filename)
{
	return load_hlsl_shader_set_data(filename.c_str());
}

} // namespace data
} // namespace cg

#endif // CG_DATA_SHADER_H_
