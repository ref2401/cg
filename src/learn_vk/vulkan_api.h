#ifndef LEARN_VK_VULKAN_API_H_
#define LEARN_VK_VULKAN_API_H_


#include <ostream>
#define VK_NO_PROTOTYPES
#include "vulkan/vulkan.h"


extern "C" {
	extern PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr;
	extern PFN_vkCreateInstance vkCreateInstance;
	extern PFN_vkEnumerateInstanceExtensionProperties vkEnumerateInstanceExtensionProperties;
	extern PFN_vkEnumerateInstanceLayerProperties vkEnumerateInstanceLayerProperties;
	extern PFN_vkEnumeratePhysicalDevices vkEnumeratePhysicalDevices;
	extern PFN_vkGetPhysicalDeviceProperties vkGetPhysicalDeviceProperties;
	extern PFN_vkGetPhysicalDeviceFeatures vkGetPhysicalDeviceFeatures;
	extern PFN_vkGetPhysicalDeviceQueueFamilyProperties vkGetPhysicalDeviceQueueFamilyProperties;
	extern PFN_vkCreateDevice vkCreateDevice;
	extern PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr;
	extern PFN_vkDestroyInstance vkDestroyInstance;
}


namespace learn_vk {

class Vulkan_context final {
public:

	Vulkan_context(const char* app_name);

	Vulkan_context(const Vulkan_context&) = delete;

	Vulkan_context(Vulkan_context&&) = delete;

	~Vulkan_context() noexcept;


	Vulkan_context& operator=(const Vulkan_context&) = delete;

	Vulkan_context& operator=(Vulkan_context&&) = delete;

private:

	void init_vk_instance(const char* app_name);

	void print_physical_devices();

	void* _vulkan_dll = nullptr;
	VkInstance _vk_instance = nullptr;
};


template<typename T, size_t count>
inline std::ostream& array_to_stream(const T(&arr)[count], std::ostream& o)
{
	for (size_t i = 0; i < count; ++i)
		o << arr[i] << " ";

	return o;
}

std::ostream& physical_device_limits_to_stream(const VkPhysicalDeviceLimits& limits, std::ostream& o);

std::ostream& physical_device_properties(const VkPhysicalDeviceProperties& props, std::ostream& o);

std::ostream& physical_device_sparce_properties(const VkPhysicalDeviceSparseProperties& props, std::ostream& o);

std::ostream& physical_device_type_to_stream(const VkPhysicalDeviceType& type, std::ostream& o);

std::ostream& sample_count_flags_to_stream(const VkSampleCountFlags& flags, std::ostream& o);

} // namespace learn_vk

#endif // LEARN_VK_VULKAN_API_H_
