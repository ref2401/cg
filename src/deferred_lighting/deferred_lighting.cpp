#include "deferred_lighting/deferred_lighting.h"

#include "cg/sys/opengl.h"


namespace deferred_lighting {

void Deferred_lighting::render(float blend_state) 
{
	float rgb[4] = { 0.5f, 0.5f, 0.55f, 1.f };
	glClearBufferfv(GL_COLOR, 0, rgb);
}

} // namespace deferred_lighting