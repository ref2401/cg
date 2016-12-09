#ifndef LEARN_DX11_BASE_APP_H_
#define LEARN_DX11_BASE_APP_H_

#include <memory>
#include <type_traits>
#include "cg/math/math.h"
#include "learn_dx11/base/dx11.h"
#include <windows.h>


namespace learn_dx11 {

class Example {
public:

	Example(Render_context& rnd_ctx) noexcept : 
		_device(rnd_ctx.device()),
		_debug(rnd_ctx.debug()),
		_device_ctx(rnd_ctx.device_ctx()),
		_pipeline_state(rnd_ctx.pipeline_state())
	{}

	virtual ~Example() noexcept {}


	virtual void on_keypress() {}

	virtual void on_viewport_resize(const cg::uint2& viewport_size) = 0;

	virtual void render() = 0;

	virtual void update(float dt_milliseconds) = 0;

protected:

	void clear_color_buffer(const cg::float4& clear_color);

	void clear_depth_stencil_buffer(float clear_depth);

	// Creates a new rasterizer state object, overwrites _pipeline_state.resterizer_state() 
	// object with the new one and sets the new state as the current of the pipeline.
	void setup_rasterizer_state(const D3D11_RASTERIZER_DESC& desc);

	ID3D11Device* _device;
	ID3D11Debug* _debug;
	ID3D11DeviceContext* _device_ctx;
	Pipeline_state& _pipeline_state;
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


	Com_ptr<ID3D11Debug> get_dx_debug();

	void on_keypress();

	void on_window_resize(const cg::uint2& window_size);

	// The method instantiates the specified example T and runs it.
	template<typename T>
	void run();

private:

	void run_main_loop();

	void show_window() noexcept;


	HINSTANCE _hinstance = nullptr;
	HWND _hwnd = nullptr;
	Render_context _rnd_ctx;
	std::unique_ptr<Example> _example;
};

template<typename T>
void Application::run()
{
	static_assert(std::is_base_of<Example, T>::value, "T must be derived from learn_dx11::Example.");
	assert(!_example);

	show_window();

	_example = std::make_unique<T>(_rnd_ctx);
	run_main_loop();
	_example.reset();
}

} // learn_dx11

#endif // LEARN_DX11_BASE_APP_H_

