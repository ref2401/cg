#include <memory>
#include <utility>
#include <windows.h>
#include "cg/base/base.h"
#include "cg/math/math.h"
#include "cg/sys/app.h"
#include "deferred_lighting/deferred_lighting.h"

using cg::uint2;
using cg::sys::make_win32_application;


int main(int argc, char* argv[])
{
	uint2 wnd_position(90, 50);
	uint2 wnd_size(960, 540);
	auto app = make_win32_application(wnd_position, wnd_size);
	
	try {
		auto game = std::make_unique<deferred_lighting::Deferred_lighting>();
		app->run(std::move(game));
	}
	catch (std::exception& exc) {
		OutputDebugString("\nException:\n");
		OutputDebugString(cg::get_exception_message(exc).c_str());
		OutputDebugString("----------\n");
	}
	
	return 1;
}