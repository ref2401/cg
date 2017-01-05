#ifndef CG_DATA_SHADER_H_
#define CG_DATA_SHADER_H_

#include <cstdint>
#include <string>


namespace cg {
namespace data {

struct Glsl_compute_data final {

	Glsl_compute_data() = default;

	Glsl_compute_data(const std::string& compute_shader_source_code)
		: compute_shader_source_code(compute_shader_source_code)
	{}

	Glsl_compute_data(const char* compute_shader_source_code)
		: compute_shader_source_code(compute_shader_source_code)
	{}


	std::string compute_shader_source_code;
};

// Glsl_program_data struct stores all required params which are used in glsl shader creation process.
struct Glsl_program_data {

	Glsl_program_data() = default;


	bool has_vertex_shader() const noexcept
	{
		return !vertex_shader_source_code.empty();
	}

	bool has_fragment_shader() const noexcept
	{
		return !fragment_shader_source_code.empty();
	}


	std::string vertex_shader_source_code;
	std::string fragment_shader_source_code;
};

// Hlsl5_shader_set_data struct stores all required and optional params
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


Glsl_compute_data load_glsl_compute_data(const char* filename);

inline Glsl_compute_data load_glsl_compute_data(const std::string& filename)
{
	return load_glsl_compute_data(filename.c_str());
}

// Loads all found glsl shader source code files.
// Each file name is constructed as: filename + .<shader_type> + .glsl
// Only vertex shader ia required. If vertex ource code file does not exist the function will throw.  
// Won't throw if tesselation or geometry source code files do not exist.
// Example:
//		load_glsl_program_source(../data/shader/blinn_phong);
//		The constructed filenames are:
//		- ../data/shader/blinn_phong.vertex.glsl
//		- ../data/shader/blinn_phong.pixel.glsl
Glsl_program_data load_glsl_program_data(const char* filename);

// Loads all found glsl shader source code files.
// Each file name is constructed as: filename + .<shader_type> + .glsl
// Only vertex shader ia required. If vertex ource code file does not exist the function will throw.  
// Won't throw if tesselation or geometry source code files do not exist.
// Example:
//		load_glsl_program_source(../data/shader/blinn_phong);
//		The constructed filenames are:
//		- ../data/shader/blinn_phong.vertex.glsl
//		- ../data/shader/blinn_phong.pixel.glsl
inline Glsl_program_data load_glsl_program_data(const std::string& filename)
{
	return load_glsl_program_data(filename.c_str());
}

// Loads the specified glsl shader source code files.
// Will not throw if fragment_filename is empty.
Glsl_program_data load_glsl_program_data(const char* vertex_filename, const char* fragment_filename);

// Loads the specified glsl shader source code files.
// Will not throw if fragment_filename is empty.
inline Glsl_program_data load_glsl_program_data(const std::string& vertex_filename,
	const std::string& fragment_filename)
{
	return load_glsl_program_data(vertex_filename.c_str(), fragment_filename.c_str());
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
