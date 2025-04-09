

#include "core/rendering/device.h"
#include "core/xr/openxr/oxrValidate.h"
#include "core/xr/openxr/oxrDLL.h"
#include "core/xr/openxr/oxrInterface.h"
#include "core/xr/openxr/oxrHeadset.h"

#include "utilities/utilities.h"

namespace en
{
namespace xr
{

void unbindedOpenXRFunctionHandler(...)
{
    Log << "ERROR: Called unbinded OpenXR function.\n";
    assert( 0 );
}

#define CaseToString( c )    \
    case(c):                 \
        info += #c;          \
        break;               \

bool IsError(const XrResult result)
{
    sint32 value = static_cast<sint32>(result);
    if (value >= 0)
    {
        return false;
    }

    // Compose error message
    std::string info;
    info += "ERROR: OpenXR error: ";
    switch (result)
    {
        CaseToString(XR_ERROR_VALIDATION_FAILURE)
        CaseToString(XR_ERROR_RUNTIME_FAILURE)
        CaseToString(XR_ERROR_OUT_OF_MEMORY)
        CaseToString(XR_ERROR_RUNTIME_VERSION_INCOMPATIBLE)
        CaseToString(XR_ERROR_DRIVER_INCOMPATIBLE)
        CaseToString(XR_ERROR_INITIALIZATION_FAILED)
        CaseToString(XR_ERROR_FUNCTION_UNSUPPORTED)
        CaseToString(XR_ERROR_FEATURE_UNSUPPORTED)
        CaseToString(XR_ERROR_EXTENSION_NOT_PRESENT)
        CaseToString(XR_ERROR_LIMIT_REACHED)
        CaseToString(XR_ERROR_SIZE_INSUFFICIENT)
        CaseToString(XR_ERROR_HANDLE_INVALID)
        CaseToString(XR_ERROR_INSTANCE_LOST)
        CaseToString(XR_ERROR_SESSION_RUNNING)
        CaseToString(XR_ERROR_SESSION_NOT_RUNNING)
        CaseToString(XR_ERROR_SESSION_LOST)
        CaseToString(XR_ERROR_SYSTEM_INVALID)
        CaseToString(XR_ERROR_PATH_INVALID)
        CaseToString(XR_ERROR_PATH_COUNT_EXCEEDED)
        CaseToString(XR_ERROR_PATH_FORMAT_INVALID)
        CaseToString(XR_ERROR_LAYER_INVALID)
        CaseToString(XR_ERROR_LAYER_LIMIT_EXCEEDED)
        CaseToString(XR_ERROR_SWAPCHAIN_RECT_INVALID)
        CaseToString(XR_ERROR_SWAPCHAIN_FORMAT_UNSUPPORTED)
        CaseToString(XR_ERROR_ACTION_TYPE_MISMATCH)
        CaseToString(XR_ERROR_REFERENCE_SPACE_UNSUPPORTED)
        CaseToString(XR_ERROR_FILE_ACCESS_ERROR)
        CaseToString(XR_ERROR_FILE_CONTENTS_INVALID)
        CaseToString(XR_ERROR_FORM_FACTOR_UNSUPPORTED)
        CaseToString(XR_ERROR_FORM_FACTOR_UNAVAILABLE)
        CaseToString(XR_ERROR_API_LAYER_NOT_PRESENT)
        CaseToString(XR_ERROR_CALL_ORDER_INVALID)
        CaseToString(XR_ERROR_GRAPHICS_DEVICE_INVALID)
        CaseToString(XR_ERROR_POSE_INVALID)
        CaseToString(XR_ERROR_INDEX_OUT_OF_RANGE)
        CaseToString(XR_ERROR_VIEW_CONFIGURATION_TYPE_UNSUPPORTED)
        CaseToString(XR_ERROR_ENVIRONMENT_BLEND_MODE_UNSUPPORTED)
        CaseToString(XR_ERROR_BINDINGS_DUPLICATED)
        CaseToString(XR_ERROR_NAME_DUPLICATED)
        CaseToString(XR_ERROR_NAME_INVALID)
        CaseToString(XR_ERROR_ANDROID_THREAD_SETTINGS_ID_INVALID_KHR)
        CaseToString(XR_ERROR_ANDROID_THREAD_SETTINGS_FAILURE_KHR)
        CaseToString(XR_ERROR_DEBUG_UTILS_MESSENGER_INVALID_EXT)

    default:
        info += "Unknown error code!";
        break;
    };

    info += "\n";
    Log << info.c_str();
    return true;
}

bool IsWarning(const XrResult result)
{
    if (result == XR_SUCCESS)
    {
        return false;
    }

    // Compose warning message
    std::string info;
    info += "WARNING: OpenXR warning: ";
    switch (result)
    {
        CaseToString(XR_TIMEOUT_EXPIRED)
        CaseToString(XR_SESSION_VISIBILITY_UNAVAILABLE)
        CaseToString(XR_SESSION_LOSS_PENDING)
        CaseToString(XR_EVENT_UNAVAILABLE)
        CaseToString(XR_STATE_UNAVAILABLE)
        CaseToString(XR_STATE_TYPE_UNAVAILABLE)
        CaseToString(XR_SPACE_BOUNDS_UNAVAILABLE)
        CaseToString(XR_SESSION_NOT_FOCUSED)
        CaseToString(XR_FRAME_DISCARDED)

    default:
        info += "Unknown warning code!";
        break;
    };

    info += "\n";
    Log << info.c_str();
    return true;
}

LayerDescriptor::LayerDescriptor() :
    extension(nullptr),
    extensionsCount(0)
{
}

bool oxrInterface::isPresent(void)
{
    // There is no official way, to even verify, if there is any OpenXR runtime
    // present in the system, to which OpenXR launcher can connect to. As launcher
    // seems to search for runtime lazily at first function call, we do exactly
    // that here.

    PFN_xrEnumerateInstanceExtensionProperties xrEnumerateInstanceExtensionProperties;

    XrResult result = xrGetInstanceProcAddr(nullptr,                              
                                            "xrEnumerateInstanceExtensionProperties",                            
                                            (PFN_xrVoidFunction*)&xrEnumerateInstanceExtensionProperties);      

    if (en::xr::IsError(result) ||
        xrEnumerateInstanceExtensionProperties == nullptr)                                                  
    {                                                                             
        return false;
    }                                                                             
    
    uint32 globalExtensionsCount = 0;
    result = xrEnumerateInstanceExtensionProperties(nullptr, 0, &globalExtensionsCount, nullptr); 
    if (en::xr::IsError(result)) // XR_ERROR_INSTANCE_LOST
    { 
        return false;
    }

    return true;
}

oxrInterface::oxrInterface(std::string appName) :
    instance(nullptr),
    layer(nullptr),
    layersCount(0),
    globalExtension(nullptr),
    globalExtensionsCount(0),
    primaryHeadset(nullptr)
{
    for (uint32 i = 0; i < MaxSupportedThreads; ++i)
    {
        lastResult[i] = XR_SUCCESS;
    }

    // Load CoreXR global function pointers
    LoadGlobalFunction( xrEnumerateInstanceExtensionProperties )
    LoadGlobalFunction( xrEnumerateApiLayerProperties )
    LoadGlobalFunction( xrCreateInstance )

    // Gather available OpenXR Extensions
    //------------------------------------

    Validate( this, xrEnumerateInstanceExtensionProperties(nullptr, 0, &globalExtensionsCount, nullptr) )
    if (globalExtensionsCount > 0)
    {
        globalExtension = new XrExtensionProperties[globalExtensionsCount];
        for(uint32 i=0; i<globalExtensionsCount; ++i)
        {
            globalExtension[i].type             = XR_TYPE_EXTENSION_PROPERTIES;
            globalExtension[i].next             = nullptr;
            globalExtension[i].extensionName[0] = 0;
            globalExtension[i].specVersion      = 0;
        }

        Validate(this, xrEnumerateInstanceExtensionProperties(nullptr, globalExtensionsCount, &globalExtensionsCount, globalExtension))
    }

    // Gather available OpenXR Instance Layers & Layers Extensions
    //-------------------------------------------------------------

    Validate( this, xrEnumerateApiLayerProperties(0, &layersCount, nullptr) )
    if (layersCount > 0)
    {
        XrApiLayerProperties* layerProperties = new XrApiLayerProperties[layersCount];
        assert( layerProperties );

        uint32 layersReturned = 0;
        Validate( this, xrEnumerateApiLayerProperties(layersCount, &layersReturned, layerProperties) )

        // Layers count can change at runtime thus below doesn't need to be true:
        // assert(layersReturned == layersCount);

        layer = new LayerDescriptor[layersReturned];

        // Acquire information about each Layer and supported extensions
        for (uint32 i = 0; i < layersReturned; ++i)
        {
            layer[i].properties = layerProperties[i];
            layer[i].extensionsCount = 0;
            Validate( this, xrEnumerateInstanceExtensionProperties(layer[i].properties.layerName, 0, &layer[i].extensionsCount, nullptr) )
            if (layer[i].extensionsCount > 0)
            {
                layer[i].extension = new XrExtensionProperties[layer[i].extensionsCount];
                Validate( this, xrEnumerateInstanceExtensionProperties(layer[i].properties.layerName, layer[i].extensionsCount, &layer[i].extensionsCount, layer[i].extension) )
            }
        }

        delete[] layerProperties;
    }

    // Specify OpenXR Extensions to initialize
    //-----------------------------------------

    uint32 enabledExtensionsCount = 0;
    char** extensionPtrs = nullptr;
    extensionPtrs = new char*[globalExtensionsCount];


    // Adding Debug specific extensions to the list
#if defined(EN_DEBUG)
    //extensionPtrs[enabledExtensionsCount++] = ;
#endif

    // Adding System specific extensions to the list
#if defined(EN_PLATFORM_ANDROID)
    extensionPtrs[enabledExtensionsCount++] = XR_KHR_ANDROID_THREAD_SETTINGS_EXTENSION_NAME;
    extensionPtrs[enabledExtensionsCount++] = XR_KHR_ANDROID_SURFACE_SWAPCHAIN_EXTENSION_NAME;
    extensionPtrs[enabledExtensionsCount++] = XR_KHR_ANDROID_CREATE_INSTANCE_EXTENSION_NAME;
#endif
#if defined(EN_PLATFORM_LINUX)
    // TODO: Pick one on Linux (as usual it's complete mess)
    extensionPtrs[enabledExtensionsCount++] = ;
    extensionPtrs[enabledExtensionsCount++] = ;
    extensionPtrs[enabledExtensionsCount++] = ;
    extensionPtrs[enabledExtensionsCount++] = XR_KHR_CONVERT_TIMESPEC_TIME_EXTENSION_NAME;
#endif
#if defined(EN_PLATFORM_WINDOWS)
    extensionPtrs[enabledExtensionsCount++] = XR_KHR_WIN32_CONVERT_PERFORMANCE_COUNTER_TIME_EXTENSION_NAME;
#endif

    // Init specific graphics API extensions
    en::gpu::RenderingAPI api = en::Graphics->type();
    if (api == en::gpu::RenderingAPI::Direct3D)
    {
#if defined(EN_MODULE_RENDERER_DIRECT3D12)
        // Enable D3D12 extensions if using D3D12 for rendering
        extensionPtrs[enabledExtensionsCount++] = XR_KHR_D3D12_ENABLE_EXTENSION_NAME;
#endif
    }
    else
    if (api == en::gpu::RenderingAPI::Vulkan)
    {
#if defined(EN_MODULE_RENDERER_VULKAN)
        // Init Vulkan specific extensions
        extensionPtrs[enabledExtensionsCount++] = XR_KHR_VULKAN_ENABLE_EXTENSION_NAME;
        extensionPtrs[enabledExtensionsCount++] = XR_KHR_VULKAN_SWAPCHAIN_FORMAT_LIST_EXTENSION_NAME;
#endif
    }

    // TODO: Add here loading list of needed extensions from some config file.
    //       (generated automatically by the engine/editor based on features
    //        used by the app).

    // Verify selected extensions are available
    for(uint32 i=0; i<enabledExtensionsCount; ++i)
    {
        bool found = false;
        for(uint32 j=0; j<globalExtensionsCount; ++j)
        {
            if (strcmp(extensionPtrs[i], globalExtension[j].extensionName) == 0)
            {
                found = true;
            }
        }

        if (!found)
        {
            Log << "ERROR: Requested OpenXR extension " << extensionPtrs[i] << " is not supported on this system!\n";
            Log << "       Supported extensions:\n";
            for(uint32 j=0; j<globalExtensionsCount; ++j)
            {
                Log << "       - " << globalExtension[j].extensionName << std::endl;
            }

            assert( 0 );
        }
    }

    // Specify OpenXR Layers and their Extensions to initialize
    //----------------------------------------------------------

    uint32 enabledLayersCount = 0;
    char** layersPtrs = nullptr;

#if defined(EN_DEBUG)
    if (layersCount > 0)
    {
        Log << "Available OpenXR Layers:\n";
        for (uint32 i = 0; i < layersCount; ++i)
        {
            Log << "  " << layer[i].properties.layerName << std::endl;
        }
    }

    // TODO: Read list of requested layers to enable from config file

    // Cannot enable more layers than all currently available
    layersPtrs = new char*[layersCount];

    // Layers available on machine with raw graphic driver
    // XR_APILAYER_
    // XR_APILAYER_
    // XR_APILAYER_

    // Temporary WA to manually select layers to load
    //layersPtrs[enabledLayersCount++] = "XR_APILAYER_";
    //layersPtrs[enabledLayersCount++] = "XR_APILAYER_";
    //layersPtrs[enabledLayersCount++] = "XR_APILAYER_";
    //layersPtrs[enabledLayersCount++] = "XR_APILAYER_";
    //layersPtrs[enabledLayersCount++] = "XR_APILAYER_";
    //layersPtrs[enabledLayersCount++] = "XR_APILAYER_";
    //layersPtrs[enabledLayersCount++] = "XR_APILAYER_";
    //layersPtrs[enabledLayersCount++] = "XR_APILAYER_";
    //layersPtrs[enabledLayersCount++] = "XR_APILAYER_";
    //layersPtrs[enabledLayersCount++] = "XR_APILAYER_";
    //layersPtrs[enabledLayersCount++] = "XR_APILAYER_";
    //layersPtrs[enabledLayersCount++] = "XR_APILAYER_";
    //layersPtrs[enabledLayersCount++] = "XR_APILAYER_";
    //layersPtrs[enabledLayersCount++] = "XR_APILAYER_LUNARG_standard_validation";  // Meta-layer - Loads standard set of validation layers in optimal order (all of them in fact)

    //// In debug mode enable additional layers for debugging,
    //// profiling and other purposes (currently all available).
    //enabledLayersCount = layersCount;
    //layersPtrs = new char*[layersCount];
    //for(uint32 i=0; i<layersCount; ++i)  
    //   layersPtrs[i] = &layer[i].properties.layerName[0];

    // Debug: Use below to activate all available layers and their extensions:
    //
    //  // Create array of pointers to all layer names
    //  uint32 enabledLayersCount = layersCount;
    //  char* layersPtrs = char*[layersCount];
    //  uint32 index = 0;
    //  for(uint32 i=0; i<layersCount; ++i, ++index)  
    //     layersPtrs[index] = &layer[i].properties.layerName[0];
    //  
    //  // Calculate total amount of extensions supported by all Layers
    //  uint32 enabledExtensionsCount = globalExtensionsCount;
    //  for(uint32 i=0; i<layersCount; ++i)
    //     enabledExtensionsCount += layer[i].extensionsCount;  
    //  
    //  // Create array of pointers to this extension names
    //  char* extensionPtrs = char*[enabledExtensionsCount];
    //  uint32 index = 0;
    //  for(uint32 i=0; i<globalExtensionsCount; ++i, ++index)
    //     extensionPtrs[index] = &globalExtension[i].extName[0];
    //  for(uint32 i=0; i<layersCount; ++i)
    //     for(uint32 j=0; j<layer[i].extensionsCount; ++j, ++index)   
    //        extensionPtrs[index] = &layer[i].extension[j].extName[0];
#endif

    // Create Application OpenXR API Instance
    //----------------------------------------

    std::string engineName("Ngine");  // TODO: Move out of constructor

    XrApplicationInfo appInfo;
    memcpy(&appInfo.applicationName, appName.c_str(), min(appName.size(), (size_t)XR_MAX_APPLICATION_NAME_SIZE));
    appInfo.applicationVersion = XR_MAKE_VERSION(1, 0, 0);
    memcpy(&appInfo.engineName, engineName.c_str(), min(engineName.size(), (size_t)XR_MAX_ENGINE_NAME_SIZE));
    appInfo.engineVersion      = XR_MAKE_VERSION(5, 0, 0);
    appInfo.apiVersion         = XR_CURRENT_API_VERSION;

    XrInstanceCreateInfo instInfo;
    instInfo.type                  = XR_TYPE_INSTANCE_CREATE_INFO;
    instInfo.next                  = nullptr;
    instInfo.createFlags           = 0;        // XrInstanceCreateFlags - Reserved.
    instInfo.applicationInfo       = appInfo;
    instInfo.enabledApiLayerCount  = enabledLayersCount;
    instInfo.enabledApiLayerNames  = reinterpret_cast<const char*const*>(layersPtrs);
    instInfo.enabledExtensionCount = enabledExtensionsCount;
    instInfo.enabledExtensionNames = reinterpret_cast<const char*const*>(extensionPtrs);

    Validate( this, xrCreateInstance(&instInfo, &instance) )

    delete[] extensionPtrs;
    delete[] layersPtrs;

    // Bind Interface functions
    loadInterfaceFunctionPointers();

    // Gather runtime properties
    XrInstanceProperties properties;
    Validate( this, xrGetInstanceProperties(instance, &properties) )

    std::string runtimeInfo;
    runtimeInfo += "OpenXR Runtime v";
    runtimeInfo += XR_VERSION_MAJOR(properties.runtimeVersion);
    runtimeInfo += ".";
    runtimeInfo += XR_VERSION_MINOR(properties.runtimeVersion);
    runtimeInfo += ".";
    runtimeInfo += XR_VERSION_PATCH(properties.runtimeVersion);
    runtimeInfo += "\n";
    runtimeInfo += "       ";
    runtimeInfo += properties.runtimeName[XR_MAX_RUNTIME_NAME_SIZE];

    Log << runtimeInfo;
}

oxrInterface::~oxrInterface()
{
    delete primaryHeadset;

    clearInterfaceFunctionPointers();

    Validate( this, xrDestroyInstance(instance) )

    for (uint32 i = 0; i < layersCount; i++)
    {
        delete[] layer[i].extension;
    }

    delete[] layer;
    delete[] globalExtension;
}

void oxrInterface::loadInterfaceFunctionPointers(void)
{
#if defined(EN_PLATFORM_ANDROID)
    // XR_KHR_android_surface_swapchain
    LoadInstanceFunction( this, xrCreateSwapchainAndroidSurfaceKHR )

    // XR_KHR_android_thread_settings
    LoadInstanceFunction( this, xrSetAndroidApplicationThreadKHR )
#endif

#if defined(EN_PLATFORM_LINUX)
    // XR_KHR_convert_timespec_time
    LoadInstanceFunction( this, xrConvertTimespecTimeToTimeKHR )
    LoadInstanceFunction( this, xrConvertTimeToTimespecTimeKHR )
#endif

    // XR_KHR_visibility_mask
    LoadInstanceFunction( this, xrGetVisibilityMaskKHR )

#if defined(EN_PLATFORM_WINDOWS)
    // XR_KHR_win32_convert_performance_counter_time
    LoadInstanceFunction( this, xrConvertWin32PerformanceCounterToTimeKHR )
    LoadInstanceFunction( this, xrConvertTimeToWin32PerformanceCounterKHR )
#endif

#if defined(EN_DEBUG)
    // XR_EXT_debug_utils
    LoadInstanceFunction( this, xrSetDebugUtilsObjectNameEXT )
    LoadInstanceFunction( this, xrCreateDebugUtilsMessengerEXT )
    LoadInstanceFunction( this, xrDestroyDebugUtilsMessengerEXT )
    LoadInstanceFunction( this, xrSubmitDebugUtilsMessageEXT )
    LoadInstanceFunction( this, xrSessionBeginDebugUtilsLabelRegionEXT )
    LoadInstanceFunction( this, xrSessionEndDebugUtilsLabelRegionEXT )
    LoadInstanceFunction( this, xrSessionInsertDebugUtilsLabelEXT )
#endif

    // XR_EXT_performance_settings
    LoadInstanceFunction( this, xrPerfSettingsSetPerformanceLevelEXT )

    // XR_EXT_thermal_query
    LoadInstanceFunction( this, xrThermalGetTemperatureTrendEXT )
}

void oxrInterface::clearInterfaceFunctionPointers(void)
{
    // OS Function Pointer
    //xrGetInstanceProcAddr                     = nullptr;

    // OpenXR Function Pointers
    xrEnumerateInstanceExtensionProperties    = nullptr;
    xrEnumerateApiLayerProperties             = nullptr;
    xrCreateInstance                          = nullptr;

#if defined(EN_PLATFORM_ANDROID)
    // XR_KHR_android_surface_swapchain
    xrCreateSwapchainAndroidSurfaceKHR        = nullptr;

    // XR_KHR_android_thread_settings
    xrSetAndroidApplicationThreadKHR          = nullptr;
#endif

#if defined(EN_PLATFORM_LINUX)
    // XR_KHR_convert_timespec_time
    xrConvertTimespecTimeToTimeKHR            = nullptr;
    xrConvertTimeToTimespecTimeKHR            = nullptr;
#endif

    // XR_KHR_visibility_mask
    xrGetVisibilityMaskKHR                    = nullptr;

#if defined(EN_PLATFORM_WINDOWS)
    // XR_KHR_win32_convert_performance_counter_time
    xrConvertWin32PerformanceCounterToTimeKHR = nullptr;
    xrConvertTimeToWin32PerformanceCounterKHR = nullptr;
#endif

#if defined(EN_DEBUG)
    // XR_EXT_debug_utils
    xrSetDebugUtilsObjectNameEXT              = nullptr;
    xrCreateDebugUtilsMessengerEXT            = nullptr;
    xrDestroyDebugUtilsMessengerEXT           = nullptr;
    xrSubmitDebugUtilsMessageEXT              = nullptr;
    xrSessionBeginDebugUtilsLabelRegionEXT    = nullptr;
    xrSessionEndDebugUtilsLabelRegionEXT      = nullptr;
    xrSessionInsertDebugUtilsLabelEXT         = nullptr;
#endif

    // XR_EXT_performance_settings
    xrPerfSettingsSetPerformanceLevelEXT      = nullptr;

    // XR_EXT_thermal_query
    xrThermalGetTemperatureTrendEXT           = nullptr;
}

uint32 oxrInterface::headsets(void)
{
    // Delay sub-system initialization until application actually wants to use XR
    if (!primaryHeadset)
    {
        // Create single Headset (runtime) object
        //----------------------------------------

        primaryHeadset = new oxrHeadset(*this);
    }

    return 1;
}

Headset* oxrInterface::headset(const uint32 index)
{
    // Delay sub-system initialization until application actually wants to use XR
    if (!primaryHeadset)
    {
        // Create single Headset (runtime) object
        //----------------------------------------

        primaryHeadset = new oxrHeadset(*this);
    }

    return primaryHeadset;
}

} // en::xr
} // en