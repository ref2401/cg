#ifndef LEARN_DX11_BASE_APP_H_
#define LEARN_DX11_BASE_APP_H_

#include <type_traits>
#include "cg/math/math.h"
#include "learn_dx11/base/dx11.h"
#include <windows.h>


namespace learn_dx11 {

class Example {
public:

	Example(Render_context& rnd_ctx) noexcept : 
		_rnd_ctx(rnd_ctx)
	{}

	virtual ~Example() noexcept {}


	virtual void render() = 0;

	virtual void update() = 0;

protected:
	Render_context& _rnd_ctx;
};

// Application class represents the entry point of the project.
// An object of the class intialized the main window, DirectX device 
// and runs the specified example.
class Application final {
public:

	Application(cg::uint2 window_position, cg::uint2 window_size);

	Application(const Application&) = delete;

	Application(Application&&) = delete;

	~Application() noexcept;


	Application& operator=(const Application&) = delete;

	Application& operator=(Application&&) = delete;

	// The method instantiates the specified example T and runs it.
	template<typename T>
	void run();

private:

	void run_main_loop(Example& example);

	void show_window() noexcept;


	HINSTANCE _hinstance = nullptr;
	HWND _hwnd = nullptr;
	Render_context _rnd_ctx;
};

template<typename T>
void Application::run()
{
	static_assert(std::is_base_of<Example, T>::value, "T must be derived from learn_dx11::Example.");

	show_window();
	T example(_rnd_ctx);

	run_main_loop(example);
}

} // learn_dx11

#endif // LEARN_DX11_BASE_APP_H_

