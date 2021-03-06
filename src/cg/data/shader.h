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

	bool transform_feedback_is_used() const noexcept
	{
		return tf_varying_names.size() > 0;
	}


	std::string name;
	std::string vertex_shader_source_code;
	std::string fragment_shader_source_code;
	std::vector<std::string> tf_varying_names;
	bool tf_interleaved_buffer_mode = true;
};

Glsl_compute_desc load_glsl_compute_desc(const char* name, const char* filename);

inline Glsl_compute_desc load_glsl_compute_desc(const std::string& name, const std::string& filename)
{
	return load_glsl_compute_desc(name.c_str(), filename.c_str());
}

// Loads all found glsl shader source code files.
// Each file name is constructed as: filename + .<shader_type> + .glsl
// Only vertex shader ia required. If vertex ource code file does not exist the function will throw.  
// Won't throw if tesselation or geometry source code files do not exist.
// example:
//		load_glsl_program_source(../../data/shader/blinn_phong);
//		The constructed filenames are:
//		- ../../data/shader/blinn_phong.vertex.glsl
//		- ../../data/shader/blinn_phong.pixel.glsl
Glsl_program_desc load_glsl_program_desc(const char* name, const char* filename);

// ditto
inline Glsl_program_desc load_glsl_program_desc(const std::string& name, const std::string& filename)
{
	return load_glsl_program_desc(name.c_str(), filename.c_str());
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


} // namespace data
} // namespace cg

#endif // CG_DATA_SHADER_H_
