#ifndef TECHNIQUE_DEFERRED_LIGHTING_RND_FILTER_SHADER_H_
#define TECHNIQUE_DEFERRED_LIGHTING_RND_FILTER_SHADER_H_

#include "cg/math/math.h"
#include "technique/deferred_lighting/rnd/opengl.h"


namespace deferred_lighting {
namespace rnd {

enum class Filter_type {
	none,
	box,
	gaussian
};

enum class Filter_kernel_radius {
	none,
	radius_03,
	radius_05,
	radius_11
};

// Filter_shader_program utilizes the fact the Box/Gaussian filter kernels are separable. 
// First the shader program filters initial texture along X axis (horizontally), 
// then  it filters the new texture along Y axis (vertically). Or vise versa.
class Filter_shader_program final {
public:

	Filter_shader_program() noexcept = default;

	Filter_shader_program(Filter_type filter_type, Filter_kernel_radius kernel_radius);

	Filter_shader_program(const Filter_shader_program&) = delete;

	Filter_shader_program(Filter_shader_program&& sp) noexcept;

	~Filter_shader_program() noexcept = default;


	Filter_shader_program& operator=(Filter_shader_program&& sp) noexcept;


	// Sets shader program ready to make the horizontal filter pass.
	void use_for_horizontal_pass() noexcept
	{
		use_for_pass(cg::uint2::unit_x);
	}

	// Sets shader program ready to make the vertical filter pass.
	void use_for_vertical_pass() noexcept
	{
		use_for_pass(cg::uint2::unit_y);
	}


private:
	void use_for_pass(const cg::uint2& direction) noexcept;

	Filter_type _filter_type = Filter_type::none;
	Filter_kernel_radius _kernel_radius = Filter_kernel_radius::none;
	Shader_program _prog;
	GLint _u_filter_direction_location = Invalid::uniform_location;
};


// Generates random vectors within a unit hemisphere.
// The hemisphere is located at (0, 0, 0) and oriendted along the positive OZ axis.
std::vector<cg::float3> generate_hemispherical_sample_kernel(size_t sample_count);

std::vector<cg::float3> generate_sphere_normalized_sample_kernel(size_t sample_count);

std::string get_filter_pixel_shader_filename(Filter_type filter_type, Filter_kernel_radius kernel_radius) noexcept;

} // namespace rnd
} // namespace deferred_lighting

#endif // TECHNIQUE_DEFERRED_LIGHTING_RND_FILTER_SHADER_H_
