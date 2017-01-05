#include "technique/fur_simulation/fur_model.h"

#include <cassert>
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

using namespace cg;
using namespace cg::data;


namespace fur_simulation {

// ---- Model_rnd_params -----

Model_rnd_params::Model_rnd_params(float length, float mass, float k, size_t mesh_count) noexcept
	: _length(length), _mass(mass), _k(k)
{
	assert(length >= 0.0f);
	assert(mass >= 0.0f);
	assert(k > 0.0f);
	assert(mesh_count > 0);

	_meshes.reserve(mesh_count);
}

Model_rnd_params::Model_rnd_params(Model_rnd_params&& m) noexcept
	: _meshes(std::move(m._meshes)),
	_length(m._length),
	_mass(m._mass),
	_k(m._k)
{
	m._length = 0.0f;
	m._mass = 0.0f;
	m._k = 1.0f;
}

Model_rnd_params& Model_rnd_params::operator=(Model_rnd_params&& m) noexcept
{
	if (this == &m) return *this;

	_meshes = std::move(m._meshes);
	_length = m._length;
	_mass = m._mass;
	_k = m._k;

	m._length = 0.0f;
	m._mass = 0.0f;
	m._k = 1.0f;

	return *this;
}

void Model_rnd_params::push_back_mesh(size_t vertex_count, size_t base_vertex,
	size_t index_count, size_t index_offset)
{
	_meshes.emplace_back(vertex_count, base_vertex, index_count, index_offset);
}

// ----- Model_geometry_data -----

Model_geometry_data_1::Model_geometry_data_1(Model_geometry_data_1&& m) noexcept
	: _physics_input_buffer(std::move(m._physics_input_buffer)),
	_physics_output_buffer(std::move(m._physics_output_buffer)),
	_render_buffer(std::move(m._render_buffer))
{}

Model_geometry_data_1& Model_geometry_data_1::operator=(Model_geometry_data_1&& m) noexcept
{
	if (this == &m) return *this;

	_physics_input_buffer = std::move(m._physics_input_buffer);
	_physics_output_buffer = std::move(m._physics_output_buffer);
	_render_buffer = std::move(m._render_buffer);

	return *this;
}

void Model_geometry_data_1::push_back_physics_input(const float3& p0, float mass, const float3& p1, float k)
{
	_physics_input_buffer.push_back(p0.x);
	_physics_input_buffer.push_back(p0.y);
	_physics_input_buffer.push_back(p0.z);
	_physics_input_buffer.push_back(mass);
	_physics_input_buffer.push_back(p1.x);
	_physics_input_buffer.push_back(p1.y);
	_physics_input_buffer.push_back(p1.z);
	_physics_input_buffer.push_back(k);
}

void Model_geometry_data_1::push_back_physics_output(const float3& p2)
{
	_physics_output_buffer.push_back(p2.x);
	_physics_output_buffer.push_back(p2.y);
	_physics_output_buffer.push_back(p2.z);
}

void Model_geometry_data_1::push_back_render(const float3& normal, const float2& tex_coord, const float4& tangent_h)
{
	_render_buffer.push_back(normal.x);
	_render_buffer.push_back(normal.y);
	_render_buffer.push_back(normal.z);
	_render_buffer.push_back(tex_coord.x);
	_render_buffer.push_back(tex_coord.y);
	_render_buffer.push_back(tangent_h.x);
	_render_buffer.push_back(tangent_h.y);
	_render_buffer.push_back(tangent_h.z);
	_render_buffer.push_back(tangent_h.w);
}

void Model_geometry_data_1::push_back_indices(uint32_t i0, uint32_t i1, uint32_t i2)
{
	_index_buffer.push_back(i0);
	_index_buffer.push_back(i1);
	_index_buffer.push_back(i2);
}

// ----- funcs -----

std::pair<Model_geometry_data_1, Model_rnd_params> load_fur_model(
	float length, float mass, float k, const char* geometry_filename)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(geometry_filename, aiProcess_ValidateDataStructure
		| aiProcess_JoinIdenticalVertices | aiProcess_GenNormals | aiProcess_CalcTangentSpace);
	assert(scene->mNumMeshes > 0);

	Model_geometry_data_1 geometry_data;
	Model_rnd_params rnd_params(length, mass, k, scene->mNumMeshes);

	size_t base_vertex = 0;
	size_t index_offset = 0;

	// for each mesh
	for (size_t mi = 0; mi < scene->mNumMeshes; ++mi) {
		const aiMesh* mesh = scene->mMeshes[mi];

		rnd_params.push_back_mesh(mesh->mNumVertices, base_vertex, mesh->mNumFaces * 3, index_offset);

		// for each vertex of the mesh
		for (size_t vi = 0; vi < mesh->mNumVertices; ++vi) {
			float3 p0(mesh->mVertices[vi].x, mesh->mVertices[vi].y, mesh->mVertices[vi].z);
			float3 normal = normalize(float3(mesh->mNormals[vi].x, mesh->mNormals[vi].y, mesh->mNormals[vi].z));
			float2 tex_coord(mesh->mTextureCoords[0][vi].x, mesh->mTextureCoords[0][vi].y);
			float3 tangent = normalize(float3(mesh->mTangents[vi].x, mesh->mTangents[vi].y, mesh->mTangents[vi].z));
			float3 bitangent = normalize(float3(mesh->mBitangents[vi].x, mesh->mBitangents[vi].y, mesh->mBitangents[vi].z));
			
			float3 p1(p0 + length * normal);
			float3 p2 = p1;
			float4 tangent_h = compute_tangent_handedness(tangent, bitangent, normal);

			geometry_data.push_back_physics_input(p0, mass, p1, k);
			geometry_data.push_back_physics_output(p2);
			geometry_data.push_back_render(normal, tex_coord, tangent_h);
		}

		for (size_t fi = 0; fi < mesh->mNumFaces; ++fi) {
			const aiFace& face = mesh->mFaces[fi];
			assert(face.mNumIndices == 3);

			geometry_data.push_back_indices(face.mIndices[0], face.mIndices[1], face.mIndices[2]);
		}

		base_vertex += mesh->mNumVertices;
		index_offset = mesh->mNumFaces * 3;
	}

	return std::pair<Model_geometry_data_1, Model_rnd_params>(
		std::move(geometry_data), std::move(rnd_params));
}


// ----- Vertex -----

Vertex::Vertex(const cg::data::Model_geometry_vertex<cg::data::Vertex_attribs::p_n_tc_ts>& vertex,
	float length, const cg::float3& strand_curr_direction) noexcept
	: position(vertex.position),
	strand_rest_position(position + length * vertex.normal),
	strand_curr_position(position + length * strand_curr_direction),
	normal(vertex.normal),
	tex_coord(vertex.tex_coord),
	tangent_h(vertex.tangent_h)
{ }

Vertex::Vertex(const cg::float3& position, const cg::float3& normal, 
	const cg::float2& tex_coord, const cg::float4& tangent_h,
	const cg::float3& strand_curr_direction) noexcept
	: position(position),
	strand_rest_position(position + 0.3f * normal),
	strand_curr_position(position + 0.3f * strand_curr_direction),
	normal(normal),
	tex_coord(tex_coord),
	tangent_h(tangent_h)
{
	assert(cg::is_normalized(normal));
}

// ----- Model_geometry_data -----

Model_geometry_data::Model_geometry_data(const std::vector<Vertex>& vertices)
{
	assert(vertices.size() > 0);

	// pos, rest_pos, curr_pos normal, tex_coord + tangent_h
	constexpr size_t component_count = 3 + 3 + 3 + 3 + 2 + 4;
	_vertex_byte_count = sizeof(float) * component_count;
	_position_byte_offset = 0;
	_normal_byte_offset = sizeof(float) * 3;
	_tex_coord_byte_offset = _normal_byte_offset + sizeof(float) * 3;
	_tangent_h_byte_offset = _tex_coord_byte_offset + sizeof(float) * 2;
	_strand_rest_position_byte_offset = _tangent_h_byte_offset + sizeof(float) * 4;
	_strand_curr_position_byte_offset = _strand_rest_position_byte_offset + sizeof(float) * 3;

	_vertex_data = std::vector<float>(vertices.size() * component_count);

	size_t data_index = 0;
	for (size_t vi = 0; vi < vertices.size(); ++vi) {
		const auto& vertex = vertices[vi];

		// position
		_vertex_data[data_index++] = vertex.position.x;
		_vertex_data[data_index++] = vertex.position.y;
		_vertex_data[data_index++] = vertex.position.z;
		// normal
		_vertex_data[data_index++] = vertex.normal.x;
		_vertex_data[data_index++] = vertex.normal.y;
		_vertex_data[data_index++] = vertex.normal.z;
		// tex_coord
		_vertex_data[data_index++] = vertex.tex_coord.u;
		_vertex_data[data_index++] = vertex.tex_coord.v;
		// tangent_h
		_vertex_data[data_index++] = vertex.tangent_h.x;
		_vertex_data[data_index++] = vertex.tangent_h.y;
		_vertex_data[data_index++] = vertex.tangent_h.z;
		_vertex_data[data_index++] = vertex.tangent_h.w;
		// strand_rest_position
		_vertex_data[data_index++] = vertex.strand_rest_position.x;
		_vertex_data[data_index++] = vertex.strand_rest_position.y;
		_vertex_data[data_index++] = vertex.strand_rest_position.z;
		// strand_curr_position
		_vertex_data[data_index++] = vertex.strand_curr_position.x;
		_vertex_data[data_index++] = vertex.strand_curr_position.y;
		_vertex_data[data_index++] = vertex.strand_curr_position.z;
	}
}

Model_geometry_data::Model_geometry_data(const std::vector<Vertex>& vertices,
	std::initializer_list<uint32_t> indices)
	: Model_geometry_data(vertices)
{
	assert(indices.size() > 0);
	_index_data.insert(_index_data.cend(), indices);
}

Model_geometry_data::Model_geometry_data(const std::vector<Vertex>& vertices, 
	const std::vector<uint32_t>& indices)
	: Model_geometry_data(vertices)
{
	assert(indices.size() > 0);
	_index_data.insert(_index_data.cend(), indices.cbegin(), indices.cend());
}

// ----- Sphere_model -----

Arbitrary_model::Arbitrary_model(float fur_length, const char* filename)
{
	using cg::data::Vertex_attribs;
	using Model_geometry_data_t = cg::data::Model_geometry_data<Vertex_attribs::p_n_tc_ts>;
	using Vertex_data_t = Model_geometry_data_t::Vertex;

	auto model_data = cg::data::load_model<Model_geometry_data_t::Format::attribs>(filename);

	_vertices.reserve(model_data.vertex_count());
	for (size_t vi = 0; vi < model_data.vertex_count(); ++vi) {
		auto* vd_ptr = &model_data.vertex_data()[vi * sizeof(Vertex_data_t)];
		Vertex_data_t vd = *(reinterpret_cast<const Vertex_data_t*>(vd_ptr));
		
		_vertices.emplace_back(vd, fur_length, vd.normal);
	}

	_indices.insert(_indices.cend(), model_data.index_data().cbegin(), model_data.index_data().cend());
}

Model_geometry_data Arbitrary_model::get_geometry_data() const
{
	return Model_geometry_data(_vertices, _indices);
}

// ----- Square_model -----

Square_model::Square_model()
{
	const float3 origin_normal = float3::unit_z;
	const float3 normal = origin_normal;
	//const float3 normal = normalize(origin_normal + float3(-0.4f, 0.0f, 0));

	_vertices.reserve(4);
	
	// left-bottom
	_vertices.emplace_back(float3(-1, -1, 0), normal, float2::zero, float4(1, 0, 0, 1), normal);
		//normalize(normal + float3(0.2f, 0, 0)));
	// right-bottom
	_vertices.emplace_back(float3(1, -1, 0), normal, float2::unit_x, float4(1, 0, 0, 1), normal);
		//normalize(normal + float3(0.2f, 0, 0)));
	// right-top
	_vertices.emplace_back(float3(1, 1, 0), normal, float2::unit_xy, float4(1, 0, 0, 1), normal);
		//normalize(normal + float3(0.2f, 0, 0)));
	// left-bottom
	_vertices.emplace_back(float3(-1, 1, 0), normal, float2::unit_y, float4(1, 0, 0, 1), normal);
		//normalize(normal + float3(0.2f, 0, 0)));

	//_vertices.emplace_back(float3::zero, normal, tex_coord_factor * float2::zero, float4(1, 0, 0, 1), normal);
}

Model_geometry_data Square_model::get_geometry_data() const
{
	return Model_geometry_data(_vertices, { 0, 1, 2, 2, 3, 0 });
}

} // fur_simlation
