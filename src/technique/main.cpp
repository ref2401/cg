#include <memory>
#include <sstream>
#include <utility>
#include <windows.h>
#include "cg/base/base.h"
#include "cg/file/file.h"
#include "cg/math/math.h"
#include "cg/sys/app.h"
#include "technique/deferred_lighting/deferred_lighting.h"

using cg::uint2;
using cg::sys::Clock;
using cg::sys::make_win_application;


namespace {

std::string get_report_message(const Clock::Clock_report& report)
{
	std::ostringstream out;
	out << std::endl << "----- Exec report ----- " << std::endl;
	out << "\ttotal time: " << report.elapsed_seconds() << " seconds." << std::endl;
	out << "\tfps(avg): " << report.fps() << ", total frames: " << report.frame_count << std::endl;
	out << "\tups(avg): " << report.ups() << ", total updates: " << report.update_count << std::endl;
	out << "----- -----" << std::endl << std::endl;

	return out.str();
}

} // namespace


int main(int argc, char* argv[])
{
	uint2 wnd_position(90, 50);
	uint2 wnd_size(960, 540);
	auto app = make_win_application(wnd_position, wnd_size);
	try {
		auto game = std::make_unique<deferred_lighting::Deferred_lighting>(*app.get());
		auto report = app->run(std::move(game));
		OutputDebugString(get_report_message(report).c_str());
	}
	catch (std::exception& exc) {
		OutputDebugString("\nException:\n");
		OutputDebugString(cg::get_exception_message(exc).c_str());
		OutputDebugString("----------\n");
	}
	
	return 1;
}
