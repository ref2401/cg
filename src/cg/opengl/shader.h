#ifndef CG_OPENGL_SHADER_H_
#define CG_OPENGL_SHADER_H_

#include <vector>
#include "cg/data/shader.h"
#include "cg/data/mesh.h"
#include "cg/math/math.h"
#include "cg/opengl/opengl_def.h"


namespace cg {
namespace opengl {

class Shader final {
public:

	static constexpr GLuint invalid_id = 0;

	static constexpr GLenum invalid_type = GL_NONE;


	Shader(GLenum type, const std::string& source_code);

	Shader(const Shader& shader_id) = delete;

	Shader(Shader&& shader_id) = delete;

	~Shader() noexcept;


	bool compiled() const noexcept;

	GLuint id() const noexcept
	{
		return _id;
	}

	std::string log() const noexcept;

	GLenum type() const noexcept
	{
		return _type;
	}

private:

	void dispose() noexcept;

	GLint get_property(GLenum prop) const noexcept;

	GLuint _id = Invalid::shader_id;
	GLenum _type;
};

class Shader_program final {
public:

	Shader_program(const std::string& name, const cg::data::Shader_program_source_code& src);

	Shader_program(const std::string& name, const Shader& vertex_shader, const Shader& pixel_shader);

	Shader_program(const std::string& name, const Shader& vertex_shader, const std::string& pixel_source_code);

	Shader_program(const Shader_program& prog) = delete;

	Shader_program(Shader_program&& prog) = delete;

	~Shader_program() noexcept;


	// Returns a location value of the specified uniform.
	// Throws if the uniform name does not belong to this shader program.
	GLint get_uniform_location(const std::string& uniform_name) const;

	// Returns a location value of the specified uniform.
	// Throws if the uniform name does not belong to this shader program.
	GLint get_uniform_location(const char* uniform_name) const;

	GLuint id() const noexcept
	{
		return _id;
	}

	bool linked() const noexcept;

	std::string log() const noexcept;

	const std::string& name() const noexcept
	{
		return _name;
	}

	bool valid() const noexcept;

private:

	void dispose() noexcept;

	GLint get_property(GLenum prop) const noexcept;


	GLuint _id = Invalid::shader_program_id;
	std::string _name;
};

// Desribis attribute indices within a particular shader.
// If a location equals to Invalid_id::vertex_attrib_location it means that there is no such attribute is the shader.
struct Vertex_attrib_layout {

	Vertex_attrib_layout() noexcept = default;

	Vertex_attrib_layout(GLint position_location, GLint normal_location,
		GLint tex_coord_location, GLint tangent_h_location) noexcept
		: position_location(position_location), normal_location(normal_location),
		tex_coord_location(tex_coord_location), tangent_h_location(tangent_h_location)
	{}


	cg::data::Vertex_attribs attribs() const noexcept;

	GLint position_location = Invalid::vertex_attrib_location;
	GLint normal_location = Invalid::vertex_attrib_location;
	GLint tex_coord_location = Invalid::vertex_attrib_location;
	GLint tangent_h_location = Invalid::vertex_attrib_location;
};


// Sets a uniform variable value.
// Params:
// -	location: Uniform location in the currently used shader program.
// -	value: New value to be used for the specified uniform variable.
template<typename T>
void set_uniform(GLint location, const T& value) noexcept;

template<>
inline void set_uniform<cg::mat4>(GLint location, const cg::mat4& mat) noexcept
{
	assert(location != Invalid::uniform_location);

	float arr[16];
	cg::put_in_column_major_order(mat, arr);
	glUniformMatrix4fv(location, 1, false, arr);
}

template<typename T>
void set_uniform_array(GLint location, const T* ptr, size_t count) noexcept;

template<>
inline void set_uniform_array<mat4>(GLint location, const mat4* ptr, size_t count) noexcept
{
	assert(location != Invalid::uniform_location);
	assert(count > 0);

	constexpr size_t component_count = 16; // each mat4 has exactly 16 components
	std::vector<float> arr(count * component_count);
	for (size_t i = 0; i < count; ++i) {
		cg::put_in_column_major_order(ptr[i], arr.data() + i * component_count);
	}

	glUniformMatrix4fv(location, count, false, arr.data());
}

} // namespace opengl
} // namespace cg

#endif // CG_OPENGL_SHADER_H_