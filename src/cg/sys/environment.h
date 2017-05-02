#ifndef CG_SYS_ENVIRONMENT_H_
#define CG_SYS_ENVIRONMENT_H_

#include <chrono>
#include <ratio>
#include "cg/base/math.h"
#include "cg/sys/input.h"
#include <windows.h>


namespace cg {
namespace sys {

struct Clock_report final {
	using Dur_nano_t = std::chrono::duration<std::chrono::nanoseconds::rep, std::nano>;
	using Dur_msec_t = std::chrono::duration<std::chrono::milliseconds::rep, std::milli>;
	using Dur_sec_t = std::chrono::duration<std::chrono::seconds::rep>;


	Clock_report() noexcept = default;

	Clock_report(const Dur_nano_t& elapsed_time, uintmax_t frame_count, uintmax_t update_count) noexcept 
		: elapsed_time(elapsed_time),
		frame_count(frame_count),
		update_count(update_count)
	{}


	// The number of milliseconds ellapsed since Clock::restart was called.
	Dur_msec_t::rep elapsed_milliseconds() const noexcept
	{
		return std::chrono::duration_cast<Dur_msec_t>(elapsed_time).count();
	}

	// The number of seconds ellapsed since Clock::restart was called.
	Dur_sec_t::rep elapsed_seconds() const noexcept
	{
		return std::chrono::duration_cast<Dur_sec_t>(elapsed_time).count();
	}

	// An average fps estimation.
	double fps() const noexcept
	{
		return double(frame_count) / elapsed_seconds();
	}

	double ups() const noexcept
	{
		return double(update_count) / elapsed_seconds();
	}


	// The number of nanosecond ellapsed since Clock::restart was called.
	Dur_nano_t elapsed_time;

	// The number of processed frames.
	uintmax_t frame_count;

	// The number of update calls.
	uintmax_t update_count;
};

class Clock final {
public:

	using Time_point_t = std::chrono::high_resolution_clock::time_point;

	//  250 milliseconds for 1 update. 4ups is minimum update rate to avoid spiral of death.
	static constexpr auto loop_iteration_threshold = std::chrono::milliseconds(250);

	// 10 millisecons for 1 update - 100ups update rate.
	static constexpr auto update_delta_time = std::chrono::milliseconds(10);

	// Interpolation factor is a value between 0 & 1.
	// It is used to interpolate between previous & current states during rendering.
	float get_interpolation_factor() const noexcept;

	// Returns the main loop execution report, times, counters' values.
	Clock_report get_report() const noexcept;

	// Returns true if the accamulated time is greater than Clock::update_delta_time 
	bool has_update_time() const noexcept;

	// Accumulates elapsed time since the last call process_loop_iteration.
	// Call the method at the begining of the main loop.
	void process_loop_iteration() noexcept;

	// Counts the update call and decrises the accumulated time for updates.
	void process_update_call() noexcept;

	// Resets the clock and starts measuring the main loop execution time.
	void restart() noexcept;

private:
	static constexpr auto loop_iteration_threshold_dur =
		std::chrono::duration_cast<Time_point_t::duration>(Clock::loop_iteration_threshold);

	static constexpr auto update_delta_time_dur =
		std::chrono::duration_cast<Time_point_t::duration>(Clock::update_delta_time);

	Time_point_t _main_loop_begin;
	Time_point_t _prev_loop_interation;
	Time_point_t::duration _update_time_accum;
	uintmax_t _counter_frames;
	uintmax_t _counter_updates;
};

class Window final {
public:

	Window(HINSTANCE hinstance, WNDPROC window_proc, uint2 position, uint2 viewport_size) noexcept;

	Window(const Window&) = delete;

	Window(Window&&) noexcept = delete;

	~Window() noexcept;


	Window& operator=(const Window&) = delete;

	Window& operator=(Window&&) noexcept = delete;


	// Indicates whether the window has input focus.
	bool focused() const noexcept;

	HWND hwnd() const noexcept
	{
		return _hwnd;
	}

	void show() noexcept;

	// Returns the window's title bar text.
	std::string title() const;

	// Sets the window's title bar text
	void set_title(const char* str) noexcept;

	void set_title(const std::string& str) noexcept
	{
		set_title(str.c_str());
	}

	// Returns the window's viewport size in pixels.
	uint2 viewport_size() const noexcept;

private:

	static constexpr char* wnd_class_name = "cg_window_class";

	HINSTANCE _hinstance = nullptr;
	HWND _hwnd = nullptr;
};

struct App_context final {
	App_context(const Keyboard& keyboard, const Mouse& mouse, Window& window) noexcept :
		keyboard(keyboard), mouse(mouse), window(window)
	{}

	
	const Keyboard& keyboard;
	const Mouse& mouse;
	Window& window;
};

} // namespace sys
} // namespace cg

#endif // CG_SYS_ENVIRONMENT_H_

