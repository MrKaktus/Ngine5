/*

 Ngine v5.0

 Module      : OpenXR Interface
 Requirements: none
 Description : XR context abstracts system specific
               backend implementation exposing XR
               functionality (AR/MR/VR). It allows
               programmer to use easy abstraction
               layer on all platforms.

*/

#ifndef ENG_CORE_XR_OPENXR_INTERFACE
#define ENG_CORE_XR_OPENXR_INTERFACE

#include "assert.h"

#include "core/defines.h"

#if defined(EN_PLATFORM_ANDROID)
#define XR_USE_PLATFORM_ANDROID
#endif
#if defined(EN_PLATFORM_WINDOWS)
#define XR_USE_PLATFORM_WIN32
#endif

#if defined(EN_MODULE_RENDERER_DIRECT3D12)
#include "core/rendering/d3d12/dx12.h"
#define XR_USE_GRAPHICS_API_D3D12
#endif
#if defined(EN_MODULE_RENDERER_VULKAN)
#include "core/rendering/vulkan/vulkan.h"
#define XR_USE_GRAPHICS_API_VULKAN
#endif

#include "OpenXR/openxr.h"
#include "OpenXR/openxr_platform.h"

#include "core/types.h"
#include "core/xr/interface.h"

#include "core/log/log.h"
#include "core/parallel/thread.h"
#include "platform/system.h"

#define DeclareFunction(function)                                     \
    PFN_##function function;

namespace en
{
namespace xr
{
extern void unbindedOpenXRFunctionHandler(...);

// OpenXR API Layer description
struct LayerDescriptor
{
    XrApiLayerProperties   properties;
    XrExtensionProperties* extension;
    uint32                 extensionsCount;

    LayerDescriptor();
};

class oxrHeadset;

class oxrInterface : public Interface
{
public:
    XrInstance             instance;
    XrResult               lastResult[MaxSupportedThreads];
    LayerDescriptor*       layer;
    uint32                 layersCount;
    XrExtensionProperties* globalExtension;
    uint32                 globalExtensionsCount;

    XrSpace                worldSpace; // Space in relation to which player and objects are tracked
    XrSpace                headSpace;  // Head space, in relation to which views are tracked

    oxrHeadset*            headset;    // OpenXR supports only one headset

    // OS function pointer
    //DeclareFunction( xrGetInstanceProcAddr )

    // OpenXR global function pointers
    DeclareFunction( xrEnumerateInstanceExtensionProperties )
    DeclareFunction( xrEnumerateApiLayerProperties )
    DeclareFunction( xrCreateInstance )

#if defined(EN_PLATFORM_ANDROID)
    // XR_KHR_android_surface_swapchain
    DeclareFunction( xrCreateSwapchainAndroidSurfaceKHR )

    // XR_KHR_android_thread_settings
    DeclareFunction( xrSetAndroidApplicationThreadKHR )
#endif

#if defined(EN_PLATFORM_LINUX)
    // XR_KHR_convert_timespec_time
    DeclareFunction( xrConvertTimespecTimeToTimeKHR )
    DeclareFunction( xrConvertTimeToTimespecTimeKHR )
#endif

    // XR_KHR_visibility_mask
    DeclareFunction( xrGetVisibilityMaskKHR )

// TODO: requires openxr_platfrom

#if defined(EN_PLATFORM_WINDOWS)
    // XR_KHR_win32_convert_performance_counter_time
    DeclareFunction( xrConvertWin32PerformanceCounterToTimeKHR )
    DeclareFunction( xrConvertTimeToWin32PerformanceCounterKHR )
#endif

#if defined(EN_DEBUG)
    // XR_EXT_debug_utils
    DeclareFunction( xrSetDebugUtilsObjectNameEXT )
    DeclareFunction( xrCreateDebugUtilsMessengerEXT )
    DeclareFunction( xrDestroyDebugUtilsMessengerEXT )
    DeclareFunction( xrSubmitDebugUtilsMessageEXT )
    DeclareFunction( xrSessionBeginDebugUtilsLabelRegionEXT )
    DeclareFunction( xrSessionEndDebugUtilsLabelRegionEXT )
    DeclareFunction( xrSessionInsertDebugUtilsLabelEXT )
#endif

    // XR_EXT_performance_settings
    DeclareFunction( xrPerfSettingsSetPerformanceLevelEXT )

    // XR_EXT_thermal_query
    DeclareFunction( xrThermalGetTemperatureTrendEXT )


    static bool isPresent(void);

    oxrInterface(std::string appName);
   ~oxrInterface();

    void loadInterfaceFunctionPointers(void);
    void clearInterfaceFunctionPointers(void);
};

} // en::xr
} // en

#endif