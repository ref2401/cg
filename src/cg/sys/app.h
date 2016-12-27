#ifndef CG_SYS_APP_H_
#define CG_SYS_APP_H_

#include <cassert>
#include <memory>
#include <type_traits>
#include <vector>
#include "cg/math/math.h"
#include "cg/rnd/rnd.h"
#include "cg/sys/environment.h"
#include <windows.h>


namespace cg {
namespace sys {

struct Application_desc final {
	// Size of a viewport in pixels.
	// A viewport rectangle is an area of a window that is supposed to display rendering results.
	// Usualy a window rectangle comprises title bar, borders and client area (viewport rectangle).
	uint2 viewport_size;

	// Desired location of the top-left corner of a window.
	uint2 window_position;
};

class Sys_message_listener_i {
public:

	virtual void on_keyboard() {}

	virtual void on_mouse_click() {}

	virtual void on_mouse_move() {}

	virtual void on_window_resize() = 0;

protected:

	~Sys_message_listener_i() noexcept = default;
};

class Example : public virtual Sys_message_listener_i {
public:

	explicit Example(const App_context& app_ctx) noexcept : _app_ctx(app_ctx) {}

	Example(const Example&) = delete;

	Example(Example&&) noexcept = delete;

	virtual ~Example() noexcept = default;


	Example& operator=(const Example&) = delete;

	Example& operator=(Example&&) noexcept = delete;


	virtual void render(float interpolation_factor) = 0;

	virtual void update(float dt) = 0;

protected:

	App_context _app_ctx;
};

class Application final {
public:

	explicit Application(const Application_desc& desc);

	~Application() noexcept;


	void enqueue_keyboard_message(Key key, Key_state state);

	void enqueue_mouse_button_message(const Mouse_buttons& mb);

	void enqueue_mouse_enter_message(const Mouse_buttons& mb, const uint2& p);

	void enqueue_mouse_leave_message();

	void enqueue_mouse_move_message(const uint2& p);

	void enqueue_window_resize() noexcept;

	void process_sys_messages(Sys_message_listener_i& listener) noexcept;

	const Mouse& mouse() const noexcept
	{
		return _mouse;
	}

	Window& window() noexcept
	{
		return _window;
	}

	template<typename T>
	Clock_report run_opengl_example();

private:

	// Base interface for all possible system messages(events).
	struct Sys_message final {

		enum class Type {
			none,
			keyboard,
			mouse_button,
			mouse_enter,
			mouse_leave,
			mouse_move
		};

		// type of the system message.
		Type type = Type::none;

		// Indicates which keyboard's button(key) has been pressed/released
		Key key = Key::unknown;
		
		// Indicates the current state of keyboard's button(key).
		Key_state key_state = Key_state::up;

		// Indicates which buttons has been pressed. (type == Type::mouse_button).
		Mouse_buttons mouse_buttons = Mouse_buttons::none;

		// type = Type::mouse_move.	Mouse position within the window's client area. 
		//							The value is relative to the upper-left corner.
		cg::uint2 point = cg::uint2::zero;
	};


	void clear_message_queue() noexcept;

	// Processes all the system messages that are in the message queue at the moment.
	// Returns true if the application has to terminate.
	bool pump_sys_messages() const noexcept;

	// Refreshes state of mouse, keyboard etc.
	void refresh_device_state() noexcept;

	Clock_report run_main_loop(Example& example, cg::rnd::Rhi_context_i& rhi_ctx);


	HINSTANCE _hinstance = nullptr;
	// app context (app environment)
	Clock _clock;
	Keyboard _keyboard;
	Mouse _mouse;
	Window _window;
	// sys messages
	bool _window_resize_message = false;
	std::vector<Sys_message> _sys_message_queue;
};

template<typename T>
Clock_report Application::run_opengl_example()
{
	static_assert(std::is_base_of<Example, T>::value, "T must be derived from cg::sys::Example.");

	App_context app_ctx(_keyboard, _mouse, _window);
	cg::rnd::opengl::Opengl_rhi_context rhi_ctx(_window.hwnd());
	T example(app_ctx);

	return run_main_loop(example, rhi_ctx);
}

} // namespace sys
} // namespace cg

#endif // CG_SYS_APP_H_

