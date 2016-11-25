#ifndef LEARN_DX11_BASE_MODEL_H_
#define LEARN_DX11_BASE_MODEL_H_

#include <unordered_map>
#include <vector>
#include "cg/math/math.h"
#include "assimp/scene.h"


namespace learn_dx11 {

struct Bone final {
	static constexpr size_t blank_parent_index = std::numeric_limits<size_t>::max();

	explicit Bone(const char* name, size_t parent_index, const cg::mat4& bp_matrix);


	// Name of the bone specified by an artist.
	std::string name;

	// Index of the parent bone. If value equals to Bone::blank_parent then
	// the bone is considered to be at the top of bone hierarchy.
	size_t parent_index = Bone::blank_parent_index;

	// Bind pose matrix.
	cg::mat4 bp_matrix;

	// Inverse of the bind pose matrix.
	cg::mat4 bp_matrix_inv;
};

struct Transform final {
	Transform() noexcept = default;

	Transform(const cg::float3& position, const cg::quat& rotation) noexcept :
		position(position), rotation(rotation)
	{}

	cg::float3 position = cg::float3::zero;
	cg::quat rotation = cg::quat::identity;
};

class Skeleton_animation final {
public:

	static constexpr float invalid_timestamp = -1.0f;

	// Each frame contains trasnformations for all the bones of a skeleton.
	struct Frame {
		float timestamp = Skeleton_animation::invalid_timestamp;
		std::vector<Transform> transforms;
	};


	Skeleton_animation() noexcept = default;

	Skeleton_animation(size_t bone_count, size_t timestamp_count, const aiAnimation* animation,
		const std::unordered_map<std::string, size_t>& bone_mapping);


	// Total number of bones of a skeleton.
	size_t bone_count() const noexcept
	{
		return _bone_count;
	}

	// Total number of fames of this animation.
	size_t timestamp_count() const noexcept
	{
		return _timestamp_count;
	}

private:

	void init_frames(const aiAnimation* animation,
		const std::unordered_map<std::string, size_t>& bone_mapping);

	void validate_frames();

	size_t _bone_count;
	size_t _timestamp_count;
	// _frames vector has to be sorted by timestamp.
	std::vector<Frame> _frames;
};

class Bob_lamp_md5_model final {
public:

	Bob_lamp_md5_model();


	const std::vector<cg::float3>& bone_positions() const noexcept
	{
		return _bone_positions;
	}

private:

	std::unordered_map<std::string, size_t> init_bones(size_t bone_count, const aiNode* root_node);

	// Bones are stored in level-order: root, its chilred, their children, ect.
	// For any i [0, _bones.size()) _bones[i] & _bone_position[i] represent the same bone.
	std::vector<Bone> _bones;
	std::vector<cg::float3> _bone_positions;
	Skeleton_animation _skeleton_animation;
};

} // namespace learn_dx11

#endif // LEARN_DX11_BASE_MODEL_H_

