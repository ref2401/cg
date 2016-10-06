#ifndef CG_SYS_APP_H_
#define CG_SYS_APP_H_

#include <cstdint>
#include <chrono>
#include <memory>
#include <ratio>
#include <vector>
#include <windows.h>
#include "cg/math/math.h"
#include "cg/sys/input.h"


namespace cg {
namespace sys {

class Clock final {
public:

	using Time_point_t = std::chrono::high_resolution_clock::time_point;

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

class Window_i {
public:

	virtual void show() noexcept = 0;

	// Returns the window's size in pixels.
	virtual uint2 size() const noexcept = 0;

	// Returns the window's title bar text.
	virtual std::string title() const = 0;

	// Sets the window's title bar text;
	void set_title(const std::string& str) noexcept
	{
		set_title(str.c_str());
	}

	// Sets the window's title bar text;
	virtual void set_title(const char* str) noexcept = 0;

protected:
	~Window_i() noexcept = default;
};

class Application_context_i {
public:

	// Mouse device state.
	virtual const Mouse& mouse() const noexcept = 0;

	// Main window of the current application.
	virtual Window_i& window() noexcept = 0;

protected:

	~Application_context_i() noexcept = default;
};

class Game_i {
public:

	virtual ~Game_i() noexcept = default;


	virtual void on_window_resize() = 0;

	virtual void render(float blend_state) = 0;

	virtual void update(float dt) = 0;
};

// Base interface for all possible system messages(events).
// type filed is used to distinguish between message types.
struct Sys_message final {

	enum class Type {
		none,
		mouse_button
	};

	// type of the system message.
	Type type = Type::none;

	// Indicates which buttons has been pressed. (type == Type::mouse_button).
	Mouse_buttons mouse_buttons = Mouse_buttons::none;
};

class Application : public Application_context_i {
public:

	Application();

	virtual ~Application() noexcept = default;


	const Mouse& mouse() const noexcept override final
	{
		return _mouse;
	}

	void enqueu_mouse_button_message(const Mouse_buttons mb) const;

	void process_sys_messages() noexcept;

	virtual Clock::Clock_report run(std::unique_ptr<Game_i> game) = 0;

protected:
	Clock _clock;
	Mouse _mouse;
	mutable std::vector<Sys_message> _sys_message_queue;
};

class Render_context_i {
public:

	virtual ~Render_context_i() noexcept = default;


	virtual void swap_color_buffers() noexcept = 0;
};


std::unique_ptr<Application> make_win_application(uint2 wnd_position, uint2 wnd_size);

} // namespace sys
} // namespace cg

#endif // CG_SYS_APP_H_
