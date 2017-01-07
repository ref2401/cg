#include "cg/data/model_assimp.h"

#include <cassert>
#include "cg/base/base.h"
#include "cg/data/file.h"


namespace cg {
namespace data {

const aiScene* load_model(Assimp::Importer& importer, const char* filename, const Assimp_postprocess_flags& flags)
{
	ENFORCE(exists(filename), EXCEPTION_MSG("Geometry file ", filename, " does not exist."));

	const aiScene* scene = importer.ReadFile(filename, flags);
	if (!scene) {
		auto exc_msg = EXCEPTION_MSG("Error loading model file ",
			filename, ".", importer.GetErrorString());
		throw std::runtime_error(exc_msg);
	}

	ENFORCE(scene->mNumMeshes > 0, "File ", filename, " does not contain any meshes.");

	return scene;
}

} // namespace data
} // namespace cg
