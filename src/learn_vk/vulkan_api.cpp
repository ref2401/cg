#include "learn_vk/vulkan_api.h"

#include <cassert>
#include <windows.h>
#include "cg/base/base.h"

PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr;


namespace {

extern "C" using Extern_func_ptr_t = void(*);

template<typename Func_t>
Func_t load_dll_func(void* dll, const char* func_name)
{
	assert(dll);
	assert(func_name);

	FARPROC func = GetProcAddress(reinterpret_cast<HMODULE>(dll), func_name);
	ENFORCE(func, "Failed to load func: ", func_name);

	return reinterpret_cast<Func_t>(func);
}

template<typename Func_t>
inline Func_t load_vk_instance_func(VkInstance instance, const char* func_name)
{
	assert(func_name);

	PFN_vkVoidFunction func = vkGetInstanceProcAddr(instance, func_name);
	ENFORCE(func, "Failed to load vulkan func: ", func_name);

	return reinterpret_cast<Func_t>(func);
}

} // namespace


namespace learn_vk {

// ----- Vulkan_context -----

Vulkan_context::Vulkan_context()
{
	_vulkan_dll = LoadLibrary("vulkan-1.dll");

	vkGetInstanceProcAddr = load_dll_func<PFN_vkGetInstanceProcAddr>(_vulkan_dll, "vkGetInstanceProcAddr");
}

Vulkan_context::~Vulkan_context() noexcept
{
	FreeLibrary(reinterpret_cast<HMODULE>(_vulkan_dll));
	_vulkan_dll = nullptr;
}

} // namespace learn_vk
