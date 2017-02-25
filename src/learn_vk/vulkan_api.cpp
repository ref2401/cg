#include "learn_vk/vulkan_api.h"

#include <cassert>
#include <iostream>
#include <vector>
#include <windows.h>
#include "cg/base/base.h"

PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr;
PFN_vkCreateInstance vkCreateInstance;
PFN_vkEnumerateInstanceExtensionProperties vkEnumerateInstanceExtensionProperties;
PFN_vkEnumerateInstanceLayerProperties vkEnumerateInstanceLayerProperties;
PFN_vkEnumeratePhysicalDevices vkEnumeratePhysicalDevices;
PFN_vkGetPhysicalDeviceProperties vkGetPhysicalDeviceProperties;
PFN_vkGetPhysicalDeviceFeatures vkGetPhysicalDeviceFeatures;
PFN_vkGetPhysicalDeviceQueueFamilyProperties vkGetPhysicalDeviceQueueFamilyProperties;
PFN_vkCreateDevice vkCreateDevice;
PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr;
PFN_vkDestroyInstance vkDestroyInstance;


namespace {

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

Vulkan_context::Vulkan_context(const char* app_name)
{
	_vulkan_dll = LoadLibrary("vulkan-1.dll");

	vkGetInstanceProcAddr = load_dll_func<PFN_vkGetInstanceProcAddr>(_vulkan_dll, "vkGetInstanceProcAddr");
	vkCreateInstance = load_vk_instance_func<PFN_vkCreateInstance>(nullptr, "vkCreateInstance");
	vkEnumerateInstanceExtensionProperties = load_vk_instance_func<PFN_vkEnumerateInstanceExtensionProperties>(nullptr, "vkEnumerateInstanceExtensionProperties");
	vkEnumerateInstanceLayerProperties = load_vk_instance_func<PFN_vkEnumerateInstanceLayerProperties>(nullptr, "vkEnumerateInstanceLayerProperties");

	init_vk_instance(app_name);

	vkEnumeratePhysicalDevices = load_vk_instance_func<PFN_vkEnumeratePhysicalDevices>(_vk_instance, "vkEnumeratePhysicalDevices");
	vkGetPhysicalDeviceProperties = load_vk_instance_func<PFN_vkGetPhysicalDeviceProperties>(_vk_instance, "vkGetPhysicalDeviceProperties");
	vkGetPhysicalDeviceFeatures = load_vk_instance_func<PFN_vkGetPhysicalDeviceFeatures>(_vk_instance, "vkGetPhysicalDeviceFeatures");
	vkGetPhysicalDeviceQueueFamilyProperties = load_vk_instance_func<PFN_vkGetPhysicalDeviceQueueFamilyProperties>(_vk_instance, "vkGetPhysicalDeviceQueueFamilyProperties");
	vkCreateDevice = load_vk_instance_func<PFN_vkCreateDevice>(_vk_instance, "vkCreateDevice");
	vkGetDeviceProcAddr = load_vk_instance_func<PFN_vkGetDeviceProcAddr>(_vk_instance, "vkGetDeviceProcAddr");
	vkDestroyInstance = load_vk_instance_func<PFN_vkDestroyInstance>(_vk_instance, "vkDestroyInstance");

	print_physical_devices();
}

Vulkan_context::~Vulkan_context() noexcept
{
	FreeLibrary(reinterpret_cast<HMODULE>(_vulkan_dll));
	_vulkan_dll = nullptr;
}

void Vulkan_context::init_vk_instance(const char* app_name)
{
	VkApplicationInfo app_info = {};
	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.pApplicationName = app_name;
	app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	app_info.pEngineName = "learn_vk engine";
	app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	app_info.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo instance_info = {};
	instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instance_info.pApplicationInfo = &app_info;

	VkResult res = vkCreateInstance(&instance_info, nullptr, &_vk_instance);
	assert(res == VK_SUCCESS);
}

void Vulkan_context::print_physical_devices()
{
	uint32_t physical_device_count = 0;
	VkResult res = vkEnumeratePhysicalDevices(_vk_instance, &physical_device_count, nullptr);
	assert(res == VK_SUCCESS);

	std::cout << "----- Physical devices -----" << std::endl;
	std::cout << "physical_device_count: " << physical_device_count << std::endl;

	std::vector<VkPhysicalDevice> physical_devices(physical_device_count);
	res = vkEnumeratePhysicalDevices(_vk_instance, &physical_device_count, physical_devices.data());
	for (uint32_t i = 0; i < physical_device_count; ++i) {
		VkPhysicalDeviceProperties props;
		vkGetPhysicalDeviceProperties(physical_devices[i], &props);

		std::cout << "device #" << i << std::endl;
		physical_device_properties(props, std::cout);
		std::cout << std::endl;
	}
}

// ----- funcs -----

std::ostream& physical_device_limits_to_stream(const VkPhysicalDeviceLimits& limits, std::ostream& o)
{
	o << "maxImageDimension1D: " << limits.maxImageDimension1D << std::endl
		<< "maxImageDimension2D: " << limits.maxImageDimension2D << std::endl
		<< "maxImageDimension3D: " << limits.maxImageDimension3D << std::endl
		<< "maxImageDimensionCube: " << limits.maxImageDimensionCube << std::endl
		<< "maxImageArrayLayers: " << limits.maxImageArrayLayers << std::endl
		<< "maxTexelBufferElements: " << limits.maxTexelBufferElements << std::endl
		<< "maxUniformBufferRange: " << limits.maxUniformBufferRange << std::endl
		<< "maxStorageBufferRange: " << limits.maxStorageBufferRange << std::endl
		<< "maxPushConstantsSize: " << limits.maxPushConstantsSize << std::endl
		<< "maxMemoryAllocationCount: " << limits.maxMemoryAllocationCount << std::endl
		<< "maxSamplerAllocationCount: " << limits.maxSamplerAllocationCount << std::endl
		<< "bufferImageGranularity: " << limits.bufferImageGranularity << std::endl
		<< "sparseAddressSpaceSize: " << limits.sparseAddressSpaceSize << std::endl
		<< "maxBoundDescriptorSets: " << limits.maxBoundDescriptorSets << std::endl
		<< "maxPerStageDescriptorSamplers: " << limits.maxPerStageDescriptorSamplers << std::endl
		<< "maxPerStageDescriptorUniformBuffers: " << limits.maxPerStageDescriptorUniformBuffers << std::endl
		<< "maxPerStageDescriptorStorageBuffers: " << limits.maxPerStageDescriptorStorageBuffers << std::endl
		<< "maxPerStageDescriptorSampledImages: " << limits.maxPerStageDescriptorSampledImages << std::endl
		<< "maxPerStageDescriptorStorageImages: " << limits.maxPerStageDescriptorStorageImages << std::endl
		<< "maxPerStageDescriptorInputAttachments: " << limits.maxPerStageDescriptorInputAttachments << std::endl
		<< "maxPerStageResources: " << limits.maxPerStageResources << std::endl
		<< "maxDescriptorSetSamplers: " << limits.maxDescriptorSetSamplers << std::endl
		<< "maxDescriptorSetUniformBuffers: " << limits.maxDescriptorSetUniformBuffers << std::endl
		<< "maxDescriptorSetUniformBuffersDynamic: " << limits.maxDescriptorSetUniformBuffersDynamic << std::endl
		<< "maxDescriptorSetStorageBuffers: " << limits.maxDescriptorSetStorageBuffers << std::endl
		<< "maxDescriptorSetStorageBuffersDynamic: " << limits.maxDescriptorSetStorageBuffersDynamic << std::endl
		<< "maxDescriptorSetSampledImages: " << limits.maxDescriptorSetSampledImages << std::endl
		<< "maxDescriptorSetStorageImages: " << limits.maxDescriptorSetStorageImages << std::endl
		<< "maxDescriptorSetInputAttachments: " << limits.maxDescriptorSetInputAttachments << std::endl
		<< "maxVertexInputAttributes: " << limits.maxVertexInputAttributes << std::endl
		<< "maxVertexInputBindings: " << limits.maxVertexInputBindings << std::endl
		<< "maxVertexInputAttributeOffset: " << limits.maxVertexInputAttributeOffset << std::endl
		<< "maxVertexInputBindingStride: " << limits.maxVertexInputBindingStride << std::endl
		<< "maxVertexOutputComponents: " << limits.maxVertexOutputComponents << std::endl
		<< "maxTessellationGenerationLevel: " << limits.maxTessellationGenerationLevel << std::endl
		<< "maxTessellationPatchSize: " << limits.maxTessellationPatchSize << std::endl
		<< "maxTessellationControlPerVertexInputComponents: " << limits.maxTessellationControlPerVertexInputComponents << std::endl
		<< "maxTessellationControlPerVertexOutputComponents: " << limits.maxTessellationControlPerVertexOutputComponents << std::endl
		<< "maxTessellationControlPerPatchOutputComponents: " << limits.maxTessellationControlPerPatchOutputComponents << std::endl
		<< "maxTessellationControlTotalOutputComponents: " << limits.maxTessellationControlTotalOutputComponents << std::endl
		<< "maxTessellationEvaluationInputComponents: " << limits.maxTessellationEvaluationInputComponents << std::endl
		<< "maxTessellationEvaluationOutputComponents: " << limits.maxTessellationEvaluationOutputComponents << std::endl
		<< "maxGeometryShaderInvocations: " << limits.maxGeometryShaderInvocations << std::endl
		<< "maxGeometryInputComponents: " << limits.maxGeometryInputComponents << std::endl
		<< "maxGeometryOutputComponents: " << limits.maxGeometryOutputComponents << std::endl
		<< "maxGeometryOutputVertices: " << limits.maxGeometryOutputVertices << std::endl
		<< "maxGeometryTotalOutputComponents: " << limits.maxGeometryTotalOutputComponents << std::endl
		<< "maxFragmentInputComponents: " << limits.maxFragmentInputComponents << std::endl
		<< "maxFragmentOutputAttachments: " << limits.maxFragmentOutputAttachments << std::endl
		<< "maxFragmentDualSrcAttachments: " << limits.maxFragmentDualSrcAttachments << std::endl
		<< "maxFragmentCombinedOutputResources: " << limits.maxFragmentCombinedOutputResources << std::endl
		<< "maxComputeSharedMemorySize: " << limits.maxComputeSharedMemorySize << std::endl
		<< "maxComputeWorkGroupCount: "; array_to_stream(limits.maxComputeWorkGroupCount, o) << std::endl
		<< "maxComputeWorkGroupInvocations: " << limits.maxComputeWorkGroupInvocations << std::endl
		<< "maxComputeWorkGroupSize: "; array_to_stream(limits.maxComputeWorkGroupSize, o) << std::endl
		<< "subPixelPrecisionBits: " << limits.subPixelPrecisionBits << std::endl
		<< "subTexelPrecisionBits: " << limits.subTexelPrecisionBits << std::endl
		<< "mipmapPrecisionBits: " << limits.mipmapPrecisionBits << std::endl
		<< "maxDrawIndexedIndexValue: " << limits.maxDrawIndexedIndexValue << std::endl
		<< "maxDrawIndirectCount: " << limits.maxDrawIndirectCount << std::endl
		<< "maxSamplerLodBias: " << limits.maxSamplerLodBias << std::endl
		<< "maxSamplerAnisotropy: " << limits.maxSamplerAnisotropy << std::endl
		<< "maxViewports: " << limits.maxViewports << std::endl
		<< "maxViewportDimensions: "; array_to_stream(limits.maxViewportDimensions, o) << std::endl
		<< "viewportBoundsRange: "; array_to_stream(limits.viewportBoundsRange, o) << std::endl
		<< "viewportSubPixelBits: " << limits.viewportSubPixelBits << std::endl
		<< "minMemoryMapAlignment: " << limits.minMemoryMapAlignment << std::endl
		<< "minTexelBufferOffsetAlignment: " << limits.minTexelBufferOffsetAlignment << std::endl
		<< "minUniformBufferOffsetAlignment: " << limits.minUniformBufferOffsetAlignment << std::endl
		<< "minStorageBufferOffsetAlignment: " << limits.minStorageBufferOffsetAlignment << std::endl
		<< "minTexelOffset: " << limits.minTexelOffset << std::endl
		<< "maxTexelOffset: " << limits.maxTexelOffset << std::endl
		<< "minTexelGatherOffset: " << limits.minTexelGatherOffset << std::endl
		<< "maxTexelGatherOffset: " << limits.maxTexelGatherOffset << std::endl
		<< "minInterpolationOffset: " << limits.minInterpolationOffset << std::endl
		<< "maxInterpolationOffset: " << limits.maxInterpolationOffset << std::endl
		<< "subPixelInterpolationOffsetBits: " << limits.subPixelInterpolationOffsetBits << std::endl
		<< "maxFramebufferWidth: " << limits.maxFramebufferWidth << std::endl
		<< "maxFramebufferHeight: " << limits.maxFramebufferHeight << std::endl
		<< "maxFramebufferLayers: " << limits.maxFramebufferLayers << std::endl
		<< "framebufferColorSampleCounts: ";  sample_count_flags_to_stream(limits.framebufferColorSampleCounts, o) << std::endl
		<< "framebufferDepthSampleCounts: ";  sample_count_flags_to_stream(limits.framebufferDepthSampleCounts, o) << std::endl
		<< "framebufferStencilSampleCounts: ";  sample_count_flags_to_stream(limits.framebufferStencilSampleCounts, o) << std::endl
		<< "framebufferNoAttachmentsSampleCounts: ";  sample_count_flags_to_stream(limits.framebufferNoAttachmentsSampleCounts, o) << std::endl
		<< "maxColorAttachments: " << limits.maxColorAttachments << std::endl
		<< "sampledImageColorSampleCounts: "; sample_count_flags_to_stream(limits.sampledImageColorSampleCounts, o) << std::endl
		<< "sampledImageIntegerSampleCounts: "; sample_count_flags_to_stream(limits.sampledImageIntegerSampleCounts, o) << std::endl
		<< "sampledImageDepthSampleCounts: "; sample_count_flags_to_stream(limits.sampledImageDepthSampleCounts, o) << std::endl
		<< "sampledImageStencilSampleCounts: "; sample_count_flags_to_stream(limits.sampledImageStencilSampleCounts, o) << std::endl
		<< "storageImageSampleCounts: "; sample_count_flags_to_stream(limits.storageImageSampleCounts, o) << std::endl
		<< "maxSampleMaskWords: " << limits.maxSampleMaskWords << std::endl
		<< "timestampComputeAndGraphics: " << limits.timestampComputeAndGraphics << std::endl
		<< "timestampPeriod: " << limits.timestampPeriod << std::endl
		<< "maxClipDistances: " << limits.maxClipDistances << std::endl
		<< "maxCullDistances: " << limits.maxCullDistances << std::endl
		<< "maxCombinedClipAndCullDistances: " << limits.maxCombinedClipAndCullDistances << std::endl
		<< "discreteQueuePriorities: " << limits.discreteQueuePriorities << std::endl
		<< "pointSizeRange: "; array_to_stream(limits.pointSizeRange, o) << std::endl
		<< "lineWidthRange: "; array_to_stream(limits.lineWidthRange, o) << std::endl
		<< "pointSizeGranularity: " << limits.pointSizeGranularity << std::endl
		<< "lineWidthGranularity: " << limits.lineWidthGranularity << std::endl
		<< "strictLines: " << limits.strictLines << std::endl
		<< "standardSampleLocations: " << limits.standardSampleLocations << std::endl
		<< "optimalBufferCopyOffsetAlignment: " << limits.optimalBufferCopyOffsetAlignment << std::endl
		<< "optimalBufferCopyRowPitchAlignment: " << limits.optimalBufferCopyRowPitchAlignment << std::endl
		<< "nonCoherentAtomSize: " << limits.nonCoherentAtomSize;
	
	return o;
}

std::ostream& physical_device_properties(const VkPhysicalDeviceProperties& props, std::ostream& o)
{
	o << "apiVersion: " << props.apiVersion << std::endl
		<< "driverVersion: " << props.driverVersion << std::endl
		<< "vendorID: " << props.vendorID << std::endl
		<< "deviceID: " << props.deviceID << std::endl
		<< "deviceType: "; physical_device_type_to_stream(props.deviceType, o) << std::endl
		<< "deviceName: " << props.deviceName[VK_MAX_PHYSICAL_DEVICE_NAME_SIZE] << std::endl
		<< "pipelineCacheUUID: "; array_to_stream(props.pipelineCacheUUID, o) << std::endl
		<< "limits --- " << std::endl; physical_device_limits_to_stream(props.limits, o) << std::endl << "---" << std::endl
		<< "sparseProperties --- " << std::endl; physical_device_sparce_properties(props.sparseProperties, o) << std::endl << "---" << std::endl;
	
	return o;
}

std::ostream& physical_device_sparce_properties(const VkPhysicalDeviceSparseProperties& props, std::ostream& o)
{
	o << "residencyStandard2DBlockShape: " << props.residencyStandard2DBlockShape << std::endl
		<< "residencyStandard2DMultisampleBlockShape: " << props.residencyStandard2DMultisampleBlockShape << std::endl
		<< "residencyStandard3DBlockShape: " << props.residencyStandard3DBlockShape << std::endl
		<< "residencyAlignedMipSize: " << props.residencyAlignedMipSize << std::endl
		<< "residencyNonResidentStrict: " << props.residencyNonResidentStrict << std::endl;

	return o;
}

std::ostream& physical_device_type_to_stream(const VkPhysicalDeviceType& type, std::ostream& o)
{
	switch (type) {
		case VK_PHYSICAL_DEVICE_TYPE_OTHER:				o << "VK_PHYSICAL_DEVICE_TYPE_OTHER"; break;
		case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:	o << "VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU"; break;
		case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:		o << "VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU"; break;
		case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:		o << "VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU"; break;
		case VK_PHYSICAL_DEVICE_TYPE_CPU:				o << "VK_PHYSICAL_DEVICE_TYPE_CPU"; break;
	}

	return o;
}

std::ostream& sample_count_flags_to_stream(const VkSampleCountFlags& flags, std::ostream& o)
{
	switch (flags) {
		case VK_SAMPLE_COUNT_1_BIT:		o << "VK_SAMPLE_COUNT_1_BIT"; break;
		case VK_SAMPLE_COUNT_2_BIT:		o << "VK_SAMPLE_COUNT_2_BIT"; break;
		case VK_SAMPLE_COUNT_4_BIT:		o << "VK_SAMPLE_COUNT_4_BIT"; break;
		case VK_SAMPLE_COUNT_8_BIT:		o << "VK_SAMPLE_COUNT_8_BIT"; break;
		case VK_SAMPLE_COUNT_16_BIT:	o << "VK_SAMPLE_COUNT_16_BIT"; break;
		case VK_SAMPLE_COUNT_32_BIT:	o << "VK_SAMPLE_COUNT_32_BIT"; break;
		case VK_SAMPLE_COUNT_64_BIT:	o << "VK_SAMPLE_COUNT_64_BIT"; break;
	}

	return o;
}

} // namespace learn_vk
