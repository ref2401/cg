#ifndef CG_DATA_SHADER_H_
#define CG_DATA_SHADER_H_

#include <cstdint>
#include <string>


namespace cg {
namespace data {

// Glsl_program_data struct stores all required params which are used in glsl shader creation process.
struct Glsl_program_data {

	Glsl_program_data() = default;

	Glsl_program_data(const std::string& vertex_shader_source_code, const std::string& fragment_shader_source_code)
		: vertex_shader_source_code(vertex_shader_source_code), fragment_shader_source_code(fragment_shader_source_code)
	{}

	Glsl_program_data(const char* vertex_shader_source_code, const char* fragment_shader_source_code)
		: vertex_shader_source_code(vertex_shader_source_code), fragment_shader_source_code(fragment_shader_source_code)
	{}


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

	// The name of a function where pixel shader execution begins.
	std::string pixel_shader_entry_point;

	// The D3DCOMPILE constants specify how the compiler compiles the HLSL code.
	uint32_t compile_flags = 0;
};

} // namespace data
} // namespace cg

#endif // CG_DATA_SHADER_H_
