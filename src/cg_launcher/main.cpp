#include <memory>
#include <utility>
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

	std::unique_ptr<cg::sys::IGame> game = std::make_unique<deferred_lighting::Deferred_lighting>();
	app->run(std::move(game));

	return 1;
}