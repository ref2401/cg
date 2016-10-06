#ifndef CG_SYS_WIN_APP_H_
#define CG_SYS_WIN_APP_H_

#include "cg/math/math.h"
#include "cg/opengl/opengl_def.h"
#include "cg/sys/app.h"
#include <windows.h>


namespace cg {
namespace sys {

class Opengl_render_context final : public IRender_context {
public:

	Opengl_render_context(HWND hwnd);

	~Opengl_render_context() noexcept override;

	void swap_color_buffers() noexcept override;

private:

	void load_opengl_11() const;

	void load_opengl_45() const;

	// Opengl_render_context does not own and is not responsible for _hwnd object.
	HWND _hwnd = nullptr;
	HDC _hdc = nullptr;
	HGLRC _hglrc = nullptr;
	HMODULE _opengl_dll = nullptr;
	int _version_major = 0;
	int _version_minor = 0;
};

class Win_window final : public Window_i {
public:

	static constexpr char* wnd_class_name = "cg_window_class";


	Win_window(HINSTANCE hinstance, uint2 wnd_position, uint2 wnd_size);

	~Win_window() noexcept;


	HWND hwnd() const noexcept
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

class Win_app final : public Application_i {
public:

	Win_app(uint2 wnd_position, uint2 wnd_size);

	~Win_app() noexcept override = default;


	// Processes all the system messages that are situated in the message queue at the moment.
	// Returns true if the application has to terminate.
	bool pump_sys_messages() const noexcept;

	Clock_report run(std::unique_ptr<Game_i> game) override;

	Window_i& window() noexcept override
	{
		return _window;
	}

private:
	bool _is_running = false;
	HINSTANCE _hinstance = nullptr;
	Win_window _window;
	Opengl_render_context _rnd_context;
};



} // namespace sys
} // namespace cg

#endif // CG_SYS_WIN_APP_H_
