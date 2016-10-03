#ifndef TECHNIQUE_DEFERRED_LIGHTING_FRAME_H_
#define TECHNIQUE_DEFERRED_LIGHTING_FRAME_H_

#include <array>
#include "cg/math/math.h"
#include "cg/opengl/opengl.h"


namespace deferred_lighting {

struct Rnd_obj final {

	Rnd_obj(GLuint vao_id, const cg::opengl::DE_indirect_params& de_indirect_params,
		const std::array<float, 16>& model_matrix, const std::array<float, 9>& normal_matix) noexcept;

	GLuint vao_id;
	cg::opengl::DE_indirect_params de_indirect_params;
	std::array<float, 16> model_matrix;
	std::array<float, 9> normal_matrix;
};

// ...
// Implementation notes: Frame temporary contains an vao id and assumes 
// that all the DE_cmd objects added to it refere to the vao id.
class Frame final {
public:

	Frame();

	Frame(const Frame& frame) = delete;

	Frame(Frame&& frame) = delete;


	// Call begin before filling the frame with scene data.
	void begin() noexcept;

	// Call end after frame has been filled with scene data and must be renderer.
	void end();

	const cg::mat4& projection_matrix() const noexcept
	{
		return _projection_matrix;
	}

	void set_projection_matrix(const cg::mat4& mat) noexcept
	{
		_projection_matrix = mat;
	}

	// Returns result of projection_matrix() * view_matrix()
	cg::mat4 projection_view_matrix() const noexcept
	{
		return _projection_matrix * _view_matrix;
	}

	void push_back_renderable(const cg::opengl::DE_cmd& cmd, const cg::mat4& model_matrix);

	const std::vector<Rnd_obj>& rnd_objects() const noexcept
	{
		return _rnd_objects;
	}

	const cg::mat4& view_matrix() const noexcept
	{
		return _view_matrix;
	}

	void set_view_matrix(const cg::mat4& mat) noexcept
	{
		_view_matrix = mat;
	}

private:
	std::vector<Rnd_obj> _rnd_objects;
	cg::mat4 _projection_matrix = cg::mat4::identity;
	cg::mat4 _view_matrix = cg::mat4::identity;
};

} // namespace deferred_lighting

#endif //TECHNIQUE_DEFERRED_LIGHTING_FRAME_H_