#ifndef CG_FILE_FILE_H_
#define CG_FILE_FILE_H_

#include "cg/data/shader.h"
#include "cg/file/file_base.h"
#include "cg/file/file_image.h"
#include "cg/file/file_mesh.h"


namespace cg {
namespace file {

// Loads all found glsl shader source code files.
// Each file name is constructed as filename + .<shader_type> + .glsl
// Won't throw if a file does not exist.
// Example:
//		load_glsl_program_source(../data/shader/blinn_phong);
//		The constructed filenames are:
//		- ../data/shader/blinn_phong.vertex.glsl
//		- ../data/shader/blinn_phong.pixel.glsl
cg::data::Shader_program_source_code load_glsl_program_source(const char* filename);

// Loads all found glsl shader source code files.
// Each file name is constructed as filename + .<shader_type> + .glsl
// Won't throw if a file does not exist.
// Example:
//		load_glsl_program_source(../data/shader/blinn_phong);
//		The constructed filenames are:
//		- ../data/shader/blinn_phong.vertex.glsl
//		- ../data/shader/blinn_phong.pixel.glsl
inline cg::data::Shader_program_source_code load_glsl_program_source(const std::string& filename)
{
	return load_glsl_program_source(filename.c_str());
}

// Loads all found hlsl shader source code files.
// Each file name is constructed as filename + .<shader_type> + .glsl
// Won't throw if a file does not exist.
// Example:
//		load_hlsl_program_source(../data/shader/blinn_phong);
//		The constructed filenames are:
//		- ../data/shader/blinn_phong.vertex.hlsl
//		- ../data/shader/blinn_phong.pixel.hlsl
cg::data::Shader_program_source_code load_hlsl_program_source(const char* filename);

// Loads all found hlsl shader source code files.
// Each file name is constructed as filename + .<shader_type> + .glsl
// Won't throw if a file does not exist.
// Example:
//		load_hlsl_program_source(../data/shader/blinn_phong);
//		The constructed filenames are:
//		- ../data/shader/blinn_phong.vertex.hlsl
//		- ../data/shader/blinn_phong.pixel.hlsl
inline cg::data::Shader_program_source_code load_hlsl_program_source(const std::string& filename)
{
	return load_hlsl_program_source(filename.c_str());
}

// Loads the specified glsl/hlsl shader source code files.
cg::data::Shader_program_source_code load_shader_program_source(const char* vertex_filename, const char* pixel_filename);

// Loads the specified glsl/hlsl shader source code files.
inline cg::data::Shader_program_source_code load_shader_program_source(const std::string& vertex_filename,
	const std::string& pixel_filename)
{
	return load_shader_program_source(vertex_filename.c_str(), pixel_filename.c_str());
}

} // namespace file
} // namespace cg


# endif //CG_FILE_FILE_H_
