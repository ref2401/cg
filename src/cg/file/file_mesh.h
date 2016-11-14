#ifndef CG_FILE_FILE_MESH_H_
#define CG_FILE_FILE_MESH_H_

#include "cg/data/mesh.h"
#include "cg/file/file_base.h"

namespace cg {
namespace file {

cg::data::Interleaved_mesh_data_old load_mesh_wavefront_old(const std::string& filename,
	cg::data::Vertex_attribs attribs, size_t approx_vertex_count = 0, size_t approx_index_cont = 0);

cg::data::Interleaved_mesh_data_old load_mesh_wavefront_old(const char * filename,
	cg::data::Vertex_attribs attribs, size_t approx_vertex_count = 0, size_t approx_index_cont = 0);

//template<cg::data::Vertex_attribs attribs>
//cg::data::Interleaved_mesh_data<attribs> load_mesh_wavefront(const std::string& filename, 
//	size_t approx_vertex_count = 0, size_t approx_index_cont = 0);
//
//template<cg::data::Vertex_attribs attribs>
//cg::data::Interleaved_mesh_data<attribs> load_mesh_wavefront(const char * filename,
//	size_t approx_vertex_count = 0, size_t approx_index_cont = 0);

} // namespace file
} // namespace cg

#endif // CG_FILE_FILE_MESH_H_

