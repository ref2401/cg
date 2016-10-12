#ifndef CG_RND_UTILITY_GAUSSIAN_FILTER_H_
#define CG_RND_UTILITY_GAUSSIAN_FILTER_H_

#include "cg/math/math.h"
#include "cg/rnd/opengl/opengl.h"


namespace cg {
namespace rnd {
namespace utility {

enum class Gaussian_filter_kernel_size {
	none,
	radius_5,
	radius_9
};

// Gaussian_filter_shader_program utilizes the fact the the Gaussian filter kernel is separable. 
// First filter initial texture along X axis (horizontally), 
// then filter the new texture along Y axis (vertically). Or vise versa.
class Gaussian_filter_shader_program final {
public:

	Gaussian_filter_shader_program() noexcept = default;

	Gaussian_filter_shader_program(Gaussian_filter_kernel_size kernel_size);

	Gaussian_filter_shader_program(const Gaussian_filter_shader_program&) = delete;

	Gaussian_filter_shader_program(Gaussian_filter_shader_program&& sp) noexcept;

	~Gaussian_filter_shader_program() noexcept = default;


	Gaussian_filter_shader_program& operator=(Gaussian_filter_shader_program&& sp) noexcept;


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

	Gaussian_filter_kernel_size _kernel_size = Gaussian_filter_kernel_size::none;
	cg::rnd::opengl::Shader_program _prog;
	GLint _u_filter_direction_location = cg::rnd::opengl::Invalid::uniform_location;
};


} // utility
} // namespace rnd
} // namespace cg

#endif // CG_RND_UTILITY_GAUSSIAN_FILTER_H_
