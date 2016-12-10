#include "cg/sys/environment.h"


namespace cg {
namespace sys {

// ----- Clock -----

float Clock::get_interpolation_factor() const noexcept
{
	return float(_update_time_accum.count()) / Clock::update_delta_time_dur.count();
}

Clock_report Clock::get_report() const noexcept
{
	Time_point_t curr_time = std::chrono::high_resolution_clock::now();
	auto elapsed_time = std::chrono::duration_cast<Clock_report::Dur_nano_t>(curr_time - _main_loop_begin);
	return Clock_report(elapsed_time, _counter_frames, _counter_updates);
}

bool Clock::has_update_time() const noexcept
{
	return _update_time_accum > Clock::update_delta_time_dur;
}

void Clock::process_loop_iteration() noexcept
{
	++_counter_frames;
	auto curr_time = std::chrono::high_resolution_clock::now();
	auto elapsed_time = std::chrono::duration_cast<Time_point_t::duration>(curr_time - _prev_loop_interation);
	_prev_loop_interation = curr_time;

	// helps to avoid spiral of depth
	if (elapsed_time > Clock::loop_iteration_threshold_dur)
		elapsed_time = Clock::loop_iteration_threshold_dur;

	_update_time_accum += elapsed_time;
}

void Clock::process_update_call() noexcept
{
	++_counter_updates;
	_update_time_accum -= Clock::update_delta_time_dur;
}

void Clock::restart() noexcept
{
	_counter_frames = 0;
	_counter_updates = 0;
	_update_time_accum = Time_point_t::duration::zero();
	_prev_loop_interation = _main_loop_begin = std::chrono::high_resolution_clock::now();
}

// ----- Window -----

Window::Window(HINSTANCE hinstance, WNDPROC window_proc, uint2 position, uint2 viewport_size) noexcept :
	_hinstance(hinstance)
{
	assert(hinstance);
	assert(window_proc);
	assert(greater_than(viewport_size, 0));

	// register the window's class
	WNDCLASSEX wnd_class = {};
	wnd_class.cbSize = sizeof(wnd_class);
	wnd_class.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	wnd_class.lpfnWndProc = window_proc;
	wnd_class.cbClsExtra = 0;
	wnd_class.cbWndExtra = 0;
	wnd_class.hInstance = _hinstance;
	wnd_class.hIcon = nullptr;
	wnd_class.hCursor = LoadCursor(nullptr, IDI_APPLICATION);
	wnd_class.hbrBackground = nullptr;
	wnd_class.lpszMenuName = nullptr;
	wnd_class.lpszClassName = Window::wnd_class_name;

	ATOM reg_res = RegisterClassEx(&wnd_class);
	assert(reg_res != 0);

	// create a window
	RECT wnd_rect;
	wnd_rect.left = position.x;
	wnd_rect.top = position.y;
	wnd_rect.right = position.x + viewport_size.width;
	wnd_rect.bottom = position.y + viewport_size.height;
	AdjustWindowRectEx(&wnd_rect, WS_OVERLAPPEDWINDOW, false, WS_EX_APPWINDOW);

	_hwnd = CreateWindowEx(WS_EX_APPWINDOW, Window::wnd_class_name, "cg project",
		WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		wnd_rect.left, wnd_rect.top, wnd_rect.right - wnd_rect.left, wnd_rect.bottom - wnd_rect.top,
		nullptr, nullptr, _hinstance, nullptr);
	assert(_hwnd);
}

Window::~Window() noexcept
{
	// window
	if (IsWindow(_hwnd))
		DestroyWindow(_hwnd);
	_hwnd = nullptr;

	// window class
	UnregisterClass(Window::wnd_class_name, _hinstance);
}

bool Window::focused() const noexcept
{
	return (_hwnd == GetFocus());
}

void Window::show() noexcept
{
	ShowWindow(_hwnd, SW_SHOW);
	SetForegroundWindow(_hwnd);
	SetFocus(_hwnd);
}

std::string Window::title() const
{
	char buffer[256];
	int actual_length = GetWindowText(_hwnd, buffer, std::extent<decltype(buffer)>::value);

	return std::string(buffer, actual_length);
}

void Window::set_title(const char* str) noexcept
{
	SetWindowTextA(_hwnd, str);
}

uint2 Window::viewport_size() const noexcept
{
	RECT rect;
	GetClientRect(_hwnd, &rect);
	return uint2(rect.right - rect.left, rect.bottom - rect.top);
}

} // namespace sys
} // namespace cg