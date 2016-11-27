#include "learn_dx11/base/bob_lamp_model.h"

#include <cassert>
#include <cstdint>
#include <algorithm>
#include <chrono>
#include <queue>
#include <string>
#include <type_traits>
#include "cg/base/base.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"

using cg::float2;
using cg::float3;
using cg::mat4;
using cg::quat;
using cg::approx_equal;
using cg::inverse;
using cg::to_array_column_major_order;


namespace {

inline mat4 make_cg_matrix(const aiMatrix4x4& m) noexcept
{
	return mat4(
		m.a1, m.a2, m.a3, m.a4,
		m.b1, m.b2, m.b3, m.b4,
		m.c1, m.c2, m.c3, m.c4,
		m.d1, m.d2, m.d3, m.d4);
}

inline quat make_cg_quat(const aiQuaternion& q)
{
	return quat(q.x, q.y, q.z, q.w);
}

inline float3 make_cg_vector(const aiVector3D& v)
{
	return float3(v.x, v.y, v.z);
}

inline float2 make_cg_vector(const aiVector2D& v)
{
	return float2(v.x, v.y);
}

} // namespace


namespace learn_dx11 {

// ----- Bone -----

Bone::Bone(const char* name, size_t parent_index, const cg::mat4& bp_matrix) :
	name(name),
	parent_index(parent_index),
	bp_matrix(bp_matrix),
	bp_matrix_inv(inverse(bp_matrix))
{}

// ----- Skeleton_animation -----

Skeleton_animation::Skeleton_animation(size_t bone_count, const aiAnimation* animation, 
	const std::unordered_map<std::string, size_t>& bone_mapping) :
	_bone_count(bone_count),
	_frame_count(size_t(animation->mDuration) + 1),
	_frames_per_second(size_t(animation->mTicksPerSecond))
{
	assert(bone_count > 0);
	assert(_frame_count > 0);
	assert(animation);
	
	_frame_milliseconds = 1000.0f / _frames_per_second;
	_frames = std::vector<Frame>(_frame_count);
	
	for (auto& frame : _frames) {
		frame.transforms = std::vector<Transform>(bone_count);
	}

	init_frames(animation, bone_mapping);
	validate_frames();
}

void Skeleton_animation::init_frames(const aiAnimation* animation,
	const std::unordered_map<std::string, size_t>& bone_mapping)
{
	const auto mapping_end_it = bone_mapping.cend();

	for (size_t i = 0; i < _bone_count; ++i) {
		// all aiNodeAnim must have the same number of PositionKeys & RotationKeys
		aiNodeAnim* node_anim = animation->mChannels[i];
		assert(node_anim->mNumPositionKeys == _frame_count);
		assert(node_anim->mNumPositionKeys == node_anim->mNumRotationKeys);

		// find bone index by its name
		auto it = bone_mapping.find(node_anim->mNodeName.C_Str());
		assert(it != mapping_end_it);

		size_t bone_index = it->second;
		for (size_t i_ts = 0; i_ts < _frame_count; ++i_ts) {
			auto& frame = _frames[i_ts];
			aiVectorKey pos_key = node_anim->mPositionKeys[i_ts];
			aiQuatKey rot_key = node_anim->mRotationKeys[i_ts];

			// if timestamp of the frame has already been set then
			// pos_key.mTime & rot_key.mTime must be equal to it.
			// otherwise check whether pos_key.mTime & rot_key.mTime are equal.
			if (frame.timestamp > Skeleton_animation::invalid_timestamp) {
				assert(approx_equal(float(pos_key.mTime), frame.timestamp));
				assert(approx_equal(float(rot_key.mTime), frame.timestamp));
			}
			else {
				assert(approx_equal(pos_key.mTime, rot_key.mTime));
				frame.timestamp = float(pos_key.mTime);
			}

			frame.transforms[bone_index].position = make_cg_vector(pos_key.mValue);
			frame.transforms[bone_index].rotation = make_cg_quat(rot_key.mValue);
		}
	}
}

void Skeleton_animation::validate_frames()
{
	auto it = std::is_sorted_until(_frames.cbegin(), _frames.cend(), 
		[](const Frame& l, const Frame& r) { return l.timestamp < r.timestamp; });
	
	if (it != _frames.cend()) {
		auto index = it - _frames.cbegin();
		auto msg = EXCEPTION_MSG("Frames sequence is not sorted by timestamps. Index: ", std::to_string(index));
	}
}

// ----- Model_animation -----

Model_animation::Model_animation(size_t bone_count, const aiNode* root_node, const aiAnimation* animation) :
	_curr_bone_matrices_data(bone_count * 16, 0)
{
	assert(bone_count > 0);
	assert(root_node);

	init_bones(bone_count, root_node);
	_skeleton_animation = Skeleton_animation(bone_count, animation, _bone_mapping);
}

void Model_animation::init_bones(size_t bone_count, const aiNode* root_node)
{
	struct Node_info final {
		Node_info(const aiNode* node, size_t index) noexcept :
		node(node), index(index)
		{}

		const aiNode* node;
		size_t index;
	};

	_bones.reserve(bone_count);
	_bone_mapping.reserve(bone_count);

	_bones.emplace_back(root_node->mName.C_Str(),
		Bone::blank_parent_index, make_cg_matrix(root_node->mTransformation));

	std::queue<Node_info> queue;
	queue.emplace(root_node, 0);
	_bone_mapping[_bones.back().name] = 0;

	while (!queue.empty()) {
		Node_info info = queue.front();
		queue.pop();

		for (size_t i = 0; i < info.node->mNumChildren; ++i) {
			aiNode* child = info.node->mChildren[i];
			size_t child_index = _bones.size();
			mat4 tm = _bones[info.index].bp_matrix * make_cg_matrix(child->mTransformation);

			_bones.emplace_back(child->mName.C_Str(), info.index, tm);

			queue.emplace(child, child_index);
			_bone_mapping[_bones.back().name] = child_index;
		}
	}
}

void Model_animation::update_bone_matrices(float milliseconds)
{
	if (milliseconds < 0.0f) {
		float* ptr = _curr_bone_matrices_data.data();
		for (size_t i = 0; i < bone_count(); ++i) {
			mat4 bm = mat4::identity;
			to_array_column_major_order(bm, ptr);

			ptr += 16;
		}

		return;
	}

	size_t frame_index = size_t(milliseconds / _skeleton_animation.frame_milliseconds());
	size_t next_frame_index = frame_index + 1;
	if (next_frame_index >= _skeleton_animation.frame_count()) {
		assert(next_frame_index < _skeleton_animation.frame_count());
	}

	float interpolatio_factor = (milliseconds - frame_index * _skeleton_animation.frame_milliseconds()) 
		/ _skeleton_animation.frame_milliseconds();

	float* ptr = _curr_bone_matrices_data.data();
	const auto& frame0 = _skeleton_animation[frame_index];
	const auto& frame1 = _skeleton_animation[next_frame_index];
	std::vector<Transform> curr_transforms(bone_count() + 1);

	for (size_t i = 0; i < bone_count(); ++i) {
		const Transform& parent_transform = curr_transforms[_bones[i].parent_index + 1];
		Transform& curr_transform = curr_transforms[i + 1];

		float3 pos = lerp(frame0.transforms[i].position, frame1.transforms[i].position, interpolatio_factor);
		quat rot = slerp(frame0.transforms[i].rotation, frame1.transforms[i].rotation, interpolatio_factor);

		curr_transform.position = cg::rotate(parent_transform.rotation, pos) + parent_transform.position;
		curr_transform.rotation = cg::normalize(parent_transform.rotation * rot);
		mat4 tm = cg::tr_matrix(curr_transform.position, curr_transform.rotation) 
			* _bones[i].bp_matrix_inv;
		to_array_column_major_order(tm, ptr);
		ptr += 16;
	}
}

// ----- Bob_lamp_md5_model -----

Bob_lamp_md5_model::Bob_lamp_md5_model()
{
	Assimp::Importer importer;
	std::underlying_type_t <aiPostProcessSteps> flags = aiProcess_GenNormals
		| aiProcess_JoinIdenticalVertices | aiProcess_ValidateDataStructure;

	const aiScene* scene = importer.ReadFile("../data/models/bob_lamp/bob_lamp.md5mesh", flags);
	assert(scene);
	assert(scene->mNumMeshes > 0);
	assert(scene->mNumAnimations > 0);

	const size_t bone_count = scene->mAnimations[0]->mNumChannels;

	_model_animation = std::make_unique<Model_animation>(bone_count, 
		scene->mRootNode->mChildren[1]->mChildren[0], scene->mAnimations[0]);

	init_geometry(scene, _model_animation->bone_mapping());
}

void Bob_lamp_md5_model::init_geometry(const aiScene* scene, 
	const std::unordered_map<std::string, size_t>& bone_mapping)
{
	size_t vertex_count = 0;
	size_t index_count = 0;
	for (size_t mi = 0; mi < scene->mNumMeshes; ++mi) {
		vertex_count += scene->mMeshes[mi]->mNumVertices;
		index_count += scene->mMeshes[mi]->mNumFaces * 3;
	}

	_draw_params.reserve(scene->mNumMeshes);
	_vertices.reserve(vertex_count);
	_indices.reserve(index_count);

	size_t base_vertex = 0;
	size_t index_offset = 0;
	for (size_t mi = 0; mi < scene->mNumMeshes; ++mi) {
		const aiMesh* mesh = scene->mMeshes[mi];
		assert(mesh->HasPositions());
		assert(mesh->HasNormals());
		assert(mesh->HasTextureCoords(0));

		_draw_params.emplace_back(mesh->mNumFaces * 3, index_offset, base_vertex);
		index_offset += mesh->mNumFaces * 3;
		base_vertex += mesh->mNumVertices;

		for (size_t vi = 0; vi < mesh->mNumVertices; ++vi) {
			_vertices.emplace_back(make_cg_vector(mesh->mVertices[vi]),
				make_cg_vector(mesh->mNormals[vi]), make_cg_vector(mesh->mTextureCoords[0][vi]).uv());
		}

		for (size_t fi = 0; fi < mesh->mNumFaces; ++fi) {
			const aiFace& face = mesh->mFaces[fi];
			assert(face.mNumIndices == 3);
			_indices.push_back(face.mIndices[0]);
			_indices.push_back(face.mIndices[1]);
			_indices.push_back(face.mIndices[2]);
		}
	}
}

} // namespace learn_dx11
