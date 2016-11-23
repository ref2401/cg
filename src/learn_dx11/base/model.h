#ifndef LEARN_DX11_BASE_MODEL_H_
#define LEARN_DX11_BASE_MODEL_H_

#include <vector>
#include "cg/math/math.h"


namespace learn_dx11 {

class Animated_model final {
public:

	Animated_model(const char* filename);

	Animated_model(const std::string& filename) :
		Animated_model(filename.c_str())
	{}


	const std::vector<cg::float3>& positions() const noexcept
	{
		return _positions;
	}

private:

	std::vector<cg::float3> _positions;
};

} // namespace learn_dx11

#endif // LEARN_DX11_BASE_MODEL_H_

