#include "learn_dx11/base/app.h"

#include <cassert>

using cg::uint2;
using cg::greater_than;

namespace {

constexpr char* wnd_class_name = "learn_dx11_window_class";

LRESULT CALLBACK window_proc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param);


void dispose_window(HWND& hwnd) noexcept
{
	if (!hwnd) return;

	DestroyWindow(hwnd);
	hwnd = nullptr;
}

// Initialized a device that represents the display adapter and a swap chain used for rendering.
// After the function returns device, device_context & swap_chain point to appropriate directx objects.
void initialize_directx_11(const uint2& window_size, HWND hwnd,
	ID3D11Device** device, ID3D11DeviceContext** device_context, IDXGISwapChain** swap_chain) noexcept
{
	assert(greater_than(window_size, 0));
	assert(hwnd);

	HRESULT hr;

	DXGI_SWAP_CHAIN_DESC swap_chain_desc = {};
	swap_chain_desc.BufferCount = 2;
	swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_chain_desc.BufferDesc.Width = window_size.width;
	swap_chain_desc.BufferDesc.Height = window_size.height;
	swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	swap_chain_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swap_chain_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swap_chain_desc.BufferDesc.RefreshRate.Numerator = 60;
	swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;

	swap_chain_desc.SampleDesc.Count = 1;
	swap_chain_desc.SampleDesc.Quality = 0;
	swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swap_chain_desc.Windowed = true;
	swap_chain_desc.OutputWindow = hwnd;

	D3D_FEATURE_LEVEL expected_feature_level = D3D_FEATURE_LEVEL_11_0;
	D3D_FEATURE_LEVEL actual_feature_level;
	hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 
		D3D11_CREATE_DEVICE_DEBUG, &expected_feature_level, 1, D3D11_SDK_VERSION, &swap_chain_desc,
		swap_chain, device, &actual_feature_level, device_context);

	assert(hr == S_OK);
	assert(actual_feature_level == expected_feature_level);
}

HWND make_window(HINSTANCE hinstance, const uint2& position, const uint2& size)
{
	assert(hinstance);

	// register the window's class
	WNDCLASSEX wnd_class = {};
	wnd_class.cbSize = sizeof(wnd_class);
	wnd_class.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	wnd_class.lpfnWndProc = window_proc;
	wnd_class.cbClsExtra = 0;
	wnd_class.cbWndExtra = 0;
	wnd_class.hInstance = hinstance;
	wnd_class.hIcon = nullptr;
	wnd_class.hCursor = LoadCursor(nullptr, IDI_APPLICATION);
	wnd_class.hbrBackground = nullptr;
	wnd_class.lpszMenuName = nullptr;
	wnd_class.lpszClassName = wnd_class_name;

	ATOM reg_res = RegisterClassEx(&wnd_class);
	assert(reg_res != 0);

	// create a window
	RECT wnd_rect;
	wnd_rect.left = position.x;
	wnd_rect.top = position.y;
	wnd_rect.right = position.x + size.width;
	wnd_rect.bottom = position.y + size.height;
	AdjustWindowRectEx(&wnd_rect, WS_OVERLAPPEDWINDOW, false, WS_EX_APPWINDOW);

	HWND hwnd = CreateWindowEx(WS_EX_APPWINDOW, wnd_class_name, "Learn DirectX 11",
		WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		wnd_rect.left, wnd_rect.top, wnd_rect.right - wnd_rect.left, wnd_rect.bottom - wnd_rect.top,
		nullptr, nullptr, hinstance, nullptr);
	assert(hwnd);

	return hwnd;
}

// Processes all the system messages that are situated in the message queue at the moment.
// Returns true if the application has to terminate.
bool pump_sys_messages() noexcept 
{
	MSG msg;
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
		if (msg.message == WM_QUIT)
			return true;

		TranslateMessage(&msg);
		DispatchMessageA(&msg);
	}

	return false;
}

LRESULT CALLBACK window_proc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param)
{
	//if (g_win_app == nullptr)
	//	return DefWindowProc(hwnd, message, w_param, l_param);

	switch (message) {
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;

		default:
			return DefWindowProc(hwnd, message, w_param, l_param);
	}
}

} // namespace


namespace learn_dx11 {

// ----- Application -----

Application::Application(cg::uint2 window_position, cg::uint2 window_size) :
	_window_size(window_size),
	_hinstance(GetModuleHandle(nullptr))
{
	assert(greater_than(window_size, 0));

	_hwnd = make_window(_hinstance, window_position, window_size);
	
	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* device_ctx = nullptr;
	IDXGISwapChain* swap_chain = nullptr;
	initialize_directx_11(_window_size, _hwnd, &device, &device_ctx, &swap_chain);
	_device.reset(device);
	_device_ctx.reset(device_ctx);
	_swap_chain.reset(swap_chain);
}

Application::~Application() noexcept
{
	dispose_window(_hwnd);
}

void Application::run()
{
	ShowWindow(_hwnd, SW_SHOW);
	SetForegroundWindow(_hwnd);
	SetFocus(_hwnd);

	while (true) {
		bool terminate = pump_sys_messages();
		if (terminate) break;
	}
}

} // namespace learn_dx11
