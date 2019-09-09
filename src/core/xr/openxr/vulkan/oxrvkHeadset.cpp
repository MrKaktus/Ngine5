
#include "core/xr/openxr/vulkan/oxrvkHeadset.h"

#include "core/rendering/vulkan/vkDevice.h"

#if defined(EN_MODULE_RENDERER_VULKAN)

#if defined(EN_PLATFORM_ANDROID)
#define XR_USE_PLATFORM_ANDROID
#endif
#if defined(EN_PLATFORM_WINDOWS)
#define XR_USE_PLATFORM_WIN32
#endif
#define XR_USE_GRAPHICS_API_VULKAN
#define XR_NO_PROTOTYPES

#include "OpenXR/openxr.h"
#include "OpenXR/openxr_platform.h"
#include "OpenXR/openxr_platform_defines.h"

#include "core/xr/openxr/oxrDLL.h"
#include "core/xr/openxr/oxrValidate.h"
#include "core/xr/openxr/oxrInterface.h"


// oxrvkInterface.h

// Already defined in openxr_platform.h (can disable with XR_NO_PROTOTYPES)

// XR_KHR_vulkan_enable
DeclareFunction( xrGetVulkanGraphicsRequirementsKHR )
DeclareFunction( xrGetVulkanGraphicsDeviceKHR )
DeclareFunction( xrGetVulkanInstanceExtensionsKHR )
DeclareFunction( xrGetVulkanDeviceExtensionsKHR )
//*/

namespace en
{
namespace xr
{

oxrHeadsetVK::oxrHeadsetVK(oxrInterface& _session) :
    oxrHeadset(_session)
{
    loadInterfaceFunctionPointers();

    // Query supported Vulkan API versions
    XrGraphicsRequirementsVulkanKHR requirements;
    Validate(session, xrGetVulkanGraphicsRequirementsKHR(session->instance, runtime, &requirements))

    // Log supported versions
    std::string vulkanVersionInfo;
    if (XR_VERSION_MAJOR(requirements.minApiVersionSupported) == XR_VERSION_MAJOR(requirements.maxApiVersionSupported) &&
        XR_VERSION_MINOR(requirements.minApiVersionSupported) == XR_VERSION_MINOR(requirements.maxApiVersionSupported))
    {
        vulkanVersionInfo += "OpenXR Runtime is compatible with Vulkan API version ";
        vulkanVersionInfo += XR_VERSION_MAJOR(requirements.minApiVersionSupported);
        vulkanVersionInfo += ".";
        vulkanVersionInfo += XR_VERSION_MINOR(requirements.minApiVersionSupported);
        vulkanVersionInfo += "\n";
    }
    else
    {
        vulkanVersionInfo += "OpenXR Runtime is compatible with Vulkan API versions ";
        vulkanVersionInfo += XR_VERSION_MAJOR(requirements.minApiVersionSupported);
        vulkanVersionInfo += ".";
        vulkanVersionInfo += XR_VERSION_MINOR(requirements.minApiVersionSupported);
        vulkanVersionInfo += " to ";
        vulkanVersionInfo += XR_VERSION_MAJOR(requirements.maxApiVersionSupported);
        vulkanVersionInfo += ".";
        vulkanVersionInfo += XR_VERSION_MINOR(requirements.maxApiVersionSupported);
        vulkanVersionInfo += "\n";
    }
    Log << vulkanVersionInfo;

    // TODO: After Vulkan GPU subsystem is created call:

    // Query GPU that should be used to render to this Headset
    gpu::VulkanAPI* vulkanRuntime = (gpu::VulkanAPI*)&(*en::Graphics);
    VkPhysicalDevice physicalDevice;
    Validate(session, xrGetVulkanGraphicsDeviceKHR(session->instance, runtime, vulkanRuntime->instance, &physicalDevice))

    // Use physicalDevice to find VulkanDevice abstracting given GPU
    for (uint32 i=0; i<vulkanRuntime->devicesCount; ++i)
    {
        gpu::VulkanDevice* gpuDevice = &*vulkanRuntime->_device[i];
        if (gpuDevice->handle == physicalDevice)
        {
            gpu = gpuDevice;
        }
    }

/*
XrResult xrGetVulkanInstanceExtensionsKHR(session->instance, system,
    uint32_t                                    namesCapacityInput,
    uint32_t*                                   namesCountOutput,
    char*                                       namesString);

XrResult xrGetVulkanDeviceExtensionsKHR(session->instance, system,
    uint32_t                                    namesCapacityInput,
    uint32_t*                                   namesCountOutput,
    char*                                       namesString);
*/

}

oxrHeadsetVK::~oxrHeadsetVK()
{
    // TODO: Release resources

    clearInterfaceFunctionPointers();
}

// oxrvkInterface.cpp

void oxrHeadsetVK::loadInterfaceFunctionPointers(void)
{
    // XR_KHR_vulkan_enable
    LoadInstanceFunction(session, xrGetVulkanGraphicsRequirementsKHR)
    LoadInstanceFunction(session, xrGetVulkanGraphicsDeviceKHR)
    LoadInstanceFunction(session, xrGetVulkanInstanceExtensionsKHR)
    LoadInstanceFunction(session, xrGetVulkanDeviceExtensionsKHR)
}

void oxrHeadsetVK::clearInterfaceFunctionPointers(void)
{
    // XR_KHR_vulkan_enable
    xrGetVulkanGraphicsRequirementsKHR = nullptr;
    xrGetVulkanGraphicsDeviceKHR       = nullptr;
    xrGetVulkanInstanceExtensionsKHR   = nullptr;
    xrGetVulkanDeviceExtensionsKHR     = nullptr;
}

} // en::xr
} // en

#endif