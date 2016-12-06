#include "cg/data/model.h"

#include "cg/base/base.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"


using cg::float2;
using cg::float3;
using cg::float4;
using Assimp_load_flags = std::underlying_type_t<aiPostProcessSteps>;

constexpr Assimp_load_flags default_load_flags = aiProcess_ValidateDataStructure
	| aiProcess_JoinIdenticalVertices;


namespace {

using cg::data::Model_geometry_data;
using cg::data::Model_geometry_vertex;
using cg::data::Vertex_attribs_new;

inline float3 make_cg_vector(const aiVector3D& v)
{
	return float3(v.x, v.y, v.z);
}

inline float2 make_cg_vector(const aiVector2D& v)
{
	return float2(v.x, v.y);
}

template<Vertex_attribs_new attribs>
Model_geometry_data<attribs> load_model(const char* filename, const Assimp_load_flags& flags)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filename, flags);

	if (!scene) {
		auto exc_msg = EXCEPTION_MSG("Error loading model file ", 
			filename, ".", importer.GetErrorString());
		throw std::runtime_error(exc_msg);
	}

	ENFORCE(scene->mNumMeshes > 0, "File ", filename, " does not contain any meshes.");

	return process_meshes<attribs>(scene);
}

void process_vertex(Model_geometry_data<Vertex_attribs_new::p>& geometry_data,
	const aiMesh* mesh, size_t vertex_index)
{
	assert(mesh);
	assert(vertex_index < mesh->mNumVertices);

	Model_geometry_vertex<Vertex_attribs_new::p> vertex(
		make_cg_vector(mesh->mVertices[vertex_index])
	);

	geometry_data.push_back_vertex(vertex);
}

void process_vertex(Model_geometry_data<Vertex_attribs_new::p_n>& geometry_data,
	const aiMesh* mesh, size_t vertex_index)
{
	assert(mesh);
	assert(vertex_index < mesh->mNumVertices);

	Model_geometry_vertex<Vertex_attribs_new::p_n> vertex(
		make_cg_vector(mesh->mVertices[vertex_index]),
		make_cg_vector(mesh->mNormals[vertex_index]));

	geometry_data.push_back_vertex(vertex);
}

void process_vertex(Model_geometry_data<Vertex_attribs_new::p_n_tc>& geometry_data,
	const aiMesh* mesh, size_t vertex_index)
{
	assert(mesh);
	assert(vertex_index < mesh->mNumVertices);

	Model_geometry_vertex<Vertex_attribs_new::p_n_tc> vertex(
		make_cg_vector(mesh->mVertices[vertex_index]),
		make_cg_vector(mesh->mNormals[vertex_index]),
		make_cg_vector(mesh->mTextureCoords[0][vertex_index]).uv());

	geometry_data.push_back_vertex(vertex);
}

void process_vertex(Model_geometry_data<Vertex_attribs_new::p_tc>& geometry_data,
	const aiMesh* mesh, size_t vertex_index)
{
	assert(mesh);
	assert(vertex_index < mesh->mNumVertices);

	Model_geometry_vertex<Vertex_attribs_new::p_tc> vertex(
		make_cg_vector(mesh->mVertices[vertex_index]),
		make_cg_vector(mesh->mTextureCoords[0][vertex_index]).uv());

	geometry_data.push_back_vertex(vertex);
}

void process_vertex(Model_geometry_data<Vertex_attribs_new::p_n_tc_ts>& geometry_data,
	const aiMesh* mesh, size_t vertex_index)
{
	assert(mesh);
	assert(vertex_index < mesh->mNumVertices);


	float3 position = make_cg_vector(mesh->mVertices[vertex_index]);
	float3 normal = make_cg_vector(mesh->mNormals[vertex_index]);
	float2 tex_coord = make_cg_vector(mesh->mTextureCoords[0][vertex_index]).uv();
	float3 tangent = make_cg_vector(mesh->mTangents[vertex_index]);
	float3 bitangent = make_cg_vector(mesh->mBitangents[vertex_index]);
	float4 tangent_h = cg::data::compute_tangent_handedness(tangent, bitangent, normal);

	Model_geometry_vertex<Vertex_attribs_new::p_n_tc_ts> vertex(
		position, normal, tex_coord, tangent_h);
	
	geometry_data.push_back_vertex(vertex);
}

template<Vertex_attribs_new attribs>
Model_geometry_data<attribs> process_meshes(const aiScene* scene)
{
	assert(scene);
	assert(scene->mNumMeshes > 0);

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

} // namespace


namespace cg {
namespace data {

// ----- funcs -----

std::ostream& operator<<(std::ostream& out, const Vertex_attribs_new& attribs)
{
	out << "Vertex_attribs(";

	switch (attribs) {
		case Vertex_attribs_new::p:
			out << 'p';
			break;
		
		case Vertex_attribs_new::p_n:
			out << "p_n";
			break;

		case Vertex_attribs_new::p_n_tc:
			out << "p_n_tc";
			break;

		case Vertex_attribs_new::p_tc:
			out << "p_tc";
			break;

		case Vertex_attribs_new::p_n_tc_ts:
			out << "p_n_tc_ts";
			break;
	}

	out << ")";
	return out;
}

std::wostream& operator<<(std::wostream& out, const Vertex_attribs_new& attribs)
{
	out << "Vertex_attribs(";

	switch (attribs) {
		case Vertex_attribs_new::p:
			out << 'p';
			break;

		case Vertex_attribs_new::p_n:
			out << "p_n";
			break;

		case Vertex_attribs_new::p_n_tc:
			out << "p_n_tc";
			break;

		case Vertex_attribs_new::p_tc:
			out << "p_tc";
			break;

		case Vertex_attribs_new::p_n_tc_ts:
			out << "p_n_tc_ts";
			break;
	}

	out << ")";
	return out;
}

float4 compute_tangent_handedness(const float3& tangent,
	const float3& bitangent, const float3& normal) noexcept
{
	assert(is_normalized(tangent));
	assert(is_normalized(bitangent));
	assert(is_normalized(normal));

	// Gram-Schmidt orthogonalize.
	// project tangent vector onto normal.
	float3 t_prj_n = normal * dot(tangent, normal);
	float3 t = normalize(tangent - t_prj_n);
	float h = (dot(bitangent, cross(normal, t)) >= 0.0f) ? 1.0f : -1.0f;

	return float4(t, h);
}

template<>
Model_geometry_data<Vertex_attribs_new::p> load_model<Vertex_attribs_new::p>(const char* filename)
{
	Assimp_load_flags flags = default_load_flags;
	return ::load_model<Vertex_attribs_new::p>(filename, flags);
}

template<>
Model_geometry_data<Vertex_attribs_new::p_n> load_model<Vertex_attribs_new::p_n>(const char* filename)
{
	Assimp_load_flags flags = default_load_flags | aiProcess_GenNormals;
	return ::load_model<Vertex_attribs_new::p_n>(filename, flags);
}

template<>
Model_geometry_data<Vertex_attribs_new::p_n_tc> load_model<Vertex_attribs_new::p_n_tc>(const char* filename)
{
	Assimp_load_flags flags = default_load_flags | aiProcess_GenNormals;
	return ::load_model<Vertex_attribs_new::p_n_tc>(filename, flags);
}

template<>
Model_geometry_data<Vertex_attribs_new::p_tc> load_model<Vertex_attribs_new::p_tc>(const char* filename)
{
	Assimp_load_flags flags = default_load_flags;
	return ::load_model<Vertex_attribs_new::p_tc>(filename, flags);
}

template<>
Model_geometry_data<Vertex_attribs_new::p_n_tc_ts> load_model<Vertex_attribs_new::p_n_tc_ts>(const char* filename)
{
	Assimp_load_flags flags = default_load_flags 
		| aiProcess_GenNormals | aiProcess_CalcTangentSpace;
	return ::load_model<Vertex_attribs_new::p_n_tc_ts>(filename, flags);
}

} // namespace data
} // namespace cg
