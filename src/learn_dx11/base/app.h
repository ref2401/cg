#ifndef LEARN_DX11_BASE_APP_H_
#define LEARN_DX11_BASE_APP_H_

#include "cg/math/math.h"
#include "learn_dx11/base/dx11.h"
#include <windows.h>


namespace learn_dx11 {

class Application final {
public:

	Application(cg::uint2 window_position, cg::uint2 window_size);

	Application(const Application&) = delete;

	Application(Application&&) = delete;

	~Application() noexcept;


	Application& operator=(const Application&) = delete;

	Application& operator=(Application&&) = delete;


	void run();

private:
	cg::uint2 _window_size;
	HINSTANCE _hinstance = nullptr;
	HWND _hwnd = nullptr;
	unique_com<ID3D11Device> _device = nullptr;
	unique_com<ID3D11DeviceContext> _device_ctx = nullptr;
	unique_com<IDXGISwapChain> _swap_chain = nullptr;
};

} // learn_dx11

#endif // LEARN_DX11_BASE_APP_H_
