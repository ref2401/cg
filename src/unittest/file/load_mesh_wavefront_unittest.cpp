#include "cg/file/file.h"

#include "unittest/file/file_unittest_common.h"

using cg::file::load_mesh_wavefront;


namespace unittest {

TEST_CLASS(Load_mesh_wavefront_unittest) {
public:

	TEST_METHOD(load_mesh_triangle_positions)
	{
		load_mesh_wavefront(Filenames::wavefront_triangle_p);
	}

};

} // namespace unittest