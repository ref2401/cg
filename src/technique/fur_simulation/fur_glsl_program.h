#ifndef TECHNIQUE_FUR_SIMULATION_FUR_GLSL_PROGRAM_H_
#define TECHNIQUE_FUR_SIMULATION_FUR_GLSL_PROGRAM_H_

#include "cg/rnd/opengl/opengl.h"

using namespace cg;
using namespace cg::rnd::opengl;


namespace fur_simulation {

struct Material final {
	Texture_2d_immut tex_diffuse_rgb;
	float shadow_factor_power = 1.0f;
	float threshold_power = 1.0f;
	float curl_radius = 0.0f;
	float curl_frequency = 0.0;
	float specular_factor = 0.0;
	float specular_power = 1.0;
	float tex_coord_factor = 1.0f;
	size_t shell_count = 8;
};

} // namespace fur_simulation

#endif // TECHNIQUE_FUR_SIMULATION_FUR_GLSL_PROGRAM_H_

