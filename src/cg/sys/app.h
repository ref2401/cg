#ifndef CG_SYS_APP_H_
#define CG_SYS_APP_H_

#include <cassert>
#include <cstdint>
#include <chrono>
#include <memory>
#include <ratio>
#include <windows.h>
#include "cg/math/math.h"


namespace cg {
namespace sys {

class IGame {
public:

	virtual ~IGame() noexcept = default;


	virtual void on_window_resize() = 0;

	virtual void render(float blend_state) = 0;

	virtual void update(float dt) = 0;
};

class IWindow {
public:

	virtual void show() noexcept = 0;

	// Returns the window's size in pixels.
	virtual uint2 size() const noexcept = 0;

protected:
	~IWindow() noexcept = default;
};

struct Clock_report final {
	using Dur_nano_t = std::chrono::duration<std::chrono::nanoseconds::rep, std::nano>;
	using Dur_msec_t = std::chrono::duration<std::chrono::milliseconds::rep, std::milli>;
	using Dur_sec_t = std::chrono::duration<std::chrono::seconds::rep>;

	Clock_report(const Dur_nano_t& elapsed_time,
		uintmax_t frame_count, uintmax_t update_count) noexcept;


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


class IApplication {
public:

	virtual ~IApplication() noexcept = default;


	virtual Clock_report run(std::unique_ptr<IGame> game) = 0;

	virtual IWindow* window() const noexcept = 0;

protected:
	Clock _clock;
};

class IRender_context {
public:

	virtual ~IRender_context() noexcept = default;


	virtual void swap_color_buffers() noexcept = 0;
};

std::unique_ptr<IApplication> make_win32_application(uint2 wnd_position, uint2 wnd_size);

std::unique_ptr<IRender_context> make_win32_opengl_render_context(HWND hwnd);

} // namespace sys
} // namespace cg

#endif // CG_SYS_APP_H_
