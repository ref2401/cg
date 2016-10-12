#include "cg/rnd/utility/gaussian_filter.h"

#include <cassert>
#include <utility>
#include "cg/file/file.h"

using namespace cg::rnd::opengl;


namespace {

using cg::data::Shader_program_source_code;
using cg::file::load_glsl_program_source;
using cg::rnd::utility::Gaussian_filter_kernel_size;

Shader_program_source_code load_gaussina_filter_source_code(Gaussian_filter_kernel_size kernel_size)
{
	assert(kernel_size != Gaussian_filter_kernel_size::none);

	const char* vertex_filename = "../data/utility_shaders/gaussian_filter.vertex.glsl";
	char* pixel_filename = "";

	switch (kernel_size) {
		case Gaussian_filter_kernel_size::radius_5:
			pixel_filename = "../data/utility_shaders/gaussian_filter_r5.pixel.glsl";
			break;

		case Gaussian_filter_kernel_size::radius_9:
			pixel_filename = "../data/utility_shaders/gaussian_filter_r9.pixel.glsl";
			break;
	}

	return load_glsl_program_source(vertex_filename, pixel_filename);
}

} // namespace


namespace cg {
namespace rnd {
namespace utility {

Gaussian_filter_shader_program::Gaussian_filter_shader_program(Gaussian_filter_kernel_size kernel_size) :
	_kernel_size(kernel_size),
	_prog("gaussian-filter", load_gaussina_filter_source_code(kernel_size)),
	_u_filter_direction_location(_prog.get_uniform_location("u_filter_direction"))
{}

Gaussian_filter_shader_program::Gaussian_filter_shader_program(Gaussian_filter_shader_program&& sp) noexcept :
	_kernel_size(sp._kernel_size),
	_prog(std::move(sp._prog)),
		_u_filter_direction_location(sp._u_filter_direction_location)
{
	sp._kernel_size = Gaussian_filter_kernel_size::none;
	sp._u_filter_direction_location = Invalid::uniform_location;
}

Gaussian_filter_shader_program& Gaussian_filter_shader_program::operator=(
	Gaussian_filter_shader_program&& sp) noexcept
{
	if (this == &sp) return *this;

	_kernel_size = sp._kernel_size;
	_prog = std::move(sp._prog);
	_u_filter_direction_location = sp._u_filter_direction_location;

	sp._kernel_size = Gaussian_filter_kernel_size::none;
	sp._u_filter_direction_location = Invalid::uniform_location;

	return *this;
}

void Gaussian_filter_shader_program::use_for_pass(const cg::uint2& direction) noexcept
{
	assert(_kernel_size != Gaussian_filter_kernel_size::none);
	assert(_prog.id() != Invalid::shader_program_id);

	glUseProgram(_prog.id());
	set_uniform(_u_filter_direction_location, direction);
}

} // utility
} // namespace rnd
} // namespace cg
