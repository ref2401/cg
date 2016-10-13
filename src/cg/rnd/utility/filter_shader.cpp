#include "cg/rnd/utility/filter_shader.h"

#include <cassert>
#include <utility>
#include "cg/file/file.h"

using namespace cg::rnd::opengl;


namespace {

using cg::data::Shader_program_source_code;
using cg::file::load_glsl_program_source;
using cg::rnd::utility::Filter_kernel_radius;

Shader_program_source_code load_gaussina_filter_source_code(Filter_kernel_radius kernel_radius)
{
	assert(kernel_radius != Filter_kernel_radius::none);

	const char* vertex_filename = "../data/utility_shaders/filter.vertex.glsl";
	char* pixel_filename = "";

	switch (kernel_radius) {
		case Filter_kernel_radius::radius_03:
			pixel_filename = "../data/utility_shaders/gaussian_filter_r03.pixel.glsl";
			break;

		case Filter_kernel_radius::radius_05:
			pixel_filename = "../data/utility_shaders/gaussian_filter_r05.pixel.glsl";
			break;

		case Filter_kernel_radius::radius_11:
			pixel_filename = "../data/utility_shaders/gaussian_filter_r11.pixel.glsl";
			break;
	}

	return load_glsl_program_source(vertex_filename, pixel_filename);
}

} // namespace


namespace cg {
namespace rnd {
namespace utility {

// ----- Filter_shader_program -----

Filter_shader_program::Filter_shader_program(Filter_type filter_type, Filter_kernel_radius kernel_radius) :
	_filter_type(filter_type),
	_kernel_radius(kernel_radius)
{
	assert(_filter_type != Filter_type::none);
	assert(_kernel_radius != Filter_kernel_radius::none);

	cg::data::Shader_program_source_code src_code = cg::file::load_glsl_program_source(
		"../data/utility_shaders/filter.vertex.glsl", 
		get_filter_pixel_shader_filename(_filter_type, _kernel_radius));

	_prog = cg::rnd::opengl::Shader_program("filter-shader", src_code);
	_u_filter_direction_location = _prog.get_uniform_location("u_filter_direction");
}

Filter_shader_program::Filter_shader_program(Filter_shader_program&& sp) noexcept :
	_filter_type(sp._filter_type),
	_kernel_radius(sp._kernel_radius),
	_prog(std::move(sp._prog)),
		_u_filter_direction_location(sp._u_filter_direction_location)
{
	sp._filter_type = Filter_type::none;
	sp._kernel_radius = Filter_kernel_radius::none;
	sp._u_filter_direction_location = Invalid::uniform_location;
}

Filter_shader_program& Filter_shader_program::operator=(
	Filter_shader_program&& sp) noexcept
{
	if (this == &sp) return *this;

	_filter_type = sp._filter_type;
	_kernel_radius = sp._kernel_radius;
	_prog = std::move(sp._prog);
	_u_filter_direction_location = sp._u_filter_direction_location;

	sp._filter_type = Filter_type::none;
	sp._kernel_radius = Filter_kernel_radius::none;
	sp._u_filter_direction_location = Invalid::uniform_location;

	return *this;
}

void Filter_shader_program::use_for_pass(const cg::uint2& direction) noexcept
{
	assert(_kernel_radius != Filter_kernel_radius::none);
	assert(_prog.id() != Invalid::shader_program_id);

	glUseProgram(_prog.id());
	set_uniform(_u_filter_direction_location, direction);
}

// ----- funcs -----

std::string get_filter_pixel_shader_filename(Filter_type filter_type, Filter_kernel_radius kernel_radius) noexcept
{
	// NOTE(ref2401): no unittests for the function, cos it's going to perish somedsay.

	assert(filter_type != Filter_type::none);
	assert(kernel_radius != Filter_kernel_radius::none);
	
	std::string filter_type_name;
	switch (filter_type) {
		case Filter_type::box:
			filter_type_name = "box";
			break;

		case Filter_type::gaussian:
			filter_type_name = "gaussian";
			break;
	}

	assert(!filter_type_name.empty());

	std::string kernel_radius_name;
	switch (kernel_radius) {
		case Filter_kernel_radius::radius_03:
			kernel_radius_name = "r03";
			break;

		case Filter_kernel_radius::radius_05:
			kernel_radius_name = "r05";
			break;

		case Filter_kernel_radius::radius_11:
			kernel_radius_name = "r11";
			break;
	}

	assert(!kernel_radius_name.empty());

	// ../data/utility_shaders/{filter_type_name}_filter_{kernel_radius_name}.pixel.glsl
	std::string filename = "../data/utility_shaders/";
	filename.reserve(19 + filter_type_name.size() + kernel_radius_name.size());
	filename.append(filter_type_name);
	filename.append("_filter_");
	filename.append(kernel_radius_name);
	filename.append(".pixel.glsl");

	return filename;
}

} // utility
} // namespace rnd
} // namespace cg
