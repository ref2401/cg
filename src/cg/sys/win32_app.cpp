#include "cg/sys/app.h"

#include <cassert>
#include <memory>
#include "cg/math/math.h"
#include "cg/opengl/opengl_def.h"
#include <windows.h>


namespace {

using cg::uint2;
using namespace cg::sys;


LRESULT CALLBACK window_proc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param);

class Win32_window final : public IWindow {
public:	

	static constexpr char* wnd_class_name = "cg_window_class";


	Win32_window(HINSTANCE hinstance, uint2 wnd_position, uint2 wnd_size);
	
	~Win32_window() noexcept;


	HWND hwdn() const noexcept
	{
		return _hwnd;
	}

	void show() noexcept override;

	uint2 size() const noexcept override;

private:
	HINSTANCE _hinstance = nullptr;
	HWND _hwnd = nullptr;
	HDC _hdc = nullptr;
};

Win32_window::Win32_window(HINSTANCE hinstance, uint2 wnd_position, uint2 wnd_size)
	: _hinstance(hinstance)
{
	assert(_hinstance);

	// register the window's class
	WNDCLASSEX wnd_class = {};
	wnd_class.cbSize = sizeof(wnd_class);
	wnd_class.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	wnd_class.lpfnWndProc = window_proc;
	wnd_class.cbClsExtra = 0;
	wnd_class.cbWndExtra = 0;
	wnd_class.hInstance = hinstance;
	wnd_class.hIcon = nullptr;
	wnd_class.hCursor = LoadCursor(nullptr, IDI_APPLICATION);
	wnd_class.hbrBackground = nullptr;
	wnd_class.lpszMenuName = nullptr;
	wnd_class.lpszClassName = Win32_window::wnd_class_name;

	ATOM reg_res = RegisterClassEx(&wnd_class);
	assert(reg_res != 0);

	// create a window
	RECT wnd_rect;
	wnd_rect.left = wnd_position.x;
	wnd_rect.top = wnd_position.y;
	wnd_rect.right = wnd_position.x + wnd_size.width;
	wnd_rect.bottom = wnd_position.y + wnd_size.height;
	AdjustWindowRectEx(&wnd_rect, WS_OVERLAPPEDWINDOW, false, WS_EX_APPWINDOW);

	_hwnd = CreateWindowEx(WS_EX_APPWINDOW, Win32_window::wnd_class_name, "cg project",
		WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		wnd_rect.left, wnd_rect.top, wnd_rect.right - wnd_rect.left, wnd_rect.bottom - wnd_rect.top,
		nullptr, nullptr, hinstance, nullptr);
	assert(_hwnd);
}

Win32_window::~Win32_window() noexcept
{
	// window
	if (IsWindow(_hwnd))
		DestroyWindow(_hwnd);
	_hwnd = nullptr;

	// window class
	UnregisterClass(Win32_window::wnd_class_name, _hinstance);
}

void Win32_window::show() noexcept 
{
	ShowWindow(_hwnd, SW_SHOW);
	SetForegroundWindow(_hwnd);
	SetFocus(_hwnd);
}

uint2 Win32_window::size() const noexcept 
{
	RECT rect;
	GetClientRect(_hwnd, &rect);
	return uint2(rect.right - rect.left, rect.bottom - rect.top);
}


class Win32_app final : public IApplication {
public:

	Win32_app(uint2 wnd_position, uint2 wnd_size);

	~Win32_app() noexcept override = default;


	// Processes all the system messages that are situated in the message queue at the moment.
	// Returns true if the application has to terminate.
	bool pump_sys_messages();

	Clock_report run(std::unique_ptr<IGame> game) override;

	IWindow* window() const noexcept override
	{
		return _window.get();
	}

private:
	bool _is_running = false;
	HINSTANCE _hinstance = nullptr;
	std::unique_ptr<Win32_window> _window;
	std::unique_ptr<IRender_context> _rnd_context;
};

Win32_app::Win32_app(uint2 wnd_position, uint2 wnd_size)
{
	_hinstance = GetModuleHandle(nullptr);
	_window = std::make_unique<Win32_window>(_hinstance, wnd_position, wnd_size);
	_rnd_context = make_win32_opengl_render_context(_window->hwdn());
}

bool Win32_app::pump_sys_messages()
{
	MSG msg;
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
		if (msg.message == WM_QUIT) 
			return true;

		TranslateMessage(&msg);
		DispatchMessageA(&msg);
	}

	return false;
}

Clock_report Win32_app::run(std::unique_ptr<IGame> game)
{
	assert(!_is_running);
	assert(game);

	_is_running = true;
	_window->show();
	_rnd_context->swap_color_buffers(); //  what for vsync
	_clock.restart();

	while (true) {
		_clock.process_loop_iteration();

		bool terminate = pump_sys_messages();
		if (terminate) break;

		// simulation
		while (_clock.has_update_time()) {
			_clock.process_update_call();
			game->update(Clock::update_delta_time.count());
		}

		// rendering
		game->render(1.f);
		_rnd_context->swap_color_buffers();
	}

	return _clock.get_report();
}


LRESULT CALLBACK window_proc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param)
{
	switch (message) {
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;

		default:
			return DefWindowProc(hwnd, message, w_param, l_param);
	}
}

} // namespace

namespace cg {
namespace sys {


std::unique_ptr<IApplication> make_win32_application(uint2 wnd_position, uint2 wnd_size)
{
	return std::make_unique<Win32_app>(wnd_position, wnd_size);
}


} // sys
} // namespace cg