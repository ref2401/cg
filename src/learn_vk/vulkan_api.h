#ifndef LEARN_VK_VULKAN_API_H_
#define LEARN_VK_VULKAN_API_H_


#define VK_NO_PROTOTYPES
#include "vulkan/vulkan.h"

extern "C"
{
	extern PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr;
	extern PFN_vkCreateInstance vkCreateInstance;
}


namespace learn_vk {

class Vulkan_context final {
public:

	Vulkan_context();

	Vulkan_context(const Vulkan_context&) = delete;

	Vulkan_context(Vulkan_context&&) = delete;

	~Vulkan_context() noexcept;


	Vulkan_context& operator=(const Vulkan_context&) = delete;

	Vulkan_context& operator=(Vulkan_context&&) = delete;

private:

	void* _vulkan_dll = nullptr;
};

} // namespace learn_vk

#endif // LEARN_VK_VULKAN_API_H_
