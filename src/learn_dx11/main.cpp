#include <iostream>
#include "cg/base/base.h"
#include "learn_dx11/base/app.h"
#include "learn_dx11/mesh_rnd/static_mesh_example.h"


int main(int argc, char* argv[])
{
	cg::uint2 wnd_position(90, 50);
	cg::uint2 wnd_size(960, 540);

	try {
		learn_dx11::Application app(wnd_position, wnd_size);
		app.run<learn_dx11::mesh_rnd::Static_mesh_example>();
	}
	catch(std::exception& exc) {
		OutputDebugString("\nException:\n");
		OutputDebugString(cg::get_exception_message(exc).c_str());
		OutputDebugString("----------\n");
	}

	return 1;
}