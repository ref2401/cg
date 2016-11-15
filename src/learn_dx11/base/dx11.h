#ifndef LEARN_DX11_BASE_DX11_H_
#define LEARN_DX11_BASE_DX11_H_

#include <memory>
#include <d3d11.h>
#include <d3dcommon.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <dxgi.h>

using namespace DirectX;


namespace learn_dx11 {

struct Com_dispose_func final {
	void operator()(IUnknown* com_obj) noexcept
	{
		if (com_obj == nullptr) return;

		auto c = com_obj->Release();
		com_obj = nullptr;
	}
};

template<typename T>
using unique_com = std::unique_ptr<T, Com_dispose_func>;

inline void dispose_com(IUnknown* com_obj)
{
	if (com_obj == nullptr) return;

	auto c = com_obj->Release();
	com_obj = nullptr;
}

} // namespace learn_dx11

#endif // LEARN_DX11_BASE_DX11_H_
