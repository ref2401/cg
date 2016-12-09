#include "cg/sys/app_old.h"

#include <sstream>


namespace cg {
namespace sys {

// ----- Clock_report -----

Clock::Clock_report::Clock_report(const Dur_nano_t& elapsed_time,
	uintmax_t frame_count, uintmax_t update_count) noexcept :
	elapsed_time(elapsed_time),
	frame_count(frame_count),
	update_count(update_count)
{}

// ----- Clock -----

float Clock::get_interpolation_factor() const noexcept
{
	return float(_update_time_accum.count()) / Clock::update_delta_time_dur.count();
}

Clock::Clock_report Clock::get_report() const noexcept
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

// ----- Game -----

Game::Game(Application_context_i& ctx) noexcept :
	_ctx(ctx)
{}

// ----- Application -----

Application::Application()
{
	_sys_message_queue.reserve(64);
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

void Application::enqueue_mouse_enter_message(const Mouse_buttons& mb, const cg::uint2& p)
{
	Sys_message msg;
	msg.type = Sys_message::Type::mouse_enter;
	msg.mouse_buttons = mb;
	msg.point = p;
	_sys_message_queue.push_back(msg);
}

void Application::enqueue_mouse_move_message(const cg::uint2& p)
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

void Application::enqueue_window_resize(const cg::uint2& size)
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
					window().size().height - msg.point.y - 1));

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


} // namespace sys
} // namespace cg