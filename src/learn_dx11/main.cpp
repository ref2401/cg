#include <iostream>
#include "cg/base/base.h"
#include "learn_dx11/base/app.h"
#include "learn_dx11/mesh_rnd/displacement_mapping_example.h"
#include "learn_dx11/mesh_rnd/vertex_skinning_example.h"
#include "learn_dx11/mesh_rnd/static_mesh_example.h"
#include "learn_dx11/tess/terrain_tessellation_example.h"


#include "cg/data/model.h"

int main(int argc, char* argv[])
{
	cg::uint2 wnd_position(90, 50);
	cg::uint2 wnd_size(960, 540);

	try {
		Com_ptr<ID3D11Debug> debug;

		{
			OutputDebugString("----------------\n");
			learn_dx11::Application app(wnd_position, wnd_size);
			debug = app.get_dx_debug();

			// Uncomment any line to execute the appropriate example.
			//app.run<learn_dx11::mesh_rnd::Static_mesh_example>();
			//app.run<learn_dx11::mesh_rnd::Vertex_skinning_example>();
			//app.run<learn_dx11::mesh_rnd::Displacement_mapping_example>();
			app.run<learn_dx11::tess::Terrain_tessellation_example>();
		}

		debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
	}
	catch(std::exception& exc) {
		OutputDebugString("\nException:\n");
		OutputDebugString(cg::get_exception_message(exc).c_str());
		OutputDebugString("----------\n");
	}

	return 1;
}