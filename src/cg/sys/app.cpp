#include "cg/sys/app.h"

#include "cg/rnd/opengl/opengl.h"


namespace {

using cg::uint2;
using cg::sys::Mouse_buttons;


// Yay! A global. The Application ctor sets its value. The Application dtor resets it to nullptr.
// Only window_proc is supposed to use the global. 
// If any other chunk of code uses it then your PC will burn. Consider yourself warned.
cg::sys::Application* g_app = nullptr;

Mouse_buttons get_mouse_buttons(WPARAM w_param) noexcept
{
	auto buttons = Mouse_buttons::none;

	int btn_state = GET_KEYSTATE_WPARAM(w_param);
	if ((btn_state & MK_LBUTTON) == MK_LBUTTON) buttons |= Mouse_buttons::left;
	if ((btn_state & MK_MBUTTON) == MK_MBUTTON) buttons |= Mouse_buttons::middle;
	if ((btn_state & MK_RBUTTON) == MK_RBUTTON) buttons |= Mouse_buttons::right;

	return buttons;
}

uint2 get_point(LPARAM l_param) noexcept
{
	return uint2(LOWORD(l_param), HIWORD(l_param));
}

LRESULT CALLBACK window_proc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param)
{
	if (g_app == nullptr)
		return DefWindowProc(hwnd, message, w_param, l_param);

	switch (message) {
		default:
			return DefWindowProc(hwnd, message, w_param, l_param);

		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		{
			g_app->enqueue_mouse_button_message(get_mouse_buttons(w_param));
			return 0;
		}

		case WM_MOUSEMOVE:
		{
			if (!g_app->window().focused())
				return DefWindowProc(hwnd, message, w_param, l_param);

			uint2 p = get_point(l_param);

			if (g_app->mouse().is_out()) {
				g_app->enqueue_mouse_enter_message(get_mouse_buttons(w_param), p);
			}
			else {
				g_app->enqueue_mouse_move_message(p);
			}

			TRACKMOUSEEVENT tme{ sizeof(TRACKMOUSEEVENT), TME_LEAVE, g_app->window().hwnd(), 0 };
			TrackMouseEvent(&tme);
			return 0;
		}

		case WM_MOUSELEAVE:
		{
			g_app->enqueue_mouse_leave_message();
			return 0;
		}


		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}

		case WM_SIZE:
		{
			g_app->enqueue_window_resize();
			return 0;
		}
	}
}

} // namespace


namespace cg {
namespace sys {

// ----- Application -----

Application::Application(const Application_desc& desc) :
	_hinstance(GetModuleHandle(nullptr)),
	_window(_hinstance, window_proc, desc.window_position, desc.viewport_size)
{

	if (desc.rhi_type == cg::rnd::Render_api::dx11) {
		assert(false);
	}
	else if (desc.rhi_type == cg::rnd::Render_api::opengl) {
		_rhi_ctx = std::make_unique<cg::rnd::opengl::Opengl_rhi_context>(_window.hwnd());
	}

	g_app = this;
}

Application::~Application() noexcept
{
	g_app = nullptr;
}

void Application::clear_message_queue() noexcept
{
	_sys_message_queue.clear();
	_window_resize_message = false;
}

void Application::enqueue_mouse_button_message(const Mouse_buttons& mb)
{
	Sys_message msg;
	msg.type = Sys_message::Type::mouse_button;
	msg.mouse_buttons = mb;

	_sys_message_queue.push_back(msg);
}

void Application::enqueue_mouse_enter_message(const Mouse_buttons& mb, const uint2& p)
{
	Sys_message msg;
	msg.type = Sys_message::Type::mouse_enter;
	msg.mouse_buttons = mb;
	msg.point = p;
	_sys_message_queue.push_back(msg);
}

void Application::enqueue_mouse_move_message(const uint2& p)
{
	Sys_message msg;
	msg.type = Sys_message::Type::mouse_move;
	msg.point = p;

	_sys_message_queue.push_back(msg);
}

void Application::enqueue_mouse_leave_message()
{
	Sys_message msg;
	msg.type = Sys_message::Type::mouse_leave;
	_sys_message_queue.push_back(msg);
}

void Application::enqueue_window_resize() noexcept
{
	_window_resize_message = true;
}

void Application::process_sys_messages(Sys_message_listener_i& listener) noexcept
{
	if (_sys_message_queue.empty()) return;

	assert(_sys_message_queue.size() < 64);


	if (_window_resize_message)
		listener.on_window_resize();

	for (const auto& msg : _sys_message_queue) {
		switch (msg.type) {
			case Sys_message::Type::mouse_button:
			{
				_mouse.set_buttons(msg.mouse_buttons);
				listener.on_mouse_click();
				break;
			}

			case Sys_message::Type::mouse_enter:
			{
				_mouse.set_is_out(false);
				_mouse.set_buttons(msg.mouse_buttons);
				_mouse.set_position(msg.point);
				break;
			}

			case Sys_message::Type::mouse_leave:
			{
				_mouse.set_is_out(true);
				break;
			}

			case Sys_message::Type::mouse_move:
			{
				_mouse.set_position(uint2(
					msg.point.x,
					_window.viewport_size().height - msg.point.y - 1));

				listener.on_mouse_move();
				break;
			}
		}
	} // for

	  //std::ostringstream title_builder;
	  //if (_mouse.is_out()) title_builder << "out |";
	  //else title_builder << " in |";

	  //if (_mouse.left_down()) title_builder << " 1";
	  //else title_builder << " 0";
	  //if (_mouse.middle_down()) title_builder << "1";
	  //else title_builder << "0";
	  //if (_mouse.right_down()) title_builder << "1 |";
	  //else title_builder << "0 |";

	  //title_builder << _mouse.position();
	  //window().set_title(title_builder.str());

	clear_message_queue();
}

bool Application::pump_sys_messages() const noexcept
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

void Application::refresh_device_state() noexcept
{
	// mouse buttons
	auto mb = Mouse_buttons::none;
	if (HIWORD(GetKeyState(VK_LBUTTON)) == 1) mb |= Mouse_buttons::left;
	if (HIWORD(GetKeyState(VK_MBUTTON)) == 1) mb |= Mouse_buttons::middle;
	if (HIWORD(GetKeyState(VK_RBUTTON)) == 1) mb |= Mouse_buttons::right;

	// mouse position & is_out
	POINT cp;
	GetCursorPos(&cp);
	ScreenToClient(_window.hwnd(), &cp);
	bool is_out = (cp.x < 0) || (static_cast<uint32_t>(cp.x) >= _window.viewport_size().width)
		|| (cp.y < 0) || (static_cast<uint32_t>(cp.y) >= _window.viewport_size().height);
	_mouse.set_is_out(is_out);

	if (!is_out)
		_mouse.set_position(uint2(cp.x, _window.viewport_size().height - cp.y - 1));
}

Clock_report Application::run_main_loop(Example& example)
{
	_window.show();
	refresh_device_state();
	_rhi_ctx->swap_color_buffers(); //  what for vsync
	_clock.restart();

	while (true) {
		_clock.process_loop_iteration();

		bool terminate = pump_sys_messages();
		if (terminate) break;

		// simulation
		while (_clock.has_update_time()) {
			_clock.process_update_call();
			process_sys_messages(example);
			example.update(static_cast<float>(Clock::update_delta_time.count()));
		}

		// rendering
		example.render(_clock.get_interpolation_factor());
		_rhi_ctx->swap_color_buffers();
	}

	return _clock.get_report();
}

} // namespace sys
} // namespace cg
