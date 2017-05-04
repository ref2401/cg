#include "cg/data/model.h"

#include "cg/base/base.h"
#include "cg/data/model_assimp.h"


namespace {

using cg::data::Assimp_postprocess_flags;
using cg::data::Model_geometry_data;
using cg::data::Model_geometry_vertex;
using cg::data::vertex_attribs;
using cg::data::make_cg_vector;


template<vertex_attribs attribs>
Model_geometry_data<attribs> load_model(const char* filename, const Assimp_postprocess_flags& flags)
{
	Assimp::Importer importer;
	const aiScene* scene = cg::data::load_model(importer, filename, flags);
	
	size_t base_vertex = 0;
	size_t index_offset = 0;
	Model_geometry_data<attribs> geometry_data(scene->mNumMeshes);

	for (size_t mi = 0; mi < scene->mNumMeshes; ++mi) {
		const aiMesh* mesh = scene->mMeshes[mi];

		geometry_data.push_back_mesh(mesh->mNumVertices, base_vertex, mesh->mNumFaces * 3, index_offset);

		for (size_t vi = 0; vi < mesh->mNumVertices; ++vi) {
			process_vertex(geometry_data, mesh, vi);
		}

		for (size_t fi = 0; fi < mesh->mNumFaces; ++fi) {
			const aiFace& face = mesh->mFaces[fi];
			assert(face.mNumIndices == 3);

			geometry_data.push_back_indices(face.mIndices[0], face.mIndices[1], face.mIndices[2]);
		}

		base_vertex += mesh->mNumVertices;
		index_offset = mesh->mNumFaces * 3;
	}

	return geometry_data;
}

void process_vertex(Model_geometry_data<vertex_attribs::p>& geometry_data,
	const aiMesh* mesh, size_t vertex_index)
{
	assert(mesh);
	assert(vertex_index < mesh->mNumVertices);

	Model_geometry_vertex<vertex_attribs::p> vertex(
		make_cg_vector(mesh->mVertices[vertex_index])
	);

	geometry_data.push_back_vertex(vertex);
}

void process_vertex(Model_geometry_data<vertex_attribs::p_n>& geometry_data,
	const aiMesh* mesh, size_t vertex_index)
{
	assert(mesh);
	assert(vertex_index < mesh->mNumVertices);

	Model_geometry_vertex<vertex_attribs::p_n> vertex(
		make_cg_vector(mesh->mVertices[vertex_index]),
		normalize(make_cg_vector(mesh->mNormals[vertex_index])));

	geometry_data.push_back_vertex(vertex);
}

void process_vertex(Model_geometry_data<vertex_attribs::p_n_tc>& geometry_data,
	const aiMesh* mesh, size_t vertex_index)
{
	assert(mesh);
	assert(vertex_index < mesh->mNumVertices);

	Model_geometry_vertex<vertex_attribs::p_n_tc> vertex(
		make_cg_vector(mesh->mVertices[vertex_index]),
		normalize(make_cg_vector(mesh->mNormals[vertex_index])),
		make_cg_vector(mesh->mTextureCoords[0][vertex_index]).xy());

	geometry_data.push_back_vertex(vertex);
}

void process_vertex(Model_geometry_data<vertex_attribs::p_tc>& geometry_data,
	const aiMesh* mesh, size_t vertex_index)
{
	assert(mesh);
	assert(vertex_index < mesh->mNumVertices);

	Model_geometry_vertex<vertex_attribs::p_tc> vertex(
		make_cg_vector(mesh->mVertices[vertex_index]),
		make_cg_vector(mesh->mTextureCoords[0][vertex_index]).xy());

	geometry_data.push_back_vertex(vertex);
}

void process_vertex(Model_geometry_data<vertex_attribs::p_n_tc_ts>& geometry_data,
	const aiMesh* mesh, size_t vertex_index)
{
	assert(mesh);
	assert(vertex_index < mesh->mNumVertices);

	// NOTE(ref2401): bitangent is negated because assimp computes it in the wrong direction.

	float3 position = make_cg_vector(mesh->mVertices[vertex_index]);
	float3 normal = normalize(make_cg_vector(mesh->mNormals[vertex_index]));
	float2 tex_coord = make_cg_vector(mesh->mTextureCoords[0][vertex_index]).xy();
	float3 tangent = normalize(make_cg_vector(mesh->mTangents[vertex_index]));
	float3 bitangent = -normalize(make_cg_vector(mesh->mBitangents[vertex_index]));
	float4 tangent_h = cg::data::compute_tangent_handedness(tangent, bitangent, normal);

	Model_geometry_vertex<vertex_attribs::p_n_tc_ts> vertex(
		position, normal, tex_coord, tangent_h);
	
	geometry_data.push_back_vertex(vertex);
}

} // namespace


namespace cg {
namespace data {

// ----- funcs -----

std::ostream& operator<<(std::ostream& o, const Model_mesh_info& mi)
{
	o << "Model_mesh_info(" << mi.vertex_count << ", " << mi.base_vertex
		<< ", " << mi.index_count << ", " << mi.index_offset << ")";

	return o;
}

std::wostream& operator<<(std::wostream& o, const Model_mesh_info& mi)
{
	o << "Model_mesh_info(" << mi.vertex_count << ", " << mi.base_vertex
		<< ", " << mi.index_count << ", " << mi.index_offset << ")";

	return o;
}

template<>
Model_geometry_data<vertex_attribs::p> load_model<vertex_attribs::p>(const char* filename)
{
	Assimp_postprocess_flags flags = default_load_flags;
	return ::load_model<vertex_attribs::p>(filename, flags);
}

template<>
Model_geometry_data<vertex_attribs::p_n> load_model<vertex_attribs::p_n>(const char* filename)
{
	Assimp_postprocess_flags flags = default_load_flags | aiProcess_GenNormals;
	return ::load_model<vertex_attribs::p_n>(filename, flags);
}

template<>
Model_geometry_data<vertex_attribs::p_n_tc> load_model<vertex_attribs::p_n_tc>(const char* filename)
{
	Assimp_postprocess_flags flags = default_load_flags | aiProcess_GenNormals;
	return ::load_model<vertex_attribs::p_n_tc>(filename, flags);
}

template<>
Model_geometry_data<vertex_attribs::p_tc> load_model<vertex_attribs::p_tc>(const char* filename)
{
	Assimp_postprocess_flags flags = default_load_flags;
	return ::load_model<vertex_attribs::p_tc>(filename, flags);
}

template<>
Model_geometry_data<vertex_attribs::p_n_tc_ts> load_model<vertex_attribs::p_n_tc_ts>(const char* filename)
{
	Assimp_postprocess_flags flags = default_load_flags 
		| aiProcess_GenNormals | aiProcess_CalcTangentSpace;
	return ::load_model<vertex_attribs::p_n_tc_ts>(filename, flags);
}

} // namespace data
} // namespace cg
