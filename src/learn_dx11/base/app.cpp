#include "learn_dx11/base/app.h"

#include <cassert>
#include <chrono>

using cg::uint2;
using cg::greater_than;

namespace {

constexpr char* wnd_class_name = "learn_dx11_window_class";

learn_dx11::Application* g_app;

LRESULT CALLBACK window_proc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param);


void dispose_window(HWND& hwnd) noexcept
{
	if (!hwnd) return;

	DestroyWindow(hwnd);
	hwnd = nullptr;
}

HWND make_window(HINSTANCE hinstance, const uint2& position, const uint2& size)
{
	assert(hinstance);

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
	wnd_class.lpszClassName = wnd_class_name;

	ATOM reg_res = RegisterClassEx(&wnd_class);
	assert(reg_res != 0);

	// create a window
	RECT wnd_rect;
	wnd_rect.left = position.x;
	wnd_rect.top = position.y;
	wnd_rect.right = position.x + size.width;
	wnd_rect.bottom = position.y + size.height;
	AdjustWindowRectEx(&wnd_rect, WS_OVERLAPPEDWINDOW, false, WS_EX_APPWINDOW);

	HWND hwnd = CreateWindowEx(WS_EX_APPWINDOW, wnd_class_name, "Learn DirectX 11",
		WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		wnd_rect.left, wnd_rect.top, wnd_rect.right - wnd_rect.left, wnd_rect.bottom - wnd_rect.top,
		nullptr, nullptr, hinstance, nullptr);
	assert(hwnd);

	return hwnd;
}

// Processes all the system messages that are situated in the message queue at the moment.
// Returns true if the application has to terminate.
bool pump_sys_messages() noexcept 
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

LRESULT CALLBACK window_proc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param)
{
	if (g_app == nullptr)
		return DefWindowProc(hwnd, message, w_param, l_param);

	switch (message) {
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;

		case WM_KEYDOWN:
			g_app->on_keypress();
			return 0;

		case WM_SIZE:
		{
			g_app->on_window_resize(uint2(LOWORD(l_param), HIWORD(l_param)));
			return 0;
		}

		default:
			return DefWindowProc(hwnd, message, w_param, l_param);
	}
}

} // namespace


namespace learn_dx11 {

// ----- Example -----

void Example::clear_color_buffer(const cg::float4& clear_color)
{
	_device_ctx->ClearRenderTargetView(_pipeline_state.render_target_view(), clear_color.data);
}

void Example::clear_depth_stencil_buffer(float clear_depth)
{
	_device_ctx->ClearDepthStencilView(_pipeline_state.depth_stencil_view(),
		D3D11_CLEAR_DEPTH, clear_depth, 1);
}

// ----- Application -----

Application::Application(cg::uint2 window_position, cg::uint2 window_size) :
	_hinstance(GetModuleHandle(nullptr)),
	_hwnd(make_window(_hinstance, window_position, window_size)),
	_rnd_ctx(_hwnd, window_size)
{
	g_app = this;
}

Application::~Application() noexcept
{
	g_app = nullptr;
	dispose_window(_hwnd);
	_hinstance = nullptr;
}

Com_ptr<ID3D11Debug> Application::get_dx_debug()
{
	Com_ptr<ID3D11Debug> debug(_rnd_ctx.debug());
	debug->AddRef();

	return debug;
}

void Application::on_keypress()
{
	if (_example)
		_example->on_keypress();
}

void Application::on_window_resize(const uint2& window_size)
{
	assert(greater_than(window_size, 0));

	if (window_size == _rnd_ctx.pipeline_state().viewport_size()) return;

	_rnd_ctx.resize_viewport(window_size);
	if (_example)
		_example->on_viewport_resize(window_size);
}

void Application::run_main_loop()
{
	using Time_point_t = std::chrono::high_resolution_clock::time_point;
	using Duration_milliseconds_t = std::chrono::duration<std::chrono::milliseconds::rep, std::milli>;

	Time_point_t prev_frame_time;
	Time_point_t curr_frame_time = std::chrono::high_resolution_clock::now();

	while (true) {
		bool terminate = pump_sys_messages();
		if (terminate) break;

		prev_frame_time = curr_frame_time;
		curr_frame_time = std::chrono::high_resolution_clock::now();
		Duration_milliseconds_t dt = std::chrono::duration_cast<Duration_milliseconds_t>(
			curr_frame_time - prev_frame_time);
		
		_example->update(float(dt.count()));
		_example->render();
		_rnd_ctx.swap_color_buffers();
	}
}

void Application::show_window() noexcept
{
	ShowWindow(_hwnd, SW_SHOW);
	SetForegroundWindow(_hwnd);
	SetFocus(_hwnd);
}

} // namespace learn_dx11
