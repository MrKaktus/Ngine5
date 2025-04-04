/*

 Ngine v5.0
 
 Module      : Vulkan GPU Device.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#include "core/rendering/vulkan/vkDevice.h"

#if defined(EN_MODULE_RENDERER_VULKAN)

#include "core/log/log.h"
#include "core/memory/alignedAllocator.h"
#include "core/storage.h"   // For Pipeline Cache handling
#include "utilities/strings.h"

#include "core/rendering/vulkan/vkValidate.h"
#include "core/rendering/vulkan/vkCommandBuffer.h"
#include "core/rendering/vulkan/vkTexture.h"
#include "core/rendering/vulkan/vkSynchronization.h"

#include "core/rendering/windows/winDisplay.h"

namespace en
{
namespace gpu
{

void unbindedVulkanFunctionHandler(...)
{
    Log << "ERROR: Called unbinded Vulkan function.\n";
    assert( 0 );
}

// MACROS: Safe Vulkan function binding with fallback
//         Both Macros should be used only inside VulkanAPI and VulkanDevice class methods.

#if defined(EN_PLATFORM_LINUX)
    #define LoadProcAddress dlsym
#endif

#if defined(EN_PLATFORM_WINDOWS)
    #define LoadProcAddress GetProcAddress
#endif
 
#define DeclareFunction(function)                                                  \
PFN_##function function;

#define LoadFunction(function)                                                     \
{                                                                                  \
    function = (PFN_##function)LoadProcAddress(library, #function);                \
    if (function == nullptr)                                                       \
    {                                                                              \
        function = (PFN_##function) &unbindedVulkanFunctionHandler;                \
        Log << "Error: Cannot bind function " << #function << std::endl;           \
    }                                                                              \
}

#define LoadGlobalFunction(function)                                               \
{                                                                                  \
    function = (PFN_##function) vkGetInstanceProcAddr(nullptr, #function);         \
    if (function == nullptr)                                                       \
    {                                                                              \
        function = (PFN_##function) &unbindedVulkanFunctionHandler;                \
        Log << "Error: Cannot bind global function " << #function << std::endl;    \
    }                                                                              \
}
   
#define LoadInstanceFunction(function)                                             \
{                                                                                  \
    function = (PFN_##function) vkGetInstanceProcAddr(instance, #function);        \
    if (function == nullptr)                                                       \
    {                                                                              \
        function = (PFN_##function) &unbindedVulkanFunctionHandler;                \
        Log << "Error: Cannot bind instance function " << #function << std::endl;  \
    }                                                                              \
}

#define LoadDeviceFunction(function)                                               \
{                                                                                  \
    /* We don't need to do this. api is already declared in Device.                \ 
    VulkanAPI* api = reinterpret_cast<VulkanAPI*>(Graphics.get()); */              \
    function = (PFN_##function) api->vkGetDeviceProcAddr(device, #function);       \
    if (function == nullptr)                                                       \
    {                                                                              \
        function = (PFN_##function) &unbindedVulkanFunctionHandler;                \
        Log << "Error: Cannot bind device function " << #function << std::endl;    \
    }                                                                              \
}

// Checks Vulkan error state
bool IsError(const VkResult result)
{
    sint32 value = static_cast<sint32>(result);
    if (value >= 0)
    {
        return false;
    }
   
    // Compose error message
    std::string info;
    info += "ERROR: Vulkan error: ";
    if (result == VK_ERROR_OUT_OF_HOST_MEMORY)
    {
        info += "VK_ERROR_OUT_OF_HOST_MEMORY.\n";
    }
    else
    if (result == VK_ERROR_OUT_OF_DEVICE_MEMORY)
    {
        info += "VK_ERROR_OUT_OF_DEVICE_MEMORY.\n";
    }
    else
    if (result == VK_ERROR_INITIALIZATION_FAILED)
    {
        info += "VK_ERROR_INITIALIZATION_FAILED.\n";
    }
    else
    if (result == VK_ERROR_DEVICE_LOST)
    {
        info += "VK_ERROR_DEVICE_LOST.\n";
    }
    else
    if (result == VK_ERROR_MEMORY_MAP_FAILED)
    {
        info += "VK_ERROR_MEMORY_MAP_FAILED.\n";
    }
    else
    if (result == VK_ERROR_LAYER_NOT_PRESENT)
    {
        info += "VK_ERROR_LAYER_NOT_PRESENT.\n";
    }
    else
    if (result == VK_ERROR_EXTENSION_NOT_PRESENT)
    {
        info += "VK_ERROR_EXTENSION_NOT_PRESENT.\n";
        info += "       Cannot find a specified extension library.\n";
        info += "       Make sure your layers path is set appropriately.\n";
    }
    else
    if (result == VK_ERROR_INCOMPATIBLE_DRIVER)
    {
        info += "VK_ERROR_INCOMPATIBLE_DRIVER.\n";
        info += "       Cannot find a compatible Vulkan installable client driver (ICD).\n";
        info += "       Please check that your graphic card support Vulkan API and that you have latest graphic drivers installed.\n";
    }

    Log << info.c_str();
    return true; 
}

// Checks Vulkan warning state
bool IsWarning(const VkResult result)
{
    sint32 value = static_cast<sint32>(result);
    if (value == 0)
    {
        return false;
    }
   
    // Compose error message
    std::string info;
    info += "WARNING: Vulkan error: ";
    if (result == VK_NOT_READY)
    {
        info += "VK_NOT_READY.\n";
    }
    else
    if (result == VK_TIMEOUT)
    {
        info += "VK_TIMEOUT.\n";
    }
    else
    if (result == VK_EVENT_SET)
    {
        info += "VK_EVENT_SET.\n";
    }
    else
    if (result == VK_EVENT_RESET)
    {
        info += "VK_EVENT_RESET.\n";
    }
    else
    if (result == VK_INCOMPLETE)
    {
        info += "VK_INCOMPLETE.\n";
    }

    Log << info.c_str();
    return true; 
}

LayerDescriptor::LayerDescriptor() :
    extension(nullptr),
    extensionsCount(0)
{
}

// CPU memory allocation for given GPU device control
void* VKAPI_PTR defaultAlloc(
    void*                   pUserData,
    size_t                  size,
    size_t                  alignment,
    VkSystemAllocationScope allocationScope)
{
    void* ptr = reinterpret_cast<void*>(allocate<uint8>(size, alignment));
    if (ptr)
    {
        VulkanDevice* deviceVK = reinterpret_cast<VulkanDevice*>(pUserData);
        deviceVK->memoryRAM += size;
    }

    return ptr;
}
   
void* VKAPI_PTR defaultRealloc(
    void*                   pUserData,
    void*                   pOriginal,
    size_t                  size,
    size_t                  alignment,
    VkSystemAllocationScope allocationScope)
{
#ifdef EN_PLATFORM_WINDOWS
    // TODO: Needs to know size of original allocation !
    //return reinterpret_cast<void*>(reallocate<uint8>(reinterpret_cast<uint8*>(pOriginal), alignment, size));
    assert( 0 );
    return nullptr;
#else
    // THERE IS NO MEM ALIGNED REALLOC ON UNIX SYSTEMS !
    void* ptr = reinterpret_cast<void*>(allocate<uint8>(size, alignment));
    if (ptr)
    {
        // TODO: Needs to know size of original allocation !
        //if (pOriginal)
        //   memcpy(ptr, pOriginal, size);
        deallocate<uint8>(reinterpret_cast<uint8*>(pOriginal));

        VulkanDevice* deviceVK = reinterpret_cast<VulkanDevice*>(pUserData);

        // TODO: Needs to know size of original allocation !
        //deviceVK->memoryRAM -= oldSize;
        //deviceVK->memoryRAM += size;
    }

    return ptr;
#endif
}
   
void VKAPI_PTR defaultFree(
    void* pUserData,
    void* pMemory)
{
    deallocate<uint8>(reinterpret_cast<uint8*>(pMemory));

    VulkanDevice* deviceVK = reinterpret_cast<VulkanDevice*>(pUserData);
    // TODO: Needs to know size of original allocation !
    //deviceVK->memoryRAM -= oldSize;
}
   
void VKAPI_PTR defaultIntAlloc(
    void*                    pUserData,
    size_t                   size,
    VkInternalAllocationType allocationType,
    VkSystemAllocationScope  allocationScope)
{
    VulkanDevice* deviceVK = reinterpret_cast<VulkanDevice*>(pUserData);
    deviceVK->memoryDriver += size;
}

void VKAPI_PTR defaultIntFree(
    void*                    pUserData,
    size_t                   size,
    VkInternalAllocationType allocationType,
    VkSystemAllocationScope  allocationScope)
{
    VulkanDevice* deviceVK = reinterpret_cast<VulkanDevice*>(pUserData);
    deviceVK->memoryDriver -= size;
}

// TODO: Currently max is 16MB. This should be configurable through config file.
#define PipelineCacheMaximumSize 16*MB

struct taskClosure
{
    VulkanDevice* device;
    uint32 worker;
    std::atomic<uint32>* workersInitialized;
};

void taskCreateCommandPools(void* data)
{
    taskClosure* state = (taskClosure*)(data);

    // Memory pool used to allocate CommandBuffers.
    // Once device is created, on each Worker Thread, we create Command Pool for each Queue Family
    // that was associated to each of our Queue Types. Then each time Command Buffer creation will
    // be requested for Queue from given Queue Type, apropriate Command Pool for matching Queue
    // Family will be used (taking into notice parent thread).
    for(uint32 i=0; i<underlyingType(QueueType::Count); ++i)
    {
        if (state->device->queuesCount[i] > 0)
        {
            uint32 queueFamilyId = state->device->queueTypeToFamily[i];
    
            // It's abstract object as it has no size.
            // It's not tied to Queueu, but to Queue Family, so it can be shared by all Queues in this family.
            // It's single threaded, and so should be Command Buffers allocated from it
            // (so each Thread gets it's own Pool per each Family, and have it's own Command Buffers).
            VkCommandPoolCreateInfo poolInfo;
            poolInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            poolInfo.pNext            = nullptr;
            poolInfo.flags            = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT; // To reuse CB's use VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT
            poolInfo.queueFamilyIndex = queueFamilyId;
            
            Validate( state->device, vkCreateCommandPool(state->device->device, &poolInfo, nullptr, (VkCommandPool*)(&state->device->commandPool[state->worker][i])) )
        }
    }

    // TODO: Do we want to support reset of Command Pools? Should it go to API?
    //
    // // Resets Command Pool, frees all resources encoded on all CB's created from this pool. CB's are reset to initial state.
    // uint32 thread = 0; 
    // uint32 type = 0;
    // Validate( vkResetCommandPool(VkDevice device, commandPool[thread][type], VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT) )

    // Increase counter of worker threads that are initialized
    state->workersInitialized->fetch_add(1, std::memory_order_relaxed);
}

void taskDestroyCommandPools(void* data)
{
    taskClosure* state = (taskClosure*)(data);

    // Release all Command Pools used by this thread for Commands submission
    for(uint32 i=0; i<underlyingType(QueueType::Count); ++i)
    {
        if (state->device->queuesCount[i] > 0)
        {
            ValidateNoRet( state->device, vkDestroyCommandPool(state->device->device, state->device->commandPool[state->worker][i], nullptr) )
        }
    }

    // Increase counter of worker threads that are de-initialized
    uint32 previousValue = state->workersInitialized->fetch_add(1, std::memory_order_relaxed);

#ifdef EN_DEBUG
    std::string workerIndex = "Worker ";
    workerIndex.append(stringFrom(state->worker));
    workerIndex.append(": ");
    workerIndex.append(stringFrom(previousValue + 1));
    workerIndex.append("\n");

    Log << workerIndex;
#endif
}

VulkanDevice::VulkanDevice(VulkanAPI* _api, const uint32 _index, const VkPhysicalDevice _handle) :
    api(_api),
    index(_index),
    handle(_handle),
    queueFamily(nullptr),
    queueFamiliesCount(0),
    queueFamilyIndices(nullptr),
    globalExtension(nullptr),
    globalExtensionsCount(0),
    pipelineCache(VK_NULL_HANDLE),
    rebuildCache(true),
    memoryRAM(0),
    memoryDriver(0),
    CommonDevice()
{
    for(uint32 i=0; i<MaxSupportedThreads; ++i)
    {
        lastResult[i] = VK_SUCCESS;
    }

    // Clear Device function pointers
    clearDeviceFunctionPointers();

    // Init default memory allocation callbacks
    defaultAllocCallbacks.pUserData             = this;
    defaultAllocCallbacks.pfnAllocation         = defaultAlloc;
    defaultAllocCallbacks.pfnReallocation       = defaultRealloc;
    defaultAllocCallbacks.pfnFree               = defaultFree;
    defaultAllocCallbacks.pfnInternalAllocation = defaultIntAlloc;
    defaultAllocCallbacks.pfnInternalFree       = defaultIntFree;

    // Gather Device Capabilities
    ValidateNoRet( api, vkGetPhysicalDeviceFeatures(handle, &features) )
    ValidateNoRet( api, vkGetPhysicalDeviceProperties(handle, &properties) )
    ValidateNoRet( api, vkGetPhysicalDeviceMemoryProperties(handle, &memory) )

    for(uint32_t i=0; i<memory.memoryHeapCount; ++i)
    {
        Log << "Memory range " << stringFrom(i) << ":\n\n";
        Log << "    Size          : "  << stringFrom(memory.memoryHeaps[i].size) << "\n";
        Log << "    Backing memory: ";
        if (memory.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
        {
            Log << "VRAM\n";
        }
        else
        { 
            Log << "RAM\n";
        }
        Log << "    Memory types  :\n";
        for(uint32_t j=0; j<memory.memoryTypeCount; ++j)
        {
            if (memory.memoryTypes[j].heapIndex == i)
            {
                Log << "    Type: "  << stringFrom(j) << "\n";
                if (memory.memoryTypes[j].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
                {
                    Log << "        Supports: VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT\n";
                }
                if (memory.memoryTypes[j].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
                {
                    Log << "        Supports: VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT\n";
                }
                if (memory.memoryTypes[j].propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
                {
                    Log << "        Supports: VK_MEMORY_PROPERTY_HOST_COHERENT_BIT\n";
                }
                if (memory.memoryTypes[j].propertyFlags & VK_MEMORY_PROPERTY_HOST_CACHED_BIT)
                {
                    Log << "        Supports: VK_MEMORY_PROPERTY_HOST_CACHED_BIT\n\n\n"; 
                }
                if (memory.memoryTypes[j].propertyFlags & VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT)
                {
                    Log << "        Supports: VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT\n\n\n"; 
                }
            }
        }
    }

 // TODO: Gather even more information
 //ValidateNoRet( api, vkGetPhysicalDeviceFormatProperties(handle, VkFormat format, VkFormatProperties* pFormatProperties) )
 //ValidateNoRet( api, vkGetPhysicalDeviceImageFormatProperties(handle, VkFormat format, VkImageType type, VkImageTiling tiling, VkImageUsageFlags usage, VkImageCreateFlags flags, VkImageFormatProperties* pImageFormatProperties) )

    // Queue Families, Queues
    //------------------------

    // Gather information about Queue Families supported by this device
    ValidateNoRet( api, vkGetPhysicalDeviceQueueFamilyProperties(handle, &queueFamiliesCount, nullptr) )
    queueFamily = new VkQueueFamilyProperties[queueFamiliesCount];
    ValidateNoRet( api, vkGetPhysicalDeviceQueueFamilyProperties(handle, &queueFamiliesCount, queueFamily) )

    // Generate list of all Queue Family indices.
    // This list will be passed during resource creation time for resources that need to be populated first.
    // This way we can use separate Transfer Queues for faster data transfer, and driver knows to use
    // proper synchronization mechanisms to prevent hazards and race conditions.
    queueFamilyIndices = new uint32[queueFamiliesCount];
    for (uint32 i=0; i<queueFamiliesCount; ++i)
    {
        queueFamilyIndices[i] = i;
    }

    // Map Queue Families and their Queue Count to QueueType and Index.
    memset(&queuesCount[0], 0, sizeof(queuesCount));
    memset(&queueTypeToFamily[0], 0, sizeof(queueTypeToFamily));
    for(uint32_t family=0; family<queueFamiliesCount; ++family)
    {
        uint32 flags = queueFamily[family].queueFlags;

// Vulkan 1.1
#ifndef VK_QUEUE_PROTECTED_BIT
#define VK_QUEUE_PROTECTED_BIT 0x00000010
#endif

// Provided by VK_KHR_video_decode_queue
#ifndef VK_QUEUE_VIDEO_DECODE_BIT_KHR
#define VK_QUEUE_VIDEO_DECODE_BIT_KHR 0x00000020
#endif

// Provided by VK_KHR_video_encode_queue
#ifndef VK_QUEUE_VIDEO_ENCODE_BIT_KHR
#define VK_QUEUE_VIDEO_ENCODE_BIT_KHR 0x00000040
#endif

// Provided by VK_NV_optical_flow
#ifndef VK_QUEUE_OPTICAL_FLOW_BIT_NV
#define VK_QUEUE_OPTICAL_FLOW_BIT_NV 0x00000100
#endif

        // For now, we assume that there will be always only one Queue Family matching one Queue Type.
        // If that's not true in the future, then below, each range of Queues in given Family, will need
        // to be mapped to range of Queues in given Type. If such situation will occur, assertions below
        // will fire at runtime.
        if (flags == (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT | VK_QUEUE_SPARSE_BINDING_BIT))
        {
            assert( queuesCount[underlyingType(QueueType::Universal)] == 0 );
            queuesCount[underlyingType(QueueType::Universal)] = queueFamily[family].queueCount;
            queueTypeToFamily[underlyingType(QueueType::Universal)] = family;
        }
        else
        if (flags == (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT))
        {
            assert( queuesCount[underlyingType(QueueType::Universal)] == 0 );
            queuesCount[underlyingType(QueueType::Universal)] = queueFamily[family].queueCount;
            queueTypeToFamily[underlyingType(QueueType::Universal)] = family;
        }
        else
        if (flags == (VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT | VK_QUEUE_SPARSE_BINDING_BIT))
        {
            assert( queuesCount[underlyingType(QueueType::Compute)] == 0 );
            queuesCount[underlyingType(QueueType::Compute)] = queueFamily[family].queueCount;
            queueTypeToFamily[underlyingType(QueueType::Compute)] = family;
        }
        else
        if (flags == (VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT))
        {
            assert( queuesCount[underlyingType(QueueType::Compute)] == 0 );
            queuesCount[underlyingType(QueueType::Compute)] = queueFamily[family].queueCount;
            queueTypeToFamily[underlyingType(QueueType::Compute)] = family;
        }
        else
        if (flags == (VK_QUEUE_TRANSFER_BIT | VK_QUEUE_SPARSE_BINDING_BIT))
        {
            assert( queuesCount[underlyingType(QueueType::SparseTransfer)] == 0 );
            queuesCount[underlyingType(QueueType::SparseTransfer)] = queueFamily[family].queueCount;
            queueTypeToFamily[underlyingType(QueueType::SparseTransfer)] = family;
        }
        else
        if (flags == VK_QUEUE_TRANSFER_BIT)
        {
            assert( queuesCount[underlyingType(QueueType::Transfer)] == 0 );
            queuesCount[underlyingType(QueueType::Transfer)] = queueFamily[family].queueCount;
            queueTypeToFamily[underlyingType(QueueType::Transfer)] = family;
        }
        else
        if (flags == (VK_QUEUE_VIDEO_ENCODE_BIT_KHR | VK_QUEUE_TRANSFER_BIT | VK_QUEUE_SPARSE_BINDING_BIT))
        {
            assert(queuesCount[underlyingType(QueueType::VideoEncode)] == 0);
            queuesCount[underlyingType(QueueType::VideoEncode)] = queueFamily[family].queueCount;
            queueTypeToFamily[underlyingType(QueueType::VideoEncode)] = family;
        }
        else
        if (flags == (VK_QUEUE_VIDEO_DECODE_BIT_KHR | VK_QUEUE_TRANSFER_BIT | VK_QUEUE_SPARSE_BINDING_BIT))
        {
            assert(queuesCount[underlyingType(QueueType::VideoDecode)] == 0);
            queuesCount[underlyingType(QueueType::VideoDecode)] = queueFamily[family].queueCount;
            queueTypeToFamily[underlyingType(QueueType::VideoDecode)] = family;
        }
        else
        if (flags == (VK_QUEUE_OPTICAL_FLOW_BIT_NV | VK_QUEUE_TRANSFER_BIT | VK_QUEUE_SPARSE_BINDING_BIT))
        {
            assert(queuesCount[underlyingType(QueueType::OpticalFlow)] == 0);
            queuesCount[underlyingType(QueueType::OpticalFlow)] = queueFamily[family].queueCount;
            queueTypeToFamily[underlyingType(QueueType::OpticalFlow)] = family;
        }
        else
        {
            // TODO: VK_QUEUE_PROTECTED_BIT
// 
            // It's another combination of Queue Family flags that we don't support.
            // This shouldn't happen but if it will we will report it without asserting.
         
            Log << "Unsupported type of Queue Family\n";
            Log << "    Flags:" << flags << std::endl;
            Log << "    Queues in Family: " << queueFamily[family].queueCount << std::endl;
            Log << "    Queue Min Transfer Width : " << queueFamily[family].minImageTransferGranularity.width << std::endl;
            Log << "    Queue Min Transfer Height: " << queueFamily[family].minImageTransferGranularity.height << std::endl;
            Log << "    Queue Min Transfer Depth : " << queueFamily[family].minImageTransferGranularity.depth << std::endl;
            if (queueFamily[family].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                Log << "    Queue supports: GRAPHICS\n";
            }
            if (queueFamily[family].queueFlags & VK_QUEUE_COMPUTE_BIT)
            {
                Log << "    Queue supports: COMPUTE\n";
            }
            if (queueFamily[family].queueFlags & VK_QUEUE_TRANSFER_BIT)
            {
                Log << "    Queue supports: TRANSFER\n";
            }
            if (queueFamily[family].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT)
            {
                Log << "    Queue supports: SPARSE_BINDING\n";
            }
            if (queueFamily[family].queueFlags & VK_QUEUE_VIDEO_ENCODE_BIT_KHR)
            {
                Log << "    Queue supports: VIDEO_ENCODE\n";
            }
            if (queueFamily[family].queueFlags & VK_QUEUE_VIDEO_DECODE_BIT_KHR)
            {
                Log << "    Queue supports: VIDEO_DECODE\n";
            }
            if (queueFamily[family].queueFlags & VK_QUEUE_OPTICAL_FLOW_BIT_NV)
            {
                Log << "    Queue supports: OPTICAL_FLOW\n";
            }
            Log << "    Queues Time Stamp: " << queueFamily[family].timestampValidBits << std::endl << std::endl;
        }
    }
   
#if defined(EN_DEBUG)
    // Debug log Queue Families
    for(uint32_t family=0; family<queueFamiliesCount; ++family)
    {
        Log << "Queue Family: " << family << std::endl;

        Log << "    Queues: " << queueFamily[family].queueCount << std::endl;
        Log << "    Queue Min Transfer W: " << queueFamily[family].minImageTransferGranularity.width << std::endl;
        Log << "    Queue Min Transfer H: " << queueFamily[family].minImageTransferGranularity.height << std::endl;
        Log << "    Queue Min Transfer D: " << queueFamily[family].minImageTransferGranularity.depth << std::endl;
        if (queueFamily[family].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            Log << "    Family supports: GRAPHICS\n";
        }
        if (queueFamily[family].queueFlags & VK_QUEUE_COMPUTE_BIT)
        {
            Log << "    Family supports: COMPUTE\n";
        }
        if (queueFamily[family].queueFlags & VK_QUEUE_TRANSFER_BIT)
        {
            Log << "    Family supports: TRANSFER\n";
        }
        if (queueFamily[family].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT)
        {
            Log << "    Family supports: SPARSE_BINDING\n";
        }
        if (queueFamily[family].queueFlags & VK_QUEUE_VIDEO_ENCODE_BIT_KHR)
        {
            Log << "    Family supports: VIDEO_ENCODE\n";
        }
        if (queueFamily[family].queueFlags & VK_QUEUE_VIDEO_DECODE_BIT_KHR)
        {
            Log << "    Family supports: VIDEO_DECODE\n";
        }
        if (queueFamily[family].queueFlags & VK_QUEUE_OPTICAL_FLOW_BIT_NV)
        {
            Log << "    Family supports: OPTICAL_FLOW\n";
        }
        Log << "    Queues Time Stamp: " << queueFamily[family].timestampValidBits << std::endl << std::endl;
    }
#endif

    // Device Extensions
    //-------------------

    // Acquire list of Device extensions
    Validate( api, vkEnumerateDeviceExtensionProperties(handle, nullptr, &globalExtensionsCount, nullptr) )
    if (IsWarning(api->lastResult[currentThreadId()]))
    {
        assert( 0 );
    }

    if (globalExtensionsCount > 0)
    {
        globalExtension = new VkExtensionProperties[globalExtensionsCount];
        Validate( api, vkEnumerateDeviceExtensionProperties(handle, nullptr, &globalExtensionsCount, globalExtension) )
    }

    // While creating device, we can choose to init as many Queue Families as we want (but each only once).
    // In each Queue Family we can specify how many Queues we want to create and init.
    // For now lets just init everything.
    VkDeviceQueueCreateInfo* queueFamilyInfo = new VkDeviceQueueCreateInfo[queueFamiliesCount];
    for(uint32 i=0; i<queueFamiliesCount; ++i)
    {
        uint32 queues = queueFamily[i].queueCount;

        queueFamilyInfo[i].sType              = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueFamilyInfo[i].pNext              = nullptr; 
        queueFamilyInfo[i].flags              = 0; // Reserved
        queueFamilyInfo[i].queueFamilyIndex   = i;
        queueFamilyInfo[i].queueCount         = queues;

        // Mark all queues from the same family to have the same priority during workload distribution
        float* priorities = new float[queues];
        for(uint32 priority=0; priority<queues; ++priority)
        {
            priorities[priority] = 1.0f;
        }

        queueFamilyInfo[i].pQueuePriorities   = priorities;
    }

    // Specify Vulkan Extensions to initialize
    //-----------------------------------------

    uint32 enabledExtensionsCount = 0;
    char** extensionPtrs = nullptr;
    extensionPtrs = new char*[globalExtensionsCount];

    // Adding Windowing System Interface extensions to the list
    extensionPtrs[enabledExtensionsCount++] = VK_KHR_SWAPCHAIN_EXTENSION_NAME;

    // Adding minimum required set of extensions
    extensionPtrs[enabledExtensionsCount++] = VK_KHR_MAINTENANCE1_EXTENSION_NAME;

    // TODO: Add here loading list of needed extensions from some config file.
    //       (generated automatically by the engine/editor based on features
    //        used by the app).

    // Verify selected extensions are available
    for(uint32 i=0; i<enabledExtensionsCount; ++i)
    {
        bool found = false;
        for(uint32 j=0; j<globalExtensionsCount; ++j)
        {
            if (strcmp(extensionPtrs[i], globalExtension[j].extensionName) == 0 )
            {
                found = true;
            }
        }

        if (!found)
        {
            Log << "ERROR: Requested Vulkan extension " << extensionPtrs[i] << " is not supported on this system!\n";
            Log << "       Supported extensions:\n";
            for(uint32 j=0; j<globalExtensionsCount; ++j)
            {
                Log << "       - " << globalExtension[j].extensionName << std::endl;
            }

            assert( 0 );
        }
    }

    // Create Vulkan Device
    //----------------------

    // Create Device Interface
    VkDeviceCreateInfo deviceInfo;
    deviceInfo.sType                     = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceInfo.pNext                     = nullptr;
    deviceInfo.flags                     = 0;                     // Reserved
    deviceInfo.queueCreateInfoCount      = queueFamiliesCount;
    deviceInfo.pQueueCreateInfos         = queueFamilyInfo;
    deviceInfo.enabledLayerCount         = 0;                     // Deprecated, ignored.
    deviceInfo.ppEnabledLayerNames       = nullptr;               // Deprecated, ignored.
    deviceInfo.enabledExtensionCount     = enabledExtensionsCount;
    deviceInfo.ppEnabledExtensionNames   = reinterpret_cast<const char*const*>(extensionPtrs);
    deviceInfo.pEnabledFeatures          = nullptr;

    // TODO: In the future provide our own allocation callbacks to track
    //       and analyze drivers system memory usage (&defaultAllocCallbacks).

    Validate( api, vkCreateDevice(handle, &deviceInfo, nullptr, &device) )

    // Bind Device Functions
    loadDeviceFunctionPointers();

    // Free temporary resources
    delete [] extensionPtrs;

    // Command Pools
    //---------------
 
    taskClosure state[MaxSupportedWorkerThreads];

    // Vulkan device is created on main thread, outside of worker threads pool.
    // This means that main thread needs to wait until all worker threads will 
    // process their tasks, but as it's IO thread, it needs to use manual 
    // synchronization (IO threads cannot wait on Tasks).
    // This will stall main thread, but that's ok, since this happens at engine
    // initialization stage, before application main() function will be spawned.
    std::atomic<uint32> workersInitialized = 0;

    // Each thread has separate command pool for each Queue Type
    for(uint32 worker=0; worker<en::Scheduler->workers(); ++worker)
    {
        state[worker].device = this;
        state[worker].worker = worker;
        state[worker].workersInitialized = &workersInitialized;

        en::Scheduler->runOnWorker(worker, taskCreateCommandPools, (void*)&state[worker]);
    }

    // Wait until scheduler finishes initialization of this device on all workers
    bool executing = false;
    while(std::atomic_load_explicit(&workersInitialized, std::memory_order_relaxed) < en::Scheduler->workers())
    {
        _mm_pause();
    }

    // Pipeline Cache
    //---------------

    // Try to load cache from disk
    uint64 cacheSize = 0u;
    void*  cacheData = loadPipelineCache(cacheSize);

    // Create runtime Pipeline cache, and reuse previous data if possible
    VkPipelineCacheCreateInfo cacheInfo;
    cacheInfo.sType           = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    cacheInfo.pNext           = nullptr;
    cacheInfo.flags           = 0u;
    cacheInfo.initialDataSize = cacheSize; // Cache size will never be greater than 4GB
    cacheInfo.pInitialData    = cacheData;

    Validate( this, vkCreatePipelineCache(device, &cacheInfo, nullptr, &pipelineCache) )

    // Release temporary buffer
    delete [] cacheData;

    init();
}


void* VulkanDevice::loadPipelineCache(uint64& size)
{
    using namespace en::storage;

    // Pipeline cache header
    alignTo(1) 
    struct PipelineCacheHeader
    {
        uint32 headerSize;
        VkPipelineCacheHeaderVersion version;
        uint32 vendorID;
        uint32 deviceID;
        uint32 cacheUUID[4];
    };
    alignToDefault

    // Try to reuse pipeline cache from disk. 
    // It is assumed that devices are always enumerated in the same order.
    std::string filename = std::string("gpu") + stringFrom(index) + std::string("pipelineCache.data");
    std::shared_ptr<File> file = Storage->open(filename);
    if (!file)
    {
        return nullptr;
    }

    // Verify that cache file is not corrupted
    uint64 diskCacheSize = file->size();
    if (diskCacheSize < sizeof(PipelineCacheHeader))
    {
        file = nullptr;
        return nullptr;
    }

    // Size of cache copy on disk shouldn't exceed the limit
    if (diskCacheSize > PipelineCacheMaximumSize)
    {
        file = nullptr;
        return nullptr;
    }

    // Read header of cache stored on disk
    PipelineCacheHeader diskHeader;
    file->read(0u, sizeof(PipelineCacheHeader), &diskHeader);

    // Check if cache is matching this GPU
    if (diskHeader.vendorID != properties.vendorID ||
        diskHeader.deviceID != properties.deviceID) 
    {
        file = nullptr;
        return nullptr;
    }

    // Check if cache needs to be rebuild due to changed UUID (it won't match after drivers update, GPU change, etc.).
    if (memcmp(&diskHeader.cacheUUID[0], &properties.pipelineCacheUUID[0], 16) != 0)
    {
        file = nullptr;
        return nullptr;
    }

    // Load previously cached pipeline state objects from disk
    uint8* cacheData = new uint8[diskCacheSize];
    file->read((volatile void*)cacheData);
    file = nullptr;

    size = diskCacheSize;
    rebuildCache = false;
    return cacheData;
}

void VulkanDevice::init()
{
    // Memory
    support.videoMemorySize               = 0; 
    support.systemMemorySize              = 0;
    for(uint32_t i=0; i<memory.memoryHeapCount; ++i)
    {
        if (memory.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
        {
            support.videoMemorySize  += memory.memoryHeaps[i].size;
        }
        else 
        {
            support.systemMemorySize += memory.memoryHeaps[i].size;  
        } 
    }

    // Input Assembler
    support.maxInputLayoutBuffersCount    = properties.limits.maxVertexInputBindings;
    support.maxInputLayoutAttributesCount = properties.limits.maxVertexInputAttributes;

    // Texture
  //support.maxTexture1DSize              = properties.limits.maxImageDimension1D;
    support.maxTextureSize                = properties.limits.maxImageDimension2D;
    support.maxTextureCubeSize            = properties.limits.maxImageDimensionCube; 
    support.maxTexture3DSize              = properties.limits.maxImageDimension3D;
    support.maxTextureLayers              = properties.limits.maxImageArrayLayers;          
    support.maxTextureBufferSize          = properties.limits.maxTexelBufferElements;  // It's not size in bytes, but array size?
    support.maxTextureLodBias             = properties.limits.maxSamplerLodBias;            

    // Sampler                            
    support.maxAnisotropy                 = properties.limits.maxSamplerAnisotropy;

    // Rasterizer
    support.maxColorAttachments           = properties.limits.maxColorAttachments;

    // TODO: Populate API capabilities

    for(uint32 i=0; i<MaxSupportedWorkerThreads; i++)
    {
        commandBuffersExecuting[i] = 0u;
        for(uint32 j=0; j<MaxCommandBuffersExecuting; j++)  
        {    
            commandBuffers[i][j] = nullptr;
        }
    }

    initMemoryManager();
    CommonDevice::init();
}

VulkanDevice::~VulkanDevice()
{
    Validate( this, vkDeviceWaitIdle(device) )
   
    // If requested recreate pipeline cache on disk
    if (rebuildCache)
    {
        using namespace en::storage;

        // Save cache to disk (at app end):
        // - if marked for recreation of HDD cache
        //   - retrieve current cache size
        //   - retrieve cache dat
        //   - delete cache on HDD
        //   - store it as new cache on HDD

        // Read size of driver cache
        uint64 cacheSize = 0u;
        Validate( this, vkGetPipelineCacheData(device, pipelineCache, (size_t*)(&cacheSize), nullptr) )
      
        // Try to reuse Pipeline objects from the previous application run (read from drivers cache on the disk).
        if (cacheSize > 0u)
        {
            // Size of cache copy on disk shouldn't exceed the limit
            if (cacheSize > PipelineCacheMaximumSize)
            {
                cacheSize = PipelineCacheMaximumSize;
            }

            uint8* cacheData = new uint8[cacheSize];
            Validate( this, vkGetPipelineCacheData(device, pipelineCache, (size_t*)(&cacheSize), cacheData) )

            std::string filename = std::string("gpu") + stringFrom(index) + std::string("pipelineCache.data");
            std::shared_ptr<File> file = Storage->open(filename, FileAccess::Write);
            if (file)
            {
                file->write(cacheSize, cacheData);
                file = nullptr;
            }

            delete [] cacheData;
        }
    }

    ValidateNoRet( this, vkDestroyPipelineCache(device, pipelineCache, nullptr) )

    // Release CommandBuffers in flight once they are done
    for(uint32 workerId=0; workerId<MaxSupportedWorkerThreads; ++workerId)
    {
        uint32 executing = commandBuffersExecuting[workerId];
        for(uint32 i=0; i<executing; ++i)
        {
            CommandBufferVK* command = reinterpret_cast<CommandBufferVK*>(commandBuffers[workerId][i].get());
            command->waitUntilCompleted();

            // Safely release Command Buffer object
            commandBuffers[workerId][i] = nullptr;
        }
    }

    taskClosure state[MaxSupportedWorkerThreads];

    // Vulkan device is destroyed on main thread, outside of worker threads pool.
    // This means that main thread needs to wait until all worker threads will 
    // process their tasks, but as it's IO thread, it needs to use manual 
    // synchronization (IO threads cannot wait on Tasks).
    // This will stall main thread, but that's ok, since this happens at engine
    // de-initialization stage, after application main() function completes.
    std::atomic<uint32> workersInitialized = 0;
    
    // Each thread has separate command pool for each Queue Type
    for(uint32 worker=0; worker<en::Scheduler->workers(); ++worker)
    {
        
        state[worker].device = this;
        state[worker].worker = worker;
        state[worker].workersInitialized = &workersInitialized;
    
        en::Scheduler->runOnWorker(worker, taskDestroyCommandPools, (void*)&state[worker]);
    }

    // Wait until scheduler finishes de-initialization of this device on all workers
    bool executing = false;
    while(std::atomic_load_explicit(&workersInitialized, std::memory_order_acquire) < en::Scheduler->workers())
    {
        _mm_pause();
    }

    cleanupCommonResources();

    if (device != VK_NULL_HANDLE)
    {
        ValidateNoRet( this, vkDestroyDevice(device, nullptr) )  // Instance or Device function ?
    }

    delete [] queueFamily;
    delete [] queueFamilyIndices;
    delete [] globalExtension;
}
   
void VulkanDevice::loadDeviceFunctionPointers(void)
{
    // Windows WA - Undefine Windows API defines
#ifdef CreateSemaphore
#undef CreateSemaphore
#endif
#ifdef CreateEvent
#undef CreateEvent
#endif

    // Vulkan 1.0 
    //
    LoadDeviceFunction( vkDestroyDevice )
    LoadDeviceFunction( vkGetDeviceQueue )
    LoadDeviceFunction( vkQueueSubmit )
    LoadDeviceFunction( vkQueueWaitIdle )
    LoadDeviceFunction( vkDeviceWaitIdle )
    LoadDeviceFunction( vkAllocateMemory )
    LoadDeviceFunction( vkFreeMemory )
    LoadDeviceFunction( vkMapMemory )
    LoadDeviceFunction( vkUnmapMemory )
    LoadDeviceFunction( vkFlushMappedMemoryRanges )
    LoadDeviceFunction( vkInvalidateMappedMemoryRanges )
    LoadDeviceFunction( vkGetDeviceMemoryCommitment )
    LoadDeviceFunction( vkBindBufferMemory )
    LoadDeviceFunction( vkBindImageMemory )
    LoadDeviceFunction( vkGetBufferMemoryRequirements )
    LoadDeviceFunction( vkGetImageMemoryRequirements )
    LoadDeviceFunction( vkGetImageSparseMemoryRequirements )
    LoadDeviceFunction( vkQueueBindSparse )
    LoadDeviceFunction( vkCreateFence )
    LoadDeviceFunction( vkDestroyFence )
    LoadDeviceFunction( vkResetFences )
    LoadDeviceFunction( vkGetFenceStatus )
    LoadDeviceFunction( vkWaitForFences )
    LoadDeviceFunction( vkCreateSemaphore )
    LoadDeviceFunction( vkDestroySemaphore )
    LoadDeviceFunction( vkCreateEvent )
    LoadDeviceFunction( vkDestroyEvent )
    LoadDeviceFunction( vkGetEventStatus )
    LoadDeviceFunction( vkSetEvent )
    LoadDeviceFunction( vkResetEvent )
    LoadDeviceFunction( vkCreateQueryPool )
    LoadDeviceFunction( vkDestroyQueryPool )
    LoadDeviceFunction( vkGetQueryPoolResults )
    LoadDeviceFunction( vkCreateBuffer )
    LoadDeviceFunction( vkDestroyBuffer )
    LoadDeviceFunction( vkCreateBufferView )
    LoadDeviceFunction( vkDestroyBufferView )
    LoadDeviceFunction( vkCreateImage )
    LoadDeviceFunction( vkDestroyImage )
    LoadDeviceFunction( vkGetImageSubresourceLayout )
    LoadDeviceFunction( vkCreateImageView )
    LoadDeviceFunction( vkDestroyImageView )
    LoadDeviceFunction( vkCreateShaderModule )
    LoadDeviceFunction( vkDestroyShaderModule )
    LoadDeviceFunction( vkCreatePipelineCache )
    LoadDeviceFunction( vkDestroyPipelineCache )
    LoadDeviceFunction( vkGetPipelineCacheData )
    LoadDeviceFunction( vkMergePipelineCaches )
    LoadDeviceFunction( vkCreateGraphicsPipelines )
    LoadDeviceFunction( vkCreateComputePipelines )
    LoadDeviceFunction( vkDestroyPipeline )
    LoadDeviceFunction( vkCreatePipelineLayout )
    LoadDeviceFunction( vkDestroyPipelineLayout )
    LoadDeviceFunction( vkCreateSampler )
    LoadDeviceFunction( vkDestroySampler )
    LoadDeviceFunction( vkCreateDescriptorSetLayout )
    LoadDeviceFunction( vkDestroyDescriptorSetLayout )
    LoadDeviceFunction( vkCreateDescriptorPool )
    LoadDeviceFunction( vkDestroyDescriptorPool )
    LoadDeviceFunction( vkResetDescriptorPool )
    LoadDeviceFunction( vkAllocateDescriptorSets )
    LoadDeviceFunction( vkFreeDescriptorSets )
    LoadDeviceFunction( vkUpdateDescriptorSets )
    LoadDeviceFunction( vkCreateFramebuffer )
    LoadDeviceFunction( vkDestroyFramebuffer )
    LoadDeviceFunction( vkCreateRenderPass )
    LoadDeviceFunction( vkDestroyRenderPass )
    LoadDeviceFunction( vkGetRenderAreaGranularity )
    LoadDeviceFunction( vkCreateCommandPool )
    LoadDeviceFunction( vkDestroyCommandPool )
    LoadDeviceFunction( vkResetCommandPool )
    LoadDeviceFunction( vkAllocateCommandBuffers )
    LoadDeviceFunction( vkFreeCommandBuffers )
    LoadDeviceFunction( vkBeginCommandBuffer )
    LoadDeviceFunction( vkEndCommandBuffer )
    LoadDeviceFunction( vkResetCommandBuffer )
    LoadDeviceFunction( vkCmdBindPipeline )
    LoadDeviceFunction( vkCmdSetViewport )
    LoadDeviceFunction( vkCmdSetScissor )
    LoadDeviceFunction( vkCmdSetLineWidth )
    LoadDeviceFunction( vkCmdSetDepthBias )
    LoadDeviceFunction( vkCmdSetBlendConstants )
    LoadDeviceFunction( vkCmdSetDepthBounds )
    LoadDeviceFunction( vkCmdSetStencilCompareMask )
    LoadDeviceFunction( vkCmdSetStencilWriteMask )
    LoadDeviceFunction( vkCmdSetStencilReference )
    LoadDeviceFunction( vkCmdBindDescriptorSets )
    LoadDeviceFunction( vkCmdBindIndexBuffer )
    LoadDeviceFunction( vkCmdBindVertexBuffers )
    LoadDeviceFunction( vkCmdDraw )
    LoadDeviceFunction( vkCmdDrawIndexed )
    LoadDeviceFunction( vkCmdDrawIndirect )
    LoadDeviceFunction( vkCmdDrawIndexedIndirect )
    LoadDeviceFunction( vkCmdDispatch )
    LoadDeviceFunction( vkCmdDispatchIndirect )
    LoadDeviceFunction( vkCmdCopyBuffer )
    LoadDeviceFunction( vkCmdCopyImage )
    LoadDeviceFunction( vkCmdBlitImage )
    LoadDeviceFunction( vkCmdCopyBufferToImage )
    LoadDeviceFunction( vkCmdCopyImageToBuffer )
    LoadDeviceFunction( vkCmdUpdateBuffer )
    LoadDeviceFunction( vkCmdFillBuffer )
    LoadDeviceFunction( vkCmdClearColorImage )
    LoadDeviceFunction( vkCmdClearDepthStencilImage )
    LoadDeviceFunction( vkCmdClearAttachments )
    LoadDeviceFunction( vkCmdResolveImage )
    LoadDeviceFunction( vkCmdSetEvent )
    LoadDeviceFunction( vkCmdResetEvent )
    LoadDeviceFunction( vkCmdWaitEvents )
    LoadDeviceFunction( vkCmdPipelineBarrier )
    LoadDeviceFunction( vkCmdBeginQuery )
    LoadDeviceFunction( vkCmdEndQuery )
    LoadDeviceFunction( vkCmdResetQueryPool )
    LoadDeviceFunction( vkCmdWriteTimestamp )
    LoadDeviceFunction( vkCmdCopyQueryPoolResults )
    LoadDeviceFunction( vkCmdPushConstants )
    LoadDeviceFunction( vkCmdBeginRenderPass )
    LoadDeviceFunction( vkCmdNextSubpass )
    LoadDeviceFunction( vkCmdEndRenderPass )
    LoadDeviceFunction( vkCmdExecuteCommands )

    // VK_KHR_surface - Interface extension

    // VK_KHR_swapchain
    LoadDeviceFunction( vkCreateSwapchainKHR )
    LoadDeviceFunction( vkDestroySwapchainKHR )
    LoadDeviceFunction( vkGetSwapchainImagesKHR )
    LoadDeviceFunction( vkAcquireNextImageKHR )
    LoadDeviceFunction( vkQueuePresentKHR )

    // VK_KHR_display_swapchain
    LoadDeviceFunction( vkCreateSharedSwapchainsKHR )
    
    // VK_KHR_maintenance1
    LoadDeviceFunction( vkTrimCommandPoolKHR )
    
    // Windowing System Interface - OS Specyific extension
    
    // VK_KHR_xlib_surface
    // VK_KHR_xcb_surface
    // VK_KHR_wayland_surface
    // VK_KHR_mir_surface
    // VK_KHR_android_surface
       
    // VK_KHR_win32_surface - Interface extension
}

void VulkanDevice::clearDeviceFunctionPointers(void)
{
    // Vulkan 1.0
    //
    vkCreateInstance                               = nullptr;
    vkEnumeratePhysicalDevices                     = nullptr;
    vkDestroyDevice                                = nullptr;
    vkEnumerateInstanceExtensionProperties         = nullptr;
    vkEnumerateInstanceLayerProperties             = nullptr;
    vkGetDeviceQueue                               = nullptr;
    vkQueueSubmit                                  = nullptr;
    vkQueueWaitIdle                                = nullptr;
    vkDeviceWaitIdle                               = nullptr;
    vkAllocateMemory                               = nullptr;
    vkFreeMemory                                   = nullptr;
    vkMapMemory                                    = nullptr;
    vkUnmapMemory                                  = nullptr;
    vkFlushMappedMemoryRanges                      = nullptr;
    vkInvalidateMappedMemoryRanges                 = nullptr;
    vkGetDeviceMemoryCommitment                    = nullptr;
    vkBindBufferMemory                             = nullptr;
    vkBindImageMemory                              = nullptr;
    vkGetBufferMemoryRequirements                  = nullptr;
    vkGetImageMemoryRequirements                   = nullptr;
    vkGetImageSparseMemoryRequirements             = nullptr;
    vkQueueBindSparse                              = nullptr;
    vkCreateFence                                  = nullptr;
    vkDestroyFence                                 = nullptr;
    vkResetFences                                  = nullptr;
    vkGetFenceStatus                               = nullptr;
    vkWaitForFences                                = nullptr;
    vkCreateSemaphore                              = nullptr;
    vkDestroySemaphore                             = nullptr;
    vkCreateEvent                                  = nullptr;
    vkDestroyEvent                                 = nullptr;
    vkGetEventStatus                               = nullptr;
    vkSetEvent                                     = nullptr;
    vkResetEvent                                   = nullptr;
    vkCreateQueryPool                              = nullptr;
    vkDestroyQueryPool                             = nullptr;
    vkGetQueryPoolResults                          = nullptr;
    vkCreateBuffer                                 = nullptr;
    vkDestroyBuffer                                = nullptr;
    vkCreateBufferView                             = nullptr;
    vkDestroyBufferView                            = nullptr;
    vkCreateImage                                  = nullptr;
    vkDestroyImage                                 = nullptr;
    vkGetImageSubresourceLayout                    = nullptr;
    vkCreateImageView                              = nullptr;
    vkDestroyImageView                             = nullptr;
    vkCreateShaderModule                           = nullptr;
    vkDestroyShaderModule                          = nullptr;
    vkCreatePipelineCache                          = nullptr;
    vkDestroyPipelineCache                         = nullptr;
    vkGetPipelineCacheData                         = nullptr;
    vkMergePipelineCaches                          = nullptr;
    vkCreateGraphicsPipelines                      = nullptr;
    vkCreateComputePipelines                       = nullptr;
    vkDestroyPipeline                              = nullptr;
    vkCreatePipelineLayout                         = nullptr;
    vkDestroyPipelineLayout                        = nullptr;
    vkCreateSampler                                = nullptr;
    vkDestroySampler                               = nullptr;
    vkCreateDescriptorSetLayout                    = nullptr;
    vkDestroyDescriptorSetLayout                   = nullptr;
    vkCreateDescriptorPool                         = nullptr;
    vkDestroyDescriptorPool                        = nullptr;
    vkResetDescriptorPool                          = nullptr;
    vkAllocateDescriptorSets                       = nullptr;
    vkFreeDescriptorSets                           = nullptr;
    vkUpdateDescriptorSets                         = nullptr;
    vkCreateFramebuffer                            = nullptr;
    vkDestroyFramebuffer                           = nullptr;
    vkCreateRenderPass                             = nullptr;
    vkDestroyRenderPass                            = nullptr;
    vkGetRenderAreaGranularity                     = nullptr;
    vkCreateCommandPool                            = nullptr;
    vkDestroyCommandPool                           = nullptr;
    vkResetCommandPool                             = nullptr;
    vkAllocateCommandBuffers                       = nullptr;
    vkFreeCommandBuffers                           = nullptr;
    vkBeginCommandBuffer                           = nullptr;
    vkEndCommandBuffer                             = nullptr;
    vkResetCommandBuffer                           = nullptr;
    vkCmdBindPipeline                              = nullptr;
    vkCmdSetViewport                               = nullptr;
    vkCmdSetScissor                                = nullptr;
    vkCmdSetLineWidth                              = nullptr;
    vkCmdSetDepthBias                              = nullptr;
    vkCmdSetBlendConstants                         = nullptr;
    vkCmdSetDepthBounds                            = nullptr;
    vkCmdSetStencilCompareMask                     = nullptr;
    vkCmdSetStencilWriteMask                       = nullptr;
    vkCmdSetStencilReference                       = nullptr;
    vkCmdBindDescriptorSets                        = nullptr;
    vkCmdBindIndexBuffer                           = nullptr;
    vkCmdBindVertexBuffers                         = nullptr;
    vkCmdDraw                                      = nullptr;
    vkCmdDrawIndexed                               = nullptr;
    vkCmdDrawIndirect                              = nullptr;
    vkCmdDrawIndexedIndirect                       = nullptr;
    vkCmdDispatch                                  = nullptr;
    vkCmdDispatchIndirect                          = nullptr;
    vkCmdCopyBuffer                                = nullptr;
    vkCmdCopyImage                                 = nullptr;
    vkCmdBlitImage                                 = nullptr;
    vkCmdCopyBufferToImage                         = nullptr;
    vkCmdCopyImageToBuffer                         = nullptr;
    vkCmdUpdateBuffer                              = nullptr;
    vkCmdFillBuffer                                = nullptr;
    vkCmdClearColorImage                           = nullptr;
    vkCmdClearDepthStencilImage                    = nullptr;
    vkCmdClearAttachments                          = nullptr;
    vkCmdResolveImage                              = nullptr;
    vkCmdSetEvent                                  = nullptr;
    vkCmdResetEvent                                = nullptr;
    vkCmdWaitEvents                                = nullptr;
    vkCmdPipelineBarrier                           = nullptr;
    vkCmdBeginQuery                                = nullptr;
    vkCmdEndQuery                                  = nullptr;
    vkCmdResetQueryPool                            = nullptr;
    vkCmdWriteTimestamp                            = nullptr;
    vkCmdCopyQueryPoolResults                      = nullptr;
    vkCmdPushConstants                             = nullptr;
    vkCmdBeginRenderPass                           = nullptr;
    vkCmdNextSubpass                               = nullptr;
    vkCmdEndRenderPass                             = nullptr;
    vkCmdExecuteCommands                           = nullptr;
}

uint32 VulkanDevice::queues(const QueueType type) const
{
    return queuesCount[underlyingType(type)];  //Need translation table from (Type, N) -> (Family, Index)
}

std::shared_ptr<Texture> VulkanDevice::createSharedTexture(std::shared_ptr<SharedSurface> backingSurface)
{
    // Vulkan is not supporting cross-API / cross-process surfaces for now.
    // Implement it in the future.
    assert( 0 );
    return std::shared_ptr<Texture>(nullptr);
}





   //VkDeviceMemory VulkanDevice::allocMemory(VkMemoryRequirements requirements, VkFlags properties)
   //{
   //VkDeviceMemory handle;

   //// Find Memory Type that best suits required allocation
   //uint32 index = VK_MAX_MEMORY_TYPES;
   //for(uint32 i=0; i<memory.memoryTypeCount; ++i)
   //   {
   //   // Memory Type needs to be able to support requested allocation
   //   if (checkBit(requirements.memoryTypeBits, (i+1)))
   //      // Memory Type needs to support at least requested sub-set of memory properties
   //      if ((memory.memoryTypes[i].propertyFlags & properties) == properties)
   //         index = i;

   //   // If this memory type cannot be used to allocate requested resource, continue search
   //   if (index == VK_MAX_MEMORY_TYPES)
   //      continue;

   //   // Try to allocate memory on Heap supporting this memory type
   //   VkMemoryAllocateInfo allocInfo;
   //   allocInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
   //   allocInfo.pNext           = nullptr;
   //   allocInfo.allocationSize  = requirements.size;
   //   allocInfo.memoryTypeIndex = index;
   //   
   //   // Profile will block here !!! FIXME !!!
   //   Validate( this, vkAllocateMemory(device, &allocInfo, &defaultAllocCallbacks, &handle) )
   //   if (lastResult[currentThreadId()] == VK_SUCCESS)
   //      return handle;
   //   }

   //// FAIL
   //return handle;
   //}


// typedef struct {
//     VkDeviceSize                                size;
//     VkDeviceSize                                alignment;
//     uint32_t                                    memoryTypeBits;
// } VkMemoryRequirements;
// 
// 
// - Heaps are fixed up front.
// - Their amount and types is vendor / gpu / driver dependent.
// - We alloc memory on given heap by referring to Memory Type ID -> which points at given heap. (see below).
// 
// Acquire GPU heaps count an types available :
// 
// 	void vkGetPhysicalDeviceMemoryProperties(
// 	    VkPhysicalDevice physicalDevice, 
// 	    VkPhysicalDeviceMemoryProperties* pMemoryProperties);
// 
// 	#define VK_MAX_MEMORY_TYPES               32
// 	#define VK_MAX_MEMORY_HEAPS               16
// 
// 	typedef struct {
// 	    uint32_t                   memoryTypeCount;
// 	    VkMemoryType               memoryTypes[VK_MAX_MEMORY_TYPES];
// 	    uint32_t                   memoryHeapCount;
// 	    VkMemoryHeap               memoryHeaps[VK_MAX_MEMORY_HEAPS];
// 	} VkPhysicalDeviceMemoryProperties;
// 
// 
// 
// 
// 
// Memory Heap:
// 
// 	// size is uint64
// 	typedef struct {
// 	    VkDeviceSize               size;
// 	    VkMemoryHeapFlags          flags;
// 	} VkMemoryHeap;
// 
// 	typedef enum {
// 	    VK_MEMORY_HEAP_HOST_LOCAL_BIT = 0x00000001,
// 	} VkMemoryHeapFlagBits;
// 	typedef VkFlags VkMemoryHeapFlags;
// 
// 
// Memory Type:
// 
// 	// heapIndex - refers to memoryHeaps[] array above
// 	typedef struct {
// 	    VkMemoryPropertyFlags                       propertyFlags;
// 	    uint32_t                                    heapIndex;   
// 	} VkMemoryType;
// 
// 	typedef enum {
// 	    VK_MEMORY_PROPERTY_DEVICE_ONLY = 0,
// 	    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT = 0x00000001,
// 	    VK_MEMORY_PROPERTY_HOST_NON_COHERENT_BIT = 0x00000002,
// 	    VK_MEMORY_PROPERTY_HOST_UNCACHED_BIT = 0x00000004,
// 	    VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT = 0x00000008,
// 	} VkMemoryPropertyFlagBits;
// 	typedef VkFlags VkMemoryPropertyFlags;
// 
// When allocating resource, you pass MemoryType ID in the array, and through it, Driver picks Heap to use.
// 
// 
// Query memory layout for given resource:
// 
// 	void VKAPI vkGetBufferMemoryRequirements(
// 	    VkDevice                                    device,
// 	    VkBuffer                                    buffer,
// 	    VkMemoryRequirements*                       pMemoryRequirements);
// 	
// 	void VKAPI vkGetImageMemoryRequirements(
// 	    VkDevice                                    device,
// 	    VkImage                                     image,
// 	    VkMemoryRequirements*                       pMemoryRequirements);
// 	
// 	void VKAPI vkGetImageSparseMemoryRequirements(
// 	    VkDevice                                    device,
// 	    VkImage                                     image,
// 	    uint32_t*                                   pNumRequirements,
// 	    VkSparseImageMemoryRequirements*            pSparseMemoryRequirements);
// 
// Returns simple size/offset + bits :
// 
// 	typedef struct {
// 	    VkDeviceSize                                size;
// 	    VkDeviceSize                                alignment;
// 	    uint32_t                                    memoryTypeBits;
// 	} VkMemoryRequirements;
// 	
// 	typedef struct {
// 	    VkSparseImageFormatProperties               formatProps;
// 	    uint32_t                                    imageMipTailStartLOD;
// 	    VkDeviceSize                                imageMipTailSize;
// 	    VkDeviceSize                                imageMipTailOffset;
// 	    VkDeviceSize                                imageMipTailStride;
// 	} VkSparseImageMemoryRequirements;
// 
// 
// So we alloc memory :
// 
// 	VkResult VKAPI vkAllocMemory(
// 	    VkDevice                                    device,
// 	    const VkMemoryAllocInfo*                    pAllocInfo,
// 	    VkDeviceMemory*                             pMem);
// 	
// 	// memoryTypeIndex <—— alloc by pointing on MEMORY TYPE ID 
// 	typedef struct {
// 	    VkStructureType                             sType;
// 	    const void*                                 pNext;
// 	    VkDeviceSize                                allocationSize;
// 	    uint32_t                                    memoryTypeIndex;
// 	} VkMemoryAllocInfo;
// 
// Then we can bind it to resource description/handle to given memory block range:
// 
// 	VkResult VKAPI vkBindBufferMemory(
// 	    VkDevice                                    device,
// 	    VkBuffer                                    buffer,
// 	    VkDeviceMemory                              mem,
// 	    VkDeviceSize                                memOffset);
// 	
// 	VkResult VKAPI vkBindImageMemory(
// 	    VkDevice                                    device,
// 	    VkImage                                     image,
// 	    VkDeviceMemory                              mem,
// 	    VkDeviceSize                                memOffset);
// 
// 
// So it looks like in Vulkan (VkDeviceMemory == Heap in D3D12/Metal) while Vulkan Heaps are even bigger concept.
// We do allocations on the Heap, but then we can freely bind Resource Handles to different Memory regions.
// There are also functions for mapping/unmapping/invalidating memory etc.






// VK_EXT_debug_report
///////////////////////

#define CaseString( x ) case x: return #x;

// Checks Vulkan error state
char* ObjectTypeString(const VkDebugReportObjectTypeEXT objectType)
{
    switch(objectType)
    {
        CaseString( VK_DEBUG_REPORT_OBJECT_TYPE_INSTANCE_EXT )
        CaseString( VK_DEBUG_REPORT_OBJECT_TYPE_PHYSICAL_DEVICE_EXT )
        CaseString( VK_DEBUG_REPORT_OBJECT_TYPE_DEVICE_EXT )
        CaseString( VK_DEBUG_REPORT_OBJECT_TYPE_QUEUE_EXT )
        CaseString( VK_DEBUG_REPORT_OBJECT_TYPE_SEMAPHORE_EXT )
        CaseString( VK_DEBUG_REPORT_OBJECT_TYPE_COMMAND_BUFFER_EXT )
        CaseString( VK_DEBUG_REPORT_OBJECT_TYPE_FENCE_EXT )
        CaseString( VK_DEBUG_REPORT_OBJECT_TYPE_DEVICE_MEMORY_EXT )
        CaseString( VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_EXT )
        CaseString( VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_EXT )
        CaseString( VK_DEBUG_REPORT_OBJECT_TYPE_EVENT_EXT )
        CaseString( VK_DEBUG_REPORT_OBJECT_TYPE_QUERY_POOL_EXT )
        CaseString( VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_VIEW_EXT )
        CaseString( VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_VIEW_EXT )
        CaseString( VK_DEBUG_REPORT_OBJECT_TYPE_SHADER_MODULE_EXT )
        CaseString( VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_CACHE_EXT )
        CaseString( VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_LAYOUT_EXT )
        CaseString( VK_DEBUG_REPORT_OBJECT_TYPE_RENDER_PASS_EXT )
        CaseString( VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_EXT )
        CaseString( VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT_EXT )
        CaseString( VK_DEBUG_REPORT_OBJECT_TYPE_SAMPLER_EXT )
        CaseString( VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_POOL_EXT )
        CaseString( VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_SET_EXT )
        CaseString( VK_DEBUG_REPORT_OBJECT_TYPE_FRAMEBUFFER_EXT )
        CaseString( VK_DEBUG_REPORT_OBJECT_TYPE_COMMAND_POOL_EXT )
        CaseString( VK_DEBUG_REPORT_OBJECT_TYPE_SURFACE_KHR_EXT )
        CaseString( VK_DEBUG_REPORT_OBJECT_TYPE_SWAPCHAIN_KHR_EXT )
        CaseString( VK_DEBUG_REPORT_OBJECT_TYPE_DEBUG_REPORT_EXT )
        default:
        {
            return "VK_DEBUG_REPORT_OBJECT_TYPE_UNKNOWN_EXT";
        }
    };

    // Should never reach this place
    assert( 0 );
    return nullptr; 
}   

VkBool32 vulkanDebugCallbackLogger(
    VkDebugReportFlagsEXT      flags,
    VkDebugReportObjectTypeEXT objectType,
    uint64_t                   object,
    size_t                     location,
    int32_t                    messageCode,
    const char*                pLayerPrefix,
    const char*                pMessage,
    void*                      pUserData)
{
    // Log callback event type
    std::string info = "Vulkan Debug Callback: ";
    if (flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT)
    {
        info += "INFO ";
    }
    if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
    {
        info += "ERROR ";
    }
    if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT)
    {
        info += "WARNING ";
    }
    if (flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT)
    {
        info += "PERFORMANCE ";
    }
    if (flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT)
    {
        info += "DEBUG";
    }
    info += "\n";
   
    // Object type
    info += "     Type: ";
    info += ObjectTypeString(objectType);
    info += "\n";

    // Object handle
    info += "   Handle: 0x";
    info += hexStringFrom(object);
    info += "\n";

    // Object location (pointer?)
    info += " Location: 0x";
    info += hexStringFrom(location);
    info += "\n";

    // Message code
    info += "     Code: ";
    info += stringFrom(messageCode);
    info += "\n";
    
    // Layer prefix
    info += "    Layer: ";
    info += std::string(pLayerPrefix);
    info += "\n";
    
    // Message
    info += "  Message: ";
    info += std::string(pMessage);
    info += "\n\n";
    
    Log << info.c_str();
    
    // Don't abort this function call (to maintain consistency with Release behavior)
    return VK_FALSE;
}

VulkanAPI::VulkanAPI(std::string appName) :
#if defined(EN_PLATFORM_WINDOWS)
    library(LoadLibrary(L"vulkan-1.dll")),
#endif
#if defined(EN_PLATFORM_LINUX)
    library(dlopen("libvulkan.so", RTLD_NOW)),
#endif
    layer(nullptr),
    layersCount(0),
    globalExtension(nullptr),
    globalExtensionsCount(0),
    devicesCount(0),
    CommonGraphicAPI()
{
    for(uint32 i=0; i<MaxSupportedThreads; ++i)
    {
        lastResult[i] = VK_SUCCESS;
    }

    // Verify load of Vulkan dynamic library
    if (library == nullptr)
    {
        std::string info;
        info += "ERROR: Vulkan error:\n";
        info += "       Cannot find Vulkan dynamic library.\n";
        info += "       Please check that your graphic card support Vulkan API and that you have latest graphic drivers installed.\n";
        Log << info.c_str();
        assert( 0 );

        // TODO: Terminate application in Release build
    }

    // Load using OS, main function pointer used to acquire other Vulkan function pointers
    LoadFunction( vkGetInstanceProcAddr )

    // Load Global Vulkan function pointers (GPU device independent)
    LoadGlobalFunction( vkEnumerateInstanceExtensionProperties )
    LoadGlobalFunction( vkEnumerateInstanceLayerProperties )
    LoadGlobalFunction( vkCreateInstance )
  
    // Gather available Vulkan Extensions
    //------------------------------------

    // Acquire list of Vulkan extensions 
    Validate( this, vkEnumerateInstanceExtensionProperties(nullptr, &globalExtensionsCount, nullptr) )
    if (IsWarning(lastResult[0]))
    {
        assert( 0 );
    }

    if (globalExtensionsCount > 0)
    {
        globalExtension = new VkExtensionProperties[globalExtensionsCount];
        Validate( this, vkEnumerateInstanceExtensionProperties(nullptr, &globalExtensionsCount, globalExtension) )
    }
      
    // Gather available Vulkan Instance Layers & Layers Extensions
    //-------------------------------------------------------------

    Validate( this, vkEnumerateInstanceLayerProperties(&layersCount, nullptr) )
    if (IsWarning(lastResult[0]))
    {
        assert( 0 );
    }

    // Acquire list of supported Vulkan Layers
    if (layersCount > 0)
    {
        VkLayerProperties* layerProperties = new VkLayerProperties[layersCount];
        assert( layerProperties );

        Validate( this, vkEnumerateInstanceLayerProperties(&layersCount, layerProperties) )
      
        layer = new LayerDescriptor[layersCount];

        // Acquire information about each Layer and supported extensions
        for(uint32 i=0; i<layersCount; ++i)
        {
            layer[i].properties = layerProperties[i];

            Validate( this, vkEnumerateInstanceExtensionProperties(layer[i].properties.layerName, &layer[i].extensionsCount, nullptr) )
            if (IsWarning(lastResult[0]))
            {
                assert( 0 );
            }

            if (layer[i].extensionsCount > 0)
            {
                layer[i].extension = new VkExtensionProperties[layer[i].extensionsCount];
                Validate( this, vkEnumerateInstanceExtensionProperties(layer[i].properties.layerName, &layer[i].extensionsCount, layer[i].extension) )
            }
        }

        delete [] layerProperties;
    }

    // Specify Vulkan Extensions to initialize
    //-----------------------------------------

    uint32 enabledExtensionsCount = 0;
    char** extensionPtrs = nullptr;
    extensionPtrs = new char*[globalExtensionsCount];
   
    // Adding Debug specific extensions to the list
#if defined(EN_DEBUG)
    extensionPtrs[enabledExtensionsCount++] = VK_EXT_DEBUG_REPORT_EXTENSION_NAME; 
#endif

    // Adding Windowing System Interface extensions to the list
    extensionPtrs[enabledExtensionsCount++] = VK_KHR_SURFACE_EXTENSION_NAME;
#if defined(EN_PLATFORM_ANDROID)
    extensionPtrs[enabledExtensionsCount++] = VK_KHR_ANDROID_SURFACE_EXTENSION_NAME;
#endif
#if defined(EN_PLATFORM_LINUX)
    // TODO: Pick one on Linux (as usual it's complete mess)
    extensionPtrs[enabledExtensionsCount++] = VK_KHR_XCB_SURFACE_EXTENSION_NAME;
    extensionPtrs[enabledExtensionsCount++] = VK_KHR_XLIB_SURFACE_EXTENSION_NAME;
    extensionPtrs[enabledExtensionsCount++] = VK_KHR_MIR_SURFACE_EXTENSION_NAME;
    extensionPtrs[enabledExtensionsCount++] = VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME;
#endif
#if defined(EN_PLATFORM_WINDOWS)
    extensionPtrs[enabledExtensionsCount++] = VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
#endif

    // TODO: Add here loading list of needed extensions from some config file.
    //       (generated automatically by the engine/editor based on features
    //        used by the app).

    // Verify selected extensions are available
    for(uint32 i=0; i<enabledExtensionsCount; ++i)
    {
        bool found = false;
        for(uint32 j=0; j<globalExtensionsCount; ++j)
        {
            if (strcmp(extensionPtrs[i], globalExtension[j].extensionName) == 0 )
            {
                found = true;
            }
        }
         
        if (!found)
        {
            Log << "ERROR: Requested Vulkan extension " << extensionPtrs[i] << " is not supported on this system!\n";
            Log << "       Supported extensions:\n";
            for(uint32 j=0; j<globalExtensionsCount; ++j)
            {
                Log << "       - " << globalExtension[j].extensionName << std::endl;
            }

            assert( 0 );
        }
    }
     
    // Specify Vulkan Layers and their Extensions to initialize
    //----------------------------------------------------------

    uint32 enabledLayersCount = 0;
    char** layersPtrs = nullptr;
   
#if defined(EN_DEBUG)
    if (layersCount > 0)
    {
        Log << "Available Vulkan Layers:\n";
        for(uint32 i=0; i<layersCount; ++i)  
        {
            Log << "  " << layer[i].properties.layerName << std::endl;
        }
    }

    // TODO: Read list of requested layers to enable from config file

    // Cannot enable more layers than all currently available
    layersPtrs = new char*[layersCount];

    // Layers available on machine with raw graphic driver
    // VK_LAYER_NV_optimus
    // VK_LAYER_VALVE_steam_overlay
    // VK_LAYER_LUNARG_standard_validation

    // Temporary WA to manually select layers to load
    //layersPtrs[enabledLayersCount++] = "VK_LAYER_LUNARG_api_dump";
    //layersPtrs[enabledLayersCount++] = "VK_LAYER_LUNARG_core_validation";
    //layersPtrs[enabledLayersCount++] = "VK_LAYER_LUNARG_image";
    //layersPtrs[enabledLayersCount++] = "VK_LAYER_LUNARG_object_tracker";
    //layersPtrs[enabledLayersCount++] = "VK_LAYER_LUNARG_parameter_validation";
    //layersPtrs[enabledLayersCount++] = "VK_LAYER_LUNARG_screenshot";
    //layersPtrs[enabledLayersCount++] = "VK_LAYER_LUNARG_swapchain";
    //layersPtrs[enabledLayersCount++] = "VK_LAYER_GOOGLE_threading";
    //layersPtrs[enabledLayersCount++] = "VK_LAYER_GOOGLE_unique_objects";
    //layersPtrs[enabledLayersCount++] = "VK_LAYER_LUNARG_vktrace";
    //layersPtrs[enabledLayersCount++] = "VK_LAYER_NV_optimus";
    //layersPtrs[enabledLayersCount++] = "VK_LAYER_RENDERDOC_Capture";
    //layersPtrs[enabledLayersCount++] = "VK_LAYER_VALVE_steam_overlay";
 
    // Deprecated:
    //layersPtrs[enabledLayersCount++] = "VK_LAYER_LUNARG_standard_validation";  // Meta-layer - Loads standard set of validation layers in optimal order (all of them in fact)
    // Use new:

    for (uint32 i=0; i<layersCount; ++i)
    {
        if (strcmp(layer[i].properties.layerName, "VK_LAYER_KHRONOS_validation") == 0)
        {
            layersPtrs[enabledLayersCount++] = "VK_LAYER_KHRONOS_validation";
            break;
        }
    }

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

    // Create Application Vulkan API Instance
    //----------------------------------------

    VkApplicationInfo appInfo;
    appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext              = nullptr;
    appInfo.pApplicationName   = appName.c_str();
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName        = "Ngine";
    appInfo.engineVersion      = VK_MAKE_VERSION(5, 0, 0);
    appInfo.apiVersion         = VK_API_VERSION_1_0;
   
    VkInstanceCreateInfo instInfo;
    instInfo.sType                     = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instInfo.pNext                     = nullptr;
    instInfo.flags                     = 0;        // VkInstanceCreateFlags - Reserved.
    instInfo.pApplicationInfo          = &appInfo;
    instInfo.enabledLayerCount         = enabledLayersCount;
    instInfo.ppEnabledLayerNames       = reinterpret_cast<const char*const*>(layersPtrs);
    instInfo.enabledExtensionCount     = enabledExtensionsCount;
    instInfo.ppEnabledExtensionNames   = reinterpret_cast<const char*const*>(extensionPtrs);

    // TODO: In the future provide our own allocation callbacks to track
    //       and analyze drivers system memory usage (&defaultAllocCallbacks).

    Validate( this, vkCreateInstance(&instInfo, nullptr, &instance) )

    // Bind Interface functions
    loadInterfaceFunctionPointers();
   
    delete [] extensionPtrs;
    delete [] layersPtrs;

    // Create Vulkan Devices
    //-----------------------

    // Enumerate available physical devices
    Validate( this, vkEnumeratePhysicalDevices(instance, &devicesCount, nullptr) )
    VkPhysicalDevice* deviceHandle = new VkPhysicalDevice[devicesCount];
    Validate( this, vkEnumeratePhysicalDevices(instance, &devicesCount, deviceHandle) )

    // Create interfaces for all available physical devices
    _device = new std::shared_ptr<VulkanDevice>[devicesCount];
    for(uint32 i=0; i<devicesCount; ++i)
    {
        _device[i] = std::make_shared<VulkanDevice>(this, i, deviceHandle[i]);
    }

    // Register debug callbacks
    //--------------------------

#if defined(EN_DEBUG)
    // Register callback for handling debug messages
    VkDebugReportCallbackCreateInfoEXT debugInfo;
    debugInfo.sType       = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
    debugInfo.pNext       = nullptr;
    debugInfo.flags       = VK_DEBUG_REPORT_ERROR_BIT_EXT;
    debugInfo.pfnCallback = vulkanDebugCallbackLogger;
    debugInfo.pUserData   = nullptr;

    Validate( this, vkCreateDebugReportCallbackEXT(instance, &debugInfo, nullptr, &debugCallbackHandle) )
#endif

    delete [] deviceHandle;
}

VulkanAPI::~VulkanAPI()
{
    // Remove debug callbacks (Error, Warning, Performance)
#if defined(EN_DEBUG)
    vkDestroyDebugReportCallbackEXT(instance, debugCallbackHandle, nullptr);
#endif

    // Release Vulkan Devices
    for(uint32 i = 0; i<devicesCount; ++i)
    {
        _device[i] = nullptr;
    }

    // Release Layers and Extensions information
    for(uint32 i=0; i<layersCount; ++i)
    {
        delete [] layer[i].extension;
    }

    delete [] layer;
    delete [] globalExtension;
   
    // Release Vulkan instance
    if (instance != VK_NULL_HANDLE)
    {
        ValidateNoRet( this, vkDestroyInstance(instance, nullptr) )
    }

    // Clear Vulkan function pointers
    clearInterfaceFunctionPointers();
   
    // Release Dynamically Linked Vulkan Library
    if (library)
    {
#if defined(EN_PLATFORM_WINDOWS)
        FreeLibrary(library);
#endif
#if defined(EN_PLATFORM_LINUX)
        dlclose(library);
#endif
    }
}

void VulkanAPI::loadInterfaceFunctionPointers(void)
{
    LoadInstanceFunction( vkEnumeratePhysicalDevices )
    LoadInstanceFunction( vkGetPhysicalDeviceFeatures )
    LoadInstanceFunction( vkGetPhysicalDeviceProperties )
    LoadInstanceFunction( vkGetPhysicalDeviceMemoryProperties )
    LoadInstanceFunction( vkGetPhysicalDeviceFormatProperties )
    LoadInstanceFunction( vkGetPhysicalDeviceImageFormatProperties )
    LoadInstanceFunction( vkGetPhysicalDeviceQueueFamilyProperties )
    LoadInstanceFunction( vkGetPhysicalDeviceSparseImageFormatProperties )
    LoadInstanceFunction( vkEnumerateDeviceExtensionProperties )
    LoadInstanceFunction( vkEnumerateDeviceLayerProperties )
    LoadInstanceFunction( vkCreateDevice )
    LoadInstanceFunction( vkGetDeviceProcAddr )
    LoadInstanceFunction( vkDestroyInstance )

    // VK_KHR_display
    LoadInstanceFunction( vkGetPhysicalDeviceDisplayPropertiesKHR )
    LoadInstanceFunction( vkGetPhysicalDeviceDisplayPlanePropertiesKHR )
    LoadInstanceFunction( vkGetDisplayPlaneSupportedDisplaysKHR )
    LoadInstanceFunction( vkGetDisplayModePropertiesKHR )
    LoadInstanceFunction( vkCreateDisplayModeKHR )
    LoadInstanceFunction( vkGetDisplayPlaneCapabilitiesKHR )
    LoadInstanceFunction( vkCreateDisplayPlaneSurfaceKHR )

    // VK_KHR_surface
    LoadInstanceFunction( vkGetPhysicalDeviceSurfaceSupportKHR )
    LoadInstanceFunction( vkGetPhysicalDeviceSurfaceCapabilitiesKHR )
    LoadInstanceFunction( vkGetPhysicalDeviceSurfaceFormatsKHR )
    LoadInstanceFunction( vkGetPhysicalDeviceSurfacePresentModesKHR )
    LoadInstanceFunction( vkDestroySurfaceKHR )
    
    // VK_KHR_win32_surface
    LoadInstanceFunction( vkCreateWin32SurfaceKHR )
    LoadInstanceFunction( vkGetPhysicalDeviceWin32PresentationSupportKHR )

    // VK_EXT_debug_report
#if defined(EN_DEBUG)
    LoadInstanceFunction( vkCreateDebugReportCallbackEXT )
    LoadInstanceFunction( vkDestroyDebugReportCallbackEXT )
    LoadInstanceFunction( vkDebugReportMessageEXT )
#endif
}

void VulkanAPI::clearInterfaceFunctionPointers(void)
{
    // OS Function Pointer
    vkGetInstanceProcAddr                          = nullptr;
    
    // Vulkan Function Pointers
    vkEnumerateInstanceExtensionProperties         = nullptr;
    vkEnumerateInstanceLayerProperties             = nullptr;
    vkCreateInstance                               = nullptr;
    
    // Vulkan Instance Function Pointers
    vkEnumeratePhysicalDevices                     = nullptr;
    vkGetPhysicalDeviceFeatures                    = nullptr;
    vkGetPhysicalDeviceProperties                  = nullptr;
    vkGetPhysicalDeviceMemoryProperties            = nullptr;
    vkGetPhysicalDeviceFormatProperties            = nullptr;
    vkGetPhysicalDeviceImageFormatProperties       = nullptr;
    vkGetPhysicalDeviceQueueFamilyProperties       = nullptr;
    vkGetPhysicalDeviceSparseImageFormatProperties = nullptr;
    vkEnumerateDeviceExtensionProperties           = nullptr;
    vkEnumerateDeviceLayerProperties               = nullptr;
    vkCreateDevice                                 = nullptr;
    vkGetDeviceProcAddr                            = nullptr;
    vkDestroyInstance                              = nullptr;
    vkGetPhysicalDeviceDisplayPropertiesKHR        = nullptr;
    vkGetPhysicalDeviceDisplayPlanePropertiesKHR   = nullptr;
    vkGetDisplayPlaneSupportedDisplaysKHR          = nullptr;
    vkGetDisplayModePropertiesKHR                  = nullptr;
    vkCreateDisplayModeKHR                         = nullptr;
    vkGetDisplayPlaneCapabilitiesKHR               = nullptr;
    vkCreateDisplayPlaneSurfaceKHR                 = nullptr;
    vkGetPhysicalDeviceSurfaceSupportKHR           = nullptr;
}
   
RenderingAPI VulkanAPI::type(void) const
{
    return RenderingAPI::Vulkan;
}

uint32 VulkanAPI::devices(void) const
{
    return devicesCount;
}
         
std::shared_ptr<GpuDevice> VulkanAPI::primaryDevice(void) const
{
    return _device[0];
}

std::shared_ptr<GpuDevice> VulkanAPI::device(const uint32 index) const
{
    assert( index < devicesCount );
   
    if (index >= devicesCount)
    {
        return nullptr;
    }

    return _device[index];
}

uint32 VulkanAPI::displays(void) const
{
    return displaysCount;
}
   
std::shared_ptr<Display> VulkanAPI::primaryDisplay(void) const
{
    return displayArray[0];
}
   
std::shared_ptr<Display> VulkanAPI::display(const uint32 index) const
{
    assert( index < displaysCount );
   
    if (index >= displaysCount)
    {
        return nullptr;
    }

    return displayArray[index];   
}


// TODO: Linux Window creation and bind !
//
//
//#ifdef EN_PLATFORM_WINDOWS
//    HINSTANCE connection;      // hInstance - Windows Instance
//    HWND      window;          // hWnd      - window handle
//#else
//    xcb_connection_t*        connection;
//    xcb_screen_t*            screen;
//    xcb_window_t             window;
//    xcb_intern_atom_reply_t* atom_wm_delete_window;
//    VkPlatformHandleXcbKHR   platformHandle;
//#endif
//
//   // Init connection
//#ifdef EN_PLATFORM_WINDOWS
//   connection = hInstance;
//#else
//   const xcb_setup_t*    setup;
//   xcb_screen_iterator_t iter;
//   int                   scr;
//   
//   connection = xcb_connect(nullptr, &scr);
//   if (demo->connection == nullptr) 
//      {
//      std::string info;
//      info += "ERROR: Vulkan error: ";
//      info += "       Cannot find a compatible Vulkan installable client driver (ICD).\n";
//      info += "       Please check that your graphic card support Vulkan API and that you have latest graphic drivers installed.\n";
//      Log << info.c_str();
//      exit(0);
//      }
//   
//   setup = xcb_get_setup(connection);
//   iter = xcb_setup_roots_iterator(setup);
//   while (scr-- > 0)
//       xcb_screen_next(&iter);
//   
//   screen = iter.data;
//#endif
//
//   // Create VulkanAPI object here
//   VulkanAPI context = new VulkanAPI();
//
//   // Create Window

//   // Construct the WSI surface description:
//   VkSurfaceDescriptionWindowKHR surfaceInfo;

//
//    // Iterate over each queue to learn whether it supports presenting to WSI:
//    VkBool32* supportsPresent = (VkBool32 *)malloc(demo->queue_count * sizeof(VkBool32));
//    for (i = 0; i < demo->queue_count; i++) {
//        demo->fpGetPhysicalDeviceSurfaceSupportKHR(demo->gpu, i, (VkSurfaceDescriptionKHR*) &surfaceInfo, &supportsPresent[i]);
//    }
//
//    // Search for a graphics and a present queue in the array of queue
//    // families, try to find one that supports both
//    uint32_t graphicsQueueNodeIndex = UINT32_MAX;
//    uint32_t presentQueueNodeIndex  = UINT32_MAX;
//    for (i = 0; i < demo->queue_count; i++) {
//        if ((demo->queue_props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) {
//            if (graphicsQueueNodeIndex == UINT32_MAX) {
//                graphicsQueueNodeIndex = i;
//            }
//            
//            if (supportsPresent[i] == VK_TRUE) {
//                graphicsQueueNodeIndex = i;
//                presentQueueNodeIndex = i;
//                break;
//            }
//        }
//    }
//    if (presentQueueNodeIndex == UINT32_MAX) {
//        // If didn't find a queue that supports both graphics and present, then
//        // find a separate present queue.
//        for (uint32_t i = 0; i < demo->queue_count; ++i) {
//            if (supportsPresent[i] == VK_TRUE) {
//                presentQueueNodeIndex = i;
//                break;
//            }
//        }
//    }
//    free(supportsPresent);
//
//    // Generate error if could not find both a graphics and a present queue
//    if (graphicsQueueNodeIndex == UINT32_MAX || presentQueueNodeIndex == UINT32_MAX) {
//        ERR_EXIT("Could not find a graphics and a present queue\n",
//                 "WSI Initialization Failure");
//    }
//
//    // TODO: Add support for separate queues, including presentation,
//    //       synchronization, and appropriate tracking for QueueSubmit
//    // While it is possible for an application to use a separate graphics and a
//    // present queues, this demo program assumes it is only using one:
//    if (graphicsQueueNodeIndex != presentQueueNodeIndex) {
//        ERR_EXIT("Could not find a common graphics and a present queue\n",
//                 "WSI Initialization Failure");
//    }
//
//    demo->graphics_queue_node_index = graphicsQueueNodeIndex;
//
//    err = vkGetDeviceQueue(demo->device, demo->graphics_queue_node_index,
//            0, &demo->queue);
//    assert(!err);
//
//    // Get the list of VkFormat's that are supported:
//    uint32_t formatCount;
//    err = demo->fpGetSurfaceFormatsKHR(demo->device,
//                                    (VkSurfaceDescriptionKHR *) &demo->surface_description,
//                                    &formatCount, NULL);
//    assert(!err);
//    VkSurfaceFormatKHR *surfFormats =
//        (VkSurfaceFormatKHR *)malloc(formatCount * sizeof(VkSurfaceFormatKHR));
//    err = demo->fpGetSurfaceFormatsKHR(demo->device,
//                                    (VkSurfaceDescriptionKHR *) &demo->surface_description,
//                                    &formatCount, surfFormats);
//    assert(!err);
//    // If the format list includes just one entry of VK_FORMAT_UNDEFINED,
//    // the surface has no preferred format.  Otherwise, at least one
//    // supported format will be returned.
//    if (formatCount == 1 && surfFormats[0].format == VK_FORMAT_UNDEFINED)
//    {
//        demo->format = VK_FORMAT_B8G8R8A8_UNORM;
//    }
//    else
//    {
//        assert(formatCount >= 1);
//        demo->format = surfFormats[0].format;
//    }
//    demo->color_space = surfFormats[0].colorSpace;
//
//    demo->quit = false;
//    demo->curFrame = 0;
//
//    // Get Memory information and properties
//    err = vkGetPhysicalDeviceMemoryProperties(demo->gpu, &demo->memory_properties);
//    assert(!err);
//
//
//
//   // Here Window is created and Vulkan context is bound to it.
//   // We can render.


} // en::gpu
} // en

#endif
