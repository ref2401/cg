#ifndef CG_FILE_FILE_H_
#define CG_FILE_FILE_H_

#include "cg/data/shader.h"
#include "cg/file/file_base.h"
#include "cg/file/file_image.h"
#include "cg/file/file_mesh.h"


namespace cg {
namespace file {

// Loads all found glsl shader source code files.
// Each file name is constructed as: filename + .<shader_type> + .glsl
// Vertex & fragment shaders are required. If vertex or fragment source code file does not exist 
// the function will throw.  Won't throw if tesselation or geometry source code files do not exist.
// Example:
//		load_glsl_program_source(../data/shader/blinn_phong);
//		The constructed filenames are:
//		- ../data/shader/blinn_phong.vertex.glsl
//		- ../data/shader/blinn_phong.pixel.glsl
cg::data::Glsl_program_data load_glsl_program_data(const char* filename);

// Loads all found glsl shader source code files.
// Each file name is constructed as: filename + .<shader_type> + .glsl
// Vertex & fragment shaders are required. If vertex or fragment source code file does not exist 
// the function will throw.  Won't throw if tesselation or geometry source code files do not exist.
// Example:
//		load_glsl_program_source(../data/shader/blinn_phong);
//		The constructed filenames are:
//		- ../data/shader/blinn_phong.vertex.glsl
//		- ../data/shader/blinn_phong.pixel.glsl
inline cg::data::Glsl_program_data load_glsl_program_data(const std::string& filename)
{
	return load_glsl_program_data(filename.c_str());
}

// Loads the specified glsl shader source code files.
cg::data::Glsl_program_data load_glsl_program_data(const char* vertex_filename, const char* pixel_filename);

// Loads the specified glsl shader source code files.
inline cg::data::Glsl_program_data load_glsl_program_data(const std::string& vertex_filename,
	const std::string& pixel_filename)
{
	return load_glsl_program_data(vertex_filename.c_str(), pixel_filename.c_str());
}

// Loads the specified hlsl shader source code file.
// If the specified file does not exist the function will throw. 
cg::data::Hlsl_shader_set_data load_hlsl_shader_set_data(const char* filename);

// Loads the specified hlsl shader source code file.
// If the specified file does not exist the function will throw. 
inline cg::data::Hlsl_shader_set_data load_hlsl_shader_set_data(const std::string& filename)
{
	return load_hlsl_shader_set_data(filename.c_str());
}

} // namespace file
} // namespace cg


# endif //CG_FILE_FILE_H_
