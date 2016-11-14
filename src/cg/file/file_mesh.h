#ifndef CG_FILE_FILE_MESH_H_
#define CG_FILE_FILE_MESH_H_

#include <utility>
#include "cg/data/mesh.h"
#include "cg/data/vertex.h"
#include "cg/file/file_base.h"


namespace cg {
namespace file {


namespace internal {

// The class is used as in-memory storage of a wavefront (.obj) file.
// The file's content is read to an Wf_mesh_data object and then the object constructs a mesh.
class Wf_mesh_data final {
public:

	Wf_mesh_data(size_t vertex_count);


	void clear() noexcept;

	bool has_normals() const noexcept
	{
		return (normals.size() > 0u);
	}

	bool has_tex_coords() const noexcept
	{
		return (tex_coords.size() > 0u);
	}

	void reserve(size_t vertex_count);


	std::vector<float3> positions;
	std::vector<float3> normals;
	std::vector<float2> tex_coords;
};

// Loads a wavefront file pointed by it and stores the data in mesh_builder.
void load_mesh_wavefront(By_line_iterator it, cg::data::Vertex_attribs attribs, 
	Wf_mesh_data& wf_mesh_data, cg::data::Mesh_builder& mesh_builder);

template<cg::data::Vertex_attribs attribs>
cg::data::Interleaved_mesh_data<attribs>load_mesh_wavefront(By_line_iterator it,
	size_t approx_vertex_count = 0, size_t approx_index_count = 0)
{
	static_assert(attribs != cg::data::Vertex_attribs::none, "Vertex_attribs parameter may not be none.");

	static thread_local Wf_mesh_data wf_mesh_data(256);
	static thread_local cg::data::Mesh_builder mesh_builder(256, 256);

	wf_mesh_data.clear();
	wf_mesh_data.reserve(approx_vertex_count);
	mesh_builder.clear();
	mesh_builder.reserve(approx_vertex_count, approx_index_count);

	internal::load_mesh_wavefront(std::move(it), attribs, wf_mesh_data, mesh_builder);
	return mesh_builder.mesh_data<attribs>();
}

} // namespace internal

// Loads the specified wavefront file contents and pack the data accordion to the interleaved vertex format.
template<cg::data::Vertex_attribs attribs>
cg::data::Interleaved_mesh_data<attribs> load_mesh_wavefront(const std::string& filename, 
	size_t approx_vertex_count = 0, size_t approx_index_count = 0)
{
	By_line_iterator it(filename);
	return internal::load_mesh_wavefront<attribs>(std::move(it), approx_vertex_count, approx_index_count);
}

// Loads the specified wavefront file contents and pack the data accordion to the interleaved vertex format.
template<cg::data::Vertex_attribs attribs>
cg::data::Interleaved_mesh_data<attribs> load_mesh_wavefront(const char * filename,
	size_t approx_vertex_count = 0, size_t approx_index_count = 0)
{
	By_line_iterator it(filename);
	return internal::load_mesh_wavefront<attribs>(std::move(it), approx_vertex_count, approx_index_count);
}


cg::data::Interleaved_mesh_data_old load_mesh_wavefront_old(const std::string& filename,
	cg::data::Vertex_attribs attribs, size_t approx_vertex_count = 0, size_t approx_index_count = 0);

cg::data::Interleaved_mesh_data_old load_mesh_wavefront_old(const char * filename,
	cg::data::Vertex_attribs attribs, size_t approx_vertex_count = 0, size_t approx_index_count = 0);

} // namespace file
} // namespace cg

#endif // CG_FILE_FILE_MESH_H_

