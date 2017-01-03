#include <memory>
#include <sstream>
#include <utility>
#include <windows.h>
#include "cg/base/base.h"
#include "cg/math/math.h"
#include "cg/sys/app.h"
#include "technique/deferred_lighting/deferred_lighting.h"
#include "technique/fur_simulation/fur_simulation_opengl.h"

using cg::sys::Clock_report;

namespace {

std::string get_report_message(const Clock_report& report)
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
	using cg::uint2;
	using cg::rnd::Render_api;
	using cg::sys::Application;
	using cg::sys::Application_desc;

	Application_desc app_desc;
	app_desc.window_position = uint2(90, 50);
	app_desc.viewport_size = uint2(960, 540);

	try {
		Application app(app_desc);
		//auto report = app.run_opengl_example<deferred_lighting::Deferred_lighting>();
		//auto report = app.run_opengl_example<fur_simulation::Fur_simulation_opengl_example>();
		auto report = app.run_opengl_example<fur_simulation::Fur_simulation_opengl_example_2>();
		OutputDebugString(get_report_message(report).c_str());
	}
	catch (std::exception& exc) {
		OutputDebugString("\nException:\n");
		OutputDebugString(cg::get_exception_message(exc).c_str());
		OutputDebugString("----------\n");
	}
	
	return 1;
}
