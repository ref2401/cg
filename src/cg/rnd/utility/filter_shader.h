#ifndef CG_RND_UTILITY_FILTER_SHADER_H_
#define CG_RND_UTILITY_FILTER_SHADER_H_

#include "cg/math/math.h"
#include "cg/rnd/opengl/opengl.h"


namespace cg {
namespace rnd {
namespace utility {

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
	cg::rnd::opengl::Shader_program _prog;
	GLint _u_filter_direction_location = cg::rnd::opengl::Invalid::uniform_location;
};


std::string get_filter_pixel_shader_filename(Filter_type filter_type, Filter_kernel_radius kernel_radius) noexcept;

} // utility
} // namespace rnd
} // namespace cg

#endif // CG_RND_UTILITY_FILTER_SHADER_H_
