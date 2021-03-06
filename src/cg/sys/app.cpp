#include "cg/sys/app.h"

#include "cg/rnd/opengl/opengl.h"


namespace {

using cg::sys::Key;
using cg::sys::Key_state;
using cg::sys::Mouse_buttons;


// Yay! A global. The Application ctor sets its value. The Application dtor resets it to nullptr.
// Only window_proc is supposed to use the global. 
// If any other chunk of the code uses it then your PC will burn. Consider yourself warned.
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

void process_keyboard_message(Key_state state, WPARAM w_param, LPARAM l_param)
{
	static constexpr LPARAM extended_key_flag_mask = 1 << 24;
	bool is_extended_key = (l_param & extended_key_flag_mask) == extended_key_flag_mask;

	Key key = Key::unknown;
	switch (w_param) {
		case VK_MENU: 
			// It can be generated by KEYDOWN and SYSKEYDOWN simultaneously.
			key = (is_extended_key) ? (Key::alt_right) : (Key::alt_left); 
			break;

		case VK_CONTROL:
			key = (is_extended_key) ? (Key::ctrl_right) : (Key::ctrl_left);
			break;
		
		case VK_APPS:		key = Key::apps; break;
		case VK_BACK:		key = Key::backspace; break;
		case VK_CAPITAL:	key = Key::caps_lock; break;
		case 0xBC:			key = Key::comma; break;
		case VK_DELETE:		key = Key::del; break;
		case 0x30:			key = Key::digit_0; break;
		case 0x31:			key = Key::digit_1; break;
		case 0x32:			key = Key::digit_2; break;
		case 0x33:			key = Key::digit_3; break;
		case 0x34:			key = Key::digit_4; break;
		case 0x35:			key = Key::digit_5; break;
		case 0x36:			key = Key::digit_6; break;
		case 0x37:			key = Key::digit_7; break;
		case 0x38:			key = Key::digit_8; break;
		case 0x39:			key = Key::digit_9; break;
		case VK_DOWN:		key = Key::arrow_down; break;
		case VK_END:		key = Key::end; break;
		case VK_RETURN:		key = Key::enter; break;
		case VK_ESCAPE:		key = Key::escape; break;
		case VK_F1:			key = Key::f1; break;
		case VK_F2:			key = Key::f2; break;
		case VK_F3:			key = Key::f3; break;
		case VK_F4:			key = Key::f4; break;
		case VK_F5:			key = Key::f5; break;
		case VK_F6:			key = Key::f6; break;
		case VK_F7:			key = Key::f7; break;
		case VK_F8:			key = Key::f8; break;
		case VK_F9:			key = Key::f9; break;
		case VK_F10:		key = Key::f10; break;
		case VK_F11:		key = Key::f11; break;
		case VK_F12:		key = Key::f12; break;
		case VK_F13:		key = Key::f13; break;
		case VK_F14:		key = Key::f14; break;
		case VK_F15:		key = Key::f15; break;
		case VK_HOME:		key = Key::home; break;
		case VK_INSERT:		key = Key::insert; break;
		case VK_LEFT:		key = Key::arrow_left; break;
		case 0xBD:			key = Key::minus; break;
		case VK_NUMPAD0:	key = Key::numpad_0; break;
		case VK_NUMPAD1:	key = Key::numpad_1; break;
		case VK_NUMPAD2:	key = Key::numpad_2; break;
		case VK_NUMPAD3:	key = Key::numpad_3; break;
		case VK_NUMPAD4:	key = Key::numpad_4; break;
		case VK_NUMPAD5:	key = Key::numpad_5; break;
		case VK_NUMPAD6:	key = Key::numpad_6; break;
		case VK_NUMPAD7:	key = Key::numpad_7; break;
		case VK_NUMPAD8:	key = Key::numpad_8; break;
		case VK_NUMPAD9:	key = Key::numpad_9; break;
		case VK_DECIMAL:	key = Key::numpad_comma; break;
		case VK_DIVIDE:		key = Key::numpad_devide; break;
		case VK_NUMLOCK:	key = Key::num_lock; break;
		case VK_SUBTRACT:	key = Key::numpad_minus; break;
		case VK_MULTIPLY:	key = Key::numpad_multiply; break;
		case VK_ADD:		key = Key::numpad_plus; break;
		case VK_NEXT:		key = Key::page_down; break;
		case VK_PRIOR:		key = Key::page_up; break;
		case VK_PAUSE:		key = Key::pause; break;
		case 0xBE:			key = Key::period; break;
		case 0xBB:			key = Key::plus; break;
		case VK_SNAPSHOT:	key = Key::prtsrc; break;
		case 0xDE:			key = Key::quote; break;
		case 0xC0:			key = Key::quote_back; break;
		case VK_RIGHT:		key = Key::arrow_right; break;
		case VK_SCROLL:		key = Key::scroll_lock; break;
		case 0xBA:			key = Key::semicolon; break;
		case VK_SHIFT:		key = Key::shift_left; break; // BUG: When Right Shift is pressed isExtendedKey equals 0 instead of 1
		case 0xDC:			key = Key::slash; break;
		case 0xBF:			key = Key::slash_back; break;
		case VK_SPACE:		key = Key::space; break;
		case 0xDD:			key = Key::square_bracket_close; break;
		case 0xDB:			key = Key::square_bracket_open; break;
		case VK_TAB:		key = Key::tab; break;
		case VK_UP:			key = Key::arrow_up; break;
		case VK_LWIN:		key = Key::win_left; break;
		case VK_RWIN:		key = Key::win_right; break; // TEST IT:
		case 0x41:			key = Key::a; break;
		case 0x42:			key = Key::b; break;
		case 0x43:			key = Key::c; break;
		case 0x44:			key = Key::d; break;
		case 0x45:			key = Key::e; break;
		case 0x46:			key = Key::f; break;
		case 0x47:			key = Key::g; break;
		case 0x48:			key = Key::h; break;
		case 0x49:			key = Key::i; break;
		case 0x4A:			key = Key::j; break;
		case 0x4B:			key = Key::k; break;
		case 0x4C:			key = Key::l; break;
		case 0x4D:			key = Key::m; break;
		case 0x4E:			key = Key::n; break;
		case 0x4F:			key = Key::o; break;
		case 0x50:			key = Key::p; break;
		case 0x51:			key = Key::q; break;
		case 0x52:			key = Key::r; break;
		case 0x53:			key = Key::s; break;
		case 0x54:			key = Key::t; break;
		case 0x55:			key = Key::u; break;
		case 0x56:			key = Key::v; break;
		case 0x57:			key = Key::w; break;
		case 0x58:			key = Key::x; break;
		case 0x59:			key = Key::y; break;
		case 0x5A:			key = Key::z; break;
	}

	if (key != Key::unknown)
		g_app->enqueue_keyboard_message(key, state);
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

		case WM_KEYDOWN:
		{
			process_keyboard_message(Key_state::down, w_param, l_param);
			return 0;
		}

		case WM_KEYUP:
		{
			process_keyboard_message(Key_state::up, w_param, l_param);
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

void Application::enqueue_keyboard_message(Key key, Key_state state)
{
	Sys_message msg;
	msg.type = Sys_message::Type::keyboard;
	msg.key = key;
	msg.key_state = state;

	_sys_message_queue.push_back(msg);
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

void Application::process_sys_messages(cg::rnd::rhi_context_i& rhi_ctx, Sys_message_listener_i& listener)
{
	if (_sys_message_queue.empty()) return;

	assert(_sys_message_queue.size() < 64);


	if (_window_resize_message) {
		rhi_ctx.resize_viewport(_window.viewport_size());
		listener.on_window_resize();
	}

	for (const auto& msg : _sys_message_queue) {
		switch (msg.type) {
			case Sys_message::Type::keyboard:
			{
				_keyboard.set_key_state(msg.key, msg.key_state);
				listener.on_keyboard();
				break;
			}

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
					_window.viewport_size().y - msg.point.y - 1));

				listener.on_mouse_move();
				break;
			}
		}
	} // for

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
	bool is_out = (cp.x < 0) || (static_cast<uint32_t>(cp.x) >= _window.viewport_size().x)
		|| (cp.y < 0) || (static_cast<uint32_t>(cp.y) >= _window.viewport_size().y);
	_mouse.set_is_out(is_out);

	if (!is_out)
		_mouse.set_position(uint2(cp.x, _window.viewport_size().y - cp.y - 1));
}

Clock_report Application::run_main_loop(cg::rnd::rhi_context_i& rhi_ctx, example& example)
{
	_window.show();
	refresh_device_state();
	rhi_ctx.swap_color_buffers(); //  what for vsync
	_clock.restart();

	while (true) {
		_clock.process_loop_iteration();

		bool terminate = pump_sys_messages();
		if (terminate) break;

		// simulation
		while (_clock.has_update_time()) {
			_clock.process_update_call();
			process_sys_messages(rhi_ctx, example);
			example.update(static_cast<float>(Clock::update_delta_time.count()));
		}

		// rendering
		example.render(_clock.get_interpolation_factor());
		rhi_ctx.swap_color_buffers();
	}

	return _clock.get_report();
}

} // namespace sys
} // namespace cg
