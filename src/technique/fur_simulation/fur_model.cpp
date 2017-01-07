#include "technique/fur_simulation/fur_model.h"

#include <cassert>
#include "cg/data/model_assimp.h"

using namespace cg;
using namespace cg::data;


namespace fur_simulation {

// ----- Model_geometry_data -----

Model_geometry_data::Model_geometry_data(float strand_length, const char* geometry_filename)
{
	assert(strand_length > 0.0f);

	Assimp::Importer importer;
	const aiScene* scene = load_model(importer, geometry_filename, aiProcess_ValidateDataStructure
		| aiProcess_JoinIdenticalVertices | aiProcess_GenNormals | aiProcess_CalcTangentSpace);
	
	size_t base_vertex = 0;
	size_t index_offset = 0;

	// for each mesh
	for (size_t mi = 0; mi < scene->mNumMeshes; ++mi) {
		const aiMesh* mesh = scene->mMeshes[mi];
		meshes.emplace_back(mesh->mNumVertices, base_vertex, mesh->mNumFaces * 3, index_offset);

		// for each vertex of the mesh
		for (size_t vi = 0; vi < mesh->mNumVertices; ++vi) {
			float3 p_base = make_cg_vector(mesh->mVertices[vi]);
			float3 normal = normalize(make_cg_vector(mesh->mNormals[vi]));
			float2 tex_coord = make_cg_vector(mesh->mTextureCoords[0][vi]).uv();
			float3 tangent = normalize(make_cg_vector(mesh->mTangents[vi]));
			float3 bitangent = normalize(make_cg_vector(mesh->mBitangents[vi]));

			float3 p_rest(p_base + strand_length * normal);
			float3 p_curr = p_rest;
			float4 tangent_h = compute_tangent_handedness(tangent, bitangent, normal);

			push_back_position_buffer_data(p_base, p_rest);
			push_back_simulation_buffer_data(p_curr, float3::zero);
			push_back_model_attribs_buffer_data(normal, tex_coord, tangent_h);
		}

		for (size_t fi = 0; fi < mesh->mNumFaces; ++fi) {
			const aiFace& face = mesh->mFaces[fi];
			assert(face.mNumIndices == 3);

			push_back_index_buffer_data(face.mIndices[0], face.mIndices[1], face.mIndices[2]);
		}

		base_vertex += mesh->mNumVertices;
		index_offset = mesh->mNumFaces * 3;
	}
}

void Model_geometry_data::push_back_position_buffer_data(const cg::float3& p_base, const cg::float3& p_rest)
{
	position_buffer.push_back(p_base.x);
	position_buffer.push_back(p_base.y);
	position_buffer.push_back(p_base.z);
	position_buffer.push_back(p_rest.x);
	position_buffer.push_back(p_rest.y);
	position_buffer.push_back(p_rest.z);
}

void Model_geometry_data::push_back_simulation_buffer_data(const cg::float3& p_curr, const cg::float3& velocity)
{
	simulation_buffer.push_back(p_curr.x);
	simulation_buffer.push_back(p_curr.y);
	simulation_buffer.push_back(p_curr.z);
	simulation_buffer.push_back(velocity.x);
	simulation_buffer.push_back(velocity.y);
	simulation_buffer.push_back(velocity.z);
}

void Model_geometry_data::push_back_model_attribs_buffer_data(const cg::float3& normal,
	const cg::float2& tex_coord, const cg::float4& tangent_h)
{
	model_attribs_buffer.push_back(normal.x);
	model_attribs_buffer.push_back(normal.y);
	model_attribs_buffer.push_back(normal.z);
	model_attribs_buffer.push_back(tex_coord.u);
	model_attribs_buffer.push_back(tex_coord.v);
	model_attribs_buffer.push_back(tangent_h.x);
	model_attribs_buffer.push_back(tangent_h.y);
	model_attribs_buffer.push_back(tangent_h.z);
	model_attribs_buffer.push_back(tangent_h.w);

}

void Model_geometry_data::push_back_index_buffer_data(uint32_t i0, uint32_t i1, uint32_t i2)
{
	index_buffer.push_back(i0);
	index_buffer.push_back(i1);
	index_buffer.push_back(i2);
}

} // fur_simlation
