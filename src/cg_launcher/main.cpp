#include <iostream>
#include <utility>
#include "cg/file/file.h"


int main(int argc, char* argv[])
{
	auto r = cg::file::load_mesh_wavefront("../data/unittest/wavefront_triangle_pntc.obj", 
		cg::data::Vertex_attribs::mesh_tangent_h);

	std::cout << "Hello world ";
	return 1;
}