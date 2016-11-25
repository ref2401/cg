#include "learn_dx11/base/model.h"

#include <cassert>
#include <cstdint>
#include <algorithm>
#include <queue>
#include <string>
#include <type_traits>
#include "cg/base/base.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"

using cg::float3;
using cg::mat4;
using cg::quat;
using cg::approx_equal;
using cg::inverse;


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

Skeleton_animation::Skeleton_animation(size_t bone_count, size_t timestamp_count,
	const aiAnimation* animation, const std::unordered_map<std::string, size_t>& bone_mapping) :
	_bone_count(bone_count), _timestamp_count(timestamp_count)
{
	assert(bone_count > 0);
	assert(timestamp_count > 0);
	assert(animation);
	
	_frames = std::vector<Frame>(timestamp_count);
	
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
		assert(node_anim->mNumPositionKeys == _timestamp_count);
		assert(node_anim->mNumPositionKeys == node_anim->mNumRotationKeys);

		// find bone index by its name
		auto it = bone_mapping.find(node_anim->mNodeName.C_Str());
		assert(it != mapping_end_it);

		size_t bone_index = it->second;
		for (size_t i_ts = 0; i_ts < _timestamp_count; ++i_ts) {
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

// ----- Bob_lamp_md5_model -----

Bob_lamp_md5_model::Bob_lamp_md5_model()
{
	Assimp::Importer importer;
	std::underlying_type_t <aiPostProcessSteps> flags = aiProcess_GenNormals 
		| aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices 
		| aiProcess_ValidateDataStructure;

	const aiScene* scene = importer.ReadFile("../data/models/bob_lamp/boblampclean.md5mesh", flags);
	assert(scene);
	assert(scene->mNumMeshes > 0);
	assert(scene->mNumAnimations > 0);

	const size_t bone_count = scene->mAnimations[0]->mNumChannels;
	const size_t timestamp_count = scene->mAnimations[0]->mChannels[0]->mNumPositionKeys;
	
	const auto bone_mapping = init_bones(bone_count, scene->mRootNode->mChildren[1]->mChildren[0]);
	_skeleton_animation = Skeleton_animation(bone_count, timestamp_count, 
		scene->mAnimations[0], bone_mapping);
}

std::unordered_map<std::string, size_t> Bob_lamp_md5_model::init_bones(size_t bone_count, const aiNode* root_node)
{
	struct Node_info final {
		Node_info(const aiNode* node, size_t index) noexcept :
		node(node), index(index)
		{}

		const aiNode* node;
		size_t index;
	};

	assert(bone_count > 0);
	assert(root_node);

	std::unordered_map<std::string, size_t> bone_mapping;
	_bones.reserve(bone_count);
	_bone_positions.reserve(bone_count);
	bone_mapping.reserve(bone_count);

	_bones.emplace_back(root_node->mName.C_Str(),
		Bone::blank_parent_index, make_cg_matrix(root_node->mTransformation));

	std::queue<Node_info> queue;
	queue.emplace(root_node, 0);
	bone_mapping[_bones.back().name] = 0;

	while (!queue.empty()) {
		Node_info info = queue.front();
		queue.pop();

		for (size_t i = 0; i < info.node->mNumChildren; ++i) {
			aiNode* child = info.node->mChildren[i];
			size_t child_index = _bones.size();
			mat4 tm = _bones[info.index].bp_matrix * make_cg_matrix(child->mTransformation);

			_bones.emplace_back(child->mName.C_Str(), info.index, tm);
			_bone_positions.emplace_back(tm.m03, tm.m13, tm.m23);

			queue.emplace(child, child_index);
			bone_mapping[_bones.back().name] = child_index;
		}
	}

	return bone_mapping;
}


} // namespace learn_dx11
