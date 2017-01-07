#ifndef CG_DATA_MODEL_ASSIMP_H_
#define CG_DATA_MODEL_ASSIMP_H_

#include <utility>
#include "cg/math/math.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"


namespace cg {
namespace data {

using Assimp_postprocess_flags = std::underlying_type_t<aiPostProcessSteps>;

constexpr Assimp_postprocess_flags default_load_flags = aiProcess_ValidateDataStructure 
	| aiProcess_JoinIdenticalVertices;


const aiScene* load_model(Assimp::Importer& importer, const char* filename, 
	const Assimp_postprocess_flags& flags);

inline float3 make_cg_vector(const aiVector3D& v)
{
	return float3(v.x, v.y, v.z);
}

inline float2 make_cg_vector(const aiVector2D& v)
{
	return float2(v.x, v.y);
}

} // namespace data
} // namespace cg

#endif // CG_DATA_MODEL_ASSIMP_H_

