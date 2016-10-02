#include <memory>
#include <utility>
#include <windows.h>
#include "cg/base/base.h"
#include "cg/file/file.h"
#include "cg/math/math.h"
#include "cg/sys/app.h"
#include "technique/deferred_lighting/deferred_lighting.h"

using cg::uint2;
using cg::sys::make_win32_application;


int main(int argc, char* argv[])
{
	auto imd = cg::file::load_mesh_wavefront(
		"../data/unittest/wavefront_rect_negative_indices_p.obj",
		cg::data::Vertex_attribs::position);

	uint2 wnd_position(90, 50);
	uint2 wnd_size(960, 540);
	auto app = make_win32_application(wnd_position, wnd_size);
	
	try {
		auto game = std::make_unique<deferred_lighting::Deferred_lighting>(app->window()->size());
		app->run(std::move(game));
	}
	catch (std::exception& exc) {
		OutputDebugString("\nException:\n");
		OutputDebugString(cg::get_exception_message(exc).c_str());
		OutputDebugString("----------\n");
	}
	
	return 1;
}