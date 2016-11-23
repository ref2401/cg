#include "learn_dx11/base/model.h"

#include <cassert>
#include <type_traits>
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"


using cg::float3;

namespace learn_dx11 {

Animated_model::Animated_model(const char* filename)
{
	assert(filename);
		
	Assimp::Importer importer;
	std::underlying_type_t <aiPostProcessSteps> flags = aiProcess_GenNormals 
		| aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices 
		| aiProcess_ValidateDataStructure;

	const aiScene* scene = importer.ReadFile(filename, flags);
	assert(scene);

	for (size_t i = 0; i < scene->mNumMeshes; ++i) {
		aiMesh* mesh = scene->mMeshes[i];
		
		for (size_t j = 0; j < mesh->mNumBones; ++j) {
			aiMatrix4x4 bp_matrix = mesh->mBones[j]->mOffsetMatrix;
			bp_matrix.Inverse();
			_positions.emplace_back(bp_matrix.a4, bp_matrix.b4, bp_matrix.c4);
		}
	}
}

} // namespace learn_dx11
