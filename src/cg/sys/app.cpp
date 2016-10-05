#include "cg/sys/app.h"


namespace cg {
namespace sys {

// ----- Clock_report -----

Clock_report::Clock_report(const Dur_nano_t& elapsed_time,
	uintmax_t frame_count, uintmax_t update_count) noexcept :
	elapsed_time(elapsed_time),
	frame_count(frame_count),
	update_count(update_count)
{}

// ----- Clock -----

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

} // namespace sys
} // namespace cg