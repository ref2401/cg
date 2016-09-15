#include <iostream>
#include <utility>
#include "cg/file/file.h"


int main(int argc, char* argv[])
{
	cg::file::load_mesh_wavefront("../data/unittest/wavefront_triangle_pntc.obj");

	std::cout << "Hello world ";
	return 1;
}