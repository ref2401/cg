#include "technique/deferred_lighting/frame.h"

#include <cassert>
#include <algorithm>

using cg::float3;
using cg::mat3;
using cg::mat4;
using cg::opengl::DE_indirect_params;
using cg::opengl::DE_cmd;
using cg::opengl::Invalid;


namespace deferred_lighting {

// ----- Rnd_obj -----

Rnd_obj::Rnd_obj(GLuint vao_id, const cg::opengl::DE_indirect_params& de_indirect_params,
	const cg::mat4& model_matrix, const cg::mat3& normal_matrix) noexcept :
	vao_id(vao_id),
	de_indirect_params(de_indirect_params),
	model_matrix(model_matrix),
	normal_matrix(normal_matrix)
{
	assert(vao_id != Invalid::vao_id);
}

// ----- Frame -----

Frame::Frame()
{
	_rnd_objects.reserve(16);
}

void Frame::begin() noexcept
{
	_rnd_objects.clear();
	_projection_matrix = mat4::identity;
	_view_matrix = mat4::identity;
}

void Frame::end()
{
	std::sort(_rnd_objects.begin(), _rnd_objects.end(), 
		[](const Rnd_obj& l, const Rnd_obj& r) { return l.vao_id < r.vao_id; });
}

void Frame::push_back_renderable(const DE_cmd& cmd, const mat4& model_matrix)
{
	//_rnd_objects.emplace_back(cmd, model_matrix, static_cast<mat3>(model_matrix));
}


} // namespace deferred_lighting