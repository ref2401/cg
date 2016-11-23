#include "learn_dx11/mesh_rnd/vertex_skinning_example.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"


namespace learn_dx11 {
namespace mesh_rnd {

Vertex_skinning_example::Vertex_skinning_example(Render_context& rnd_ctx) :
	Example(rnd_ctx)
{
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile("../data/models/bob_lamp/boblampclean.md5mesh",
		aiProcess_GenNormals | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices | aiProcess_ValidateDataStructure);
}

void Vertex_skinning_example::on_viewport_resize(const cg::uint2& viewport_size)
{
}

void Vertex_skinning_example::render()
{
}

void Vertex_skinning_example::update()
{
}

} // namespace mesh_rnd
} // namespace learn_dx11