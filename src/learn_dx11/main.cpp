#include <iostream>
#include "cg/base/base.h"
#include "learn_dx11/base/app.h"
#include "learn_dx11/mesh_rnd/displacement_mapping_example.h"
#include "learn_dx11/mesh_rnd/vertex_skinning_example.h"
#include "learn_dx11/mesh_rnd/static_mesh_example.h"
#include "learn_dx11/tess/terrain_tessellation_example.h"
#include "learn_dx11/tess/compute_complanarity_example.h"
#include "learn_dx11/image_processing/gaussian_filter_example.h"
#include "learn_dx11/image_processing/bilateral_filter_example.h"


#include "cg/data/model.h"

int main(int argc, char* argv[])
{
	cg::uint2 window_position(90, 50);
	cg::uint2 window_size(960, 540);

	try {
		OutputDebugString("----------------\n");
		learn_dx11::Application app(window_position, window_size);
			
		// Uncomment a line to execute the appropriate example.
		//app.run<learn_dx11::mesh_rnd::Static_mesh_example>();
		//app.run<learn_dx11::mesh_rnd::Vertex_skinning_example>();
		//app.run<learn_dx11::mesh_rnd::Displacement_mapping_example>();
		//app.run<learn_dx11::tess::Terrain_tessellation_example>();
		//app.run<learn_dx11::tess::Compute_complanarity_example>();
		//app.run<learn_dx11::image_processing::Gaussian_filter_example>();
		app.run<learn_dx11::image_processing::Bilateral_filter_example>();
	}
	catch(std::exception& exc) {
		OutputDebugString("\nException:\n");
		OutputDebugString(cg::exception_message(exc).c_str());
		OutputDebugString("----------\n");
	}

	return 1;
}