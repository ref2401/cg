#ifndef LEARN_DX11_BASE_MODEL_H_
#define LEARN_DX11_BASE_MODEL_H_

#include <memory>
#include <unordered_map>
#include <vector>
#include "cg/base/math.h"
#include "cg/data/image.h"
#include "assimp/scene.h"


namespace learn_dx11 {

struct Bone final {
	static constexpr size_t blank_parent_index = std::numeric_limits<size_t>::max();


	explicit Bone(const char* name, size_t parent_index, const float4x4& bp_matrix);


	// Name of the bone specified by an artist.
	std::string name;

	// Index of the parent bone. If value equals to Bone::blank_parent then
	// the bone is considered to be at the top of bone hierarchy.
	size_t parent_index = Bone::blank_parent_index;

	// Bind pose matrix.
	float4x4 bp_matrix;

	// Inverse of the bind pose matrix.
	float4x4 bp_matrix_inv;
};

struct Mesh_draw_params final {
	Mesh_draw_params() noexcept = default;

	cg::data::image_2d diffuse_rgb_image;
	size_t index_count = 0;
	size_t index_offset = 0;
	size_t base_vertex = 0;
};

struct Transform final {
	Transform() noexcept = default;

	Transform(const float3& position, const quat& rotation) noexcept :
		position(position), rotation(rotation)
	{}

	float3 position = float3::zero;
	quat rotation = quat::identity;
};

struct Vertex final {

	Vertex() noexcept = default;

	Vertex(const float3& position, const float3& normal, const float2& tex_coord) :
		position(position), normal(normal), tex_coord(tex_coord)
	{}

	void register_bone(uint32_t bone_index, float bone_weight) noexcept;

	float3 position;
	float3 normal;
	float2 tex_coord;
	uint4 bone_indices;
	float4 bone_weights;
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

	Skeleton_animation(size_t bone_count, const aiAnimation* animation,
		const std::unordered_map<std::string, size_t>& bone_mapping);


	const Frame& operator[](size_t index) const noexcept
	{
		assert(index < _frame_count);
		return _frames[index];
	}


	// Total number of bones of a skeleton.
	size_t bone_count() const noexcept
	{
		return _bone_count;
	}

	// Total number of fames of this animation.
	size_t frame_count() const noexcept
	{
		return _frame_count;
	}

	float frame_milliseconds() const noexcept
	{
		return _frame_milliseconds;
	}

private:

	void init_frames(const aiAnimation* animation,
		const std::unordered_map<std::string, size_t>& bone_mapping);

	void validate_frames();

	size_t _bone_count;
	size_t _frame_count;
	size_t _frames_per_second;
	float _frame_milliseconds;
	// _frames vector has to be sorted by timestamp.
	std::vector<Frame> _frames;
};

class Model_animation final {
public:

	Model_animation() = default;

	Model_animation(size_t bone_count, const aiNode* root_node, const aiAnimation* animation);


	size_t bone_count() const noexcept
	{
		return _bones.size();
	}

	const std::unordered_map<std::string, size_t>& bone_mapping() const noexcept
	{
		return _bone_mapping;
	}

	float total_milliseconds() const noexcept
	{
		return (_skeleton_animation.frame_count() - 1) * _skeleton_animation.frame_milliseconds();
	}

	void update_bone_matrices(float milliseconds, std::vector<float>& bone_matrces_data);

private:

	void init_bones(size_t bone_count, const aiNode* root_node);

	// Bones are stored in level-order: root, its chilred, their children, ect.
	// _bone_mapping stores name of each bone and its index in _bones vector.
	std::vector<Bone> _bones;
	std::unordered_map<std::string, size_t> _bone_mapping;
	Skeleton_animation _skeleton_animation;
};

class Bob_lamp_md5_model final {
public:

	Bob_lamp_md5_model();


	const std::vector<Mesh_draw_params>& mesh_draw_params() noexcept
	{
		return _mesh_draw_params;
	}

	std::unique_ptr<Model_animation> get_model_animation() noexcept
	{
		return std::move(_model_animation);
	}

	size_t vertex_count() const noexcept
	{
		return _vertices.size();
	}

	size_t index_count() const noexcept
	{
		return _indices.size();
	}

	const std::vector<Vertex>& vertices() const noexcept
	{
		return _vertices;
	}

	const std::vector<uint32_t>& indices() const noexcept
	{
		return _indices;
	}

private:

	void init_vertices(const aiScene* scene, const std::unordered_map<std::string, size_t>& bone_mapping);

	void init_images();

	std::vector<Mesh_draw_params> _mesh_draw_params;
	std::vector<Vertex> _vertices;
	std::vector<uint32_t> _indices;
	std::unique_ptr<Model_animation> _model_animation;
};

} // namespace learn_dx11

#endif // LEARN_DX11_BASE_MODEL_H_

