#ifndef CG_DATA_SHADER_H_
#define CG_DATA_SHADER_H_

#include <string>


namespace cg {
namespace data {

struct Shader_program_source_code {
	
	Shader_program_source_code() = default;

	Shader_program_source_code(const std::string& vertex_source, const std::string& pixel_source)
		: vertex_source(vertex_source), pixel_source(pixel_source)
	{}

	Shader_program_source_code(const char* vertex_source, const char* pixel_source)
		: vertex_source(vertex_source), pixel_source(pixel_source)
	{}

	std::string vertex_source;
	std::string pixel_source;
};

} // namespace data
} // namespace cg

#endif // CG_DATA_SHADER_H_
