/*

 Ngine v5.0
 
 Module      : GPU Device.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#include "core/rendering/vulkan/vkDevice.h"

#if defined(EN_PLATFORM_ANDROID) || defined(EN_PLATFORM_WINDOWS)

#include "core/log/log.h"
#include "core/utilities/memory.h"

namespace en
{
   namespace gpu
   { 
   // Checks Vulkan error state
   bool IsError(const VkResult result)
   {
   sint32 value = static_cast<sint32>(result);
   if (value >= 0)
      return false;
   
   // Compose error message
   string info;
   info += "ERROR: Vulkan error: ";
   if (result == VK_ERROR_OUT_OF_HOST_MEMORY)
      info += "VK_ERROR_OUT_OF_HOST_MEMORY.\n";
   else
   if (result == VK_ERROR_OUT_OF_DEVICE_MEMORY)
      info += "VK_ERROR_OUT_OF_DEVICE_MEMORY.\n";
   else
   if (result == VK_ERROR_INITIALIZATION_FAILED)
      info += "VK_ERROR_INITIALIZATION_FAILED.\n";
   else
   if (result == VK_ERROR_DEVICE_LOST)
      info += "VK_ERROR_DEVICE_LOST.\n";
   else
   if (result == VK_ERROR_MEMORY_MAP_FAILED)
      info += "VK_ERROR_MEMORY_MAP_FAILED.\n";
   else
   if (result == VK_ERROR_LAYER_NOT_PRESENT)
      info += "VK_ERROR_LAYER_NOT_PRESENT.\n";
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
      return false;
   
   // Compose error message
   string info;
   info += "WARNING: Vulkan error: ";
   if (result == VK_NOT_READY)
      info += "VK_NOT_READY.\n";
   else
   if (result == VK_TIMEOUT)
      info += "VK_TIMEOUT.\n";
   else
   if (result == VK_EVENT_SET)
      info += "VK_EVENT_SET.\n";
   else
   if (result == VK_EVENT_RESET)
      info += "VK_EVENT_RESET.\n";
   else
   if (result == VK_INCOMPLETE)
      info += "VK_INCOMPLETE.\n";
   
   Log << info.c_str();
   return true; 
   }
      
   void unbindedVulkanFunctionHandler(...)
   {
   Log << "ERROR: Called unbinded Vulkan function.\n";
   assert(0);
   }

   // MACROS: Safe Vulkan function binding with fallback
   //         Both Macros should be used only inside VulkanAPI and VulkanDevice class methods.
   #define bindInstanceFunction(name)                                    \
   {                                                                     \
   vk##name = (PFN_vk##name) vkGetInstanceProcAddr(instance, "vk"#name); \
   if (!vk##name)                                                        \
      {                                                                  \
      vk##name = (PFN_vk##name) &unbindedVulkanFunctionHandler;          \
      Log << "Error: Cannot bind function vk" << #name << endl;          \
      };                                                                 \
   }

   #define bindDeviceFunction(name)                                      \
   {                                                                     \
   vk##name = (PFN_vk##name) vkGetDeviceProcAddr(device, "vk"#name);     \
   if (!vk##name)                                                        \
      {                                                                  \
      vk##name = (PFN_vk##name) &unbindedVulkanFunctionHandler;          \
      Log << "Error: Cannot bind function vk" << #name << endl;          \
      };                                                                 \
   }

   LayerDescriptor::LayerDescriptor() :
      extension(nullptr),
      extensionsCount(0)
   {
   }

   // CPU memory allocation for given GPU device control
   void* VKAPI_PTR defaultAlloc(
       void*                                       pUserData,
       size_t                                      size,
       size_t                                      alignment,
       VkSystemAllocationScope                     allocationScope)
   {
   void* ptr = reinterpret_cast<void*>(allocate<uint8>(alignment, size));
   if (ptr)
      {
      VulkanDevice* deviceVK = reinterpret_cast<VulkanDevice*>(pUserData);
      deviceVK->memoryRAM += size;
      }

   return ptr;
   }
   
   void* VKAPI_PTR defaultRealloc(
       void*                                       pUserData,
       void*                                       pOriginal,
       size_t                                      size,
       size_t                                      alignment,
       VkSystemAllocationScope                     allocationScope)
   {
   #ifdef EN_PLATFORM_WINDOWS
   return reinterpret_cast<void*>(reallocate<uint8>(reinterpret_cast<uint8*>(pOriginal), alignment, size));
   #else
   // THERE IS NO MEM ALIGNED REALLOC ON UNIX SYSTEMS !
   void* ptr = reinterpret_cast<void*>(allocate<uint8>(alignment, size));
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
       void*                                       pUserData,
       void*                                       pMemory)
   {
   deallocate<uint8>(reinterpret_cast<uint8*>(pMemory));

   VulkanDevice* deviceVK = reinterpret_cast<VulkanDevice*>(pUserData);
   // TODO: Needs to know size of original allocation !
   //deviceVK->memoryRAM -= oldSize;
   }
   
   void VKAPI_PTR defaultIntAlloc(
       void*                                       pUserData,
       size_t                                      size,
       VkInternalAllocationType                    allocationType,
       VkSystemAllocationScope                     allocationScope)
   {
   VulkanDevice* deviceVK = reinterpret_cast<VulkanDevice*>(pUserData);
   deviceVK->memoryDriver += size;
   }
   
   void VKAPI_PTR defaultIntFree(
       void*                                       pUserData,
       size_t                                      size,
       VkInternalAllocationType                    allocationType,
       VkSystemAllocationScope                     allocationScope)
   {
   VulkanDevice* deviceVK = reinterpret_cast<VulkanDevice*>(pUserData);
   deviceVK->memoryDriver -= size;
   }



   VulkanDevice::VulkanDevice(const VkPhysicalDevice _handle) :
      lastResult(VK_SUCCESS),
      handle(_handle),
      queueFamily(nullptr),
      queueFamiliesCount(0),
      layer(nullptr),
      layersCount(0),
      globalExtension(nullptr),
      globalExtensionsCount(0),
      memoryRAM(0),
      memoryDriver(0)
   {
   // Unbind all function pointers first
   unbindDeviceFunctionPointers();

   // Init default memory allocation callbacks
   defaultAllocCallbacks.pUserData             = this;
   defaultAllocCallbacks.pfnAllocation         = defaultAlloc;
   defaultAllocCallbacks.pfnReallocation       = defaultRealloc;
   defaultAllocCallbacks.pfnFree               = defaultFree;
   defaultAllocCallbacks.pfnInternalAllocation = defaultIntAlloc;
   defaultAllocCallbacks.pfnInternalFree       = defaultIntFree;

   // Gather Device Capabilities
   ProfileNoRet( vkGetPhysicalDeviceFeatures(handle, &features) )
   ProfileNoRet( vkGetPhysicalDeviceProperties(handle, &properties) )
   ProfileNoRet( vkGetPhysicalDeviceMemoryProperties(handle, &memory) )

    for(uint32_t i=0; i<memory.memoryHeapCount; ++i)
       {
       printf("Memory range %i:\n\n", i);
       printf("    Size          : %u\n", memory.memoryHeaps[i].size);
       printf("    Backing memory: %s\n", memory.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT ? "VRAM" : "RAM");
       printf("    Memory types  :\n");
       for(uint32_t j=0; j<memory.memoryTypeCount; ++j)
          if (memory.memoryTypes[j].heapIndex == i)
             {
             printf("    Type: %i\n", j);
             if (memory.memoryTypes[j].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
                printf("        Supports: VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT\n");
             if (memory.memoryTypes[j].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
                printf("        Supports: VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT\n");
             if (memory.memoryTypes[j].propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
                printf("        Supports: VK_MEMORY_PROPERTY_HOST_COHERENT_BIT\n");
             if (memory.memoryTypes[j].propertyFlags & VK_MEMORY_PROPERTY_HOST_CACHED_BIT)
                printf("        Supports: VK_MEMORY_PROPERTY_HOST_CACHED_BIT\n\n\n"); 
             if (memory.memoryTypes[j].propertyFlags & VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT)
                printf("        Supports: VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT\n\n\n"); 
             }

       }


 // TODO: Gather even more information
 //Profile( vkGetPhysicalDeviceFormatProperties(handle, VkFormat format, VkFormatProperties* pFormatProperties) )
 //Profile( vkGetPhysicalDeviceImageFormatProperties(handle, VkFormat format, VkImageType type, VkImageTiling tiling, VkImageUsageFlags usage, VkImageCreateFlags flags, VkImageFormatProperties* pImageFormatProperties) )

   // TODO: Populate "Support" section of CommonDevice
   // Texture
 //support.maxTextureSize1D     = properties.limits.maxImageDimension1D;
   support.maxTextureSize       = properties.limits.maxImageDimension2D;
   support.maxTextureRectSize   = properties.limits.maxImageDimension2D;     // TODO: There is no such thing in Vulkan
   support.maxTextureCubeSize   = properties.limits.maxImageDimensionCube; 
   support.maxTexture3DSize     = properties.limits.maxImageDimension3D;
   support.maxTextureLayers     = properties.limits.maxImageArrayLayers;          
   support.maxTextureBufferSize = properties.limits.maxImageDimension1D;     // TODO: Looks like there is no such thing in Vulkan
   support.maxTextureLodBias    = properties.limits.maxSamplerLodBias;            
   // Sampler
   support.maxAnisotropy        = properties.limits.maxSamplerAnisotropy;

   // Gather information about Queue Families supported by this device
   ProfileNoRet( vkGetPhysicalDeviceQueueFamilyProperties(handle, &queueFamiliesCount, nullptr) )
   queueFamily = new VkQueueFamilyProperties[queueFamiliesCount];
   ProfileNoRet( vkGetPhysicalDeviceQueueFamilyProperties(handle, &queueFamiliesCount, queueFamily) )

    //for(uint32_t family=0; family<queueCount; ++family)
    //   {
    //   printf("Queue Family %i:\n\n", family);

    //   printf("    Queues in Family: %i\n", queueProps[family].queueCount);
    //   printf("    Queue Min Transfer W: %i\n", queueProps[family].minImageTransferGranularity.width);
    //   printf("    Queue Min Transfer H: %i\n", queueProps[family].minImageTransferGranularity.height);
    //   printf("    Queue Min Transfer D: %i\n", queueProps[family].minImageTransferGranularity.depth);
    //   if (queueProps[family].queueFlags & VK_QUEUE_GRAPHICS_BIT)
    //      printf("    Queue supports: GRAPHICS\n");
    //   if (queueProps[family].queueFlags & VK_QUEUE_COMPUTE_BIT)
    //      printf("    Queue supports: COMPUTE\n");
    //   if (queueProps[family].queueFlags & VK_QUEUE_TRANSFER_BIT)
    //      printf("    Queue supports: TRANSFER\n");
    //   if (queueProps[family].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT)
    //      printf("    Queue supports: SPARSE_BINDING\n\n\n"); 
    //   printf("    Queues Time Stamp: %i\n", queueProps[family].timestampValidBits);  
    //   }

   VkLayerProperties* layerProperties = nullptr;

   // Acquire list of Vulkan Layers supported by this Device 
   lastResult = VK_INCOMPLETE;
   while(lastResult == VK_INCOMPLETE)
      {
      Profile( vkEnumerateDeviceLayerProperties(handle, &layersCount, nullptr) )
      if (IsWarning(lastResult))
         assert(0);

      if (layersCount == 0)
         break;

      // Allocate array of Layer Properties descriptors
      if (layerProperties)
         delete [] layerProperties;
      layerProperties = new VkLayerProperties[layersCount];
      Profile( vkEnumerateDeviceLayerProperties(handle, &layersCount, layerProperties) )
      }

   // Acquire information about each Layer and supported extensions
   layer = new LayerDescriptor[layersCount];
   for(uint32 i=0; i<layersCount; ++i)
      {
      layer[i].properties = layerProperties[i];

      // Acquire list of all extensions supported by this layer
      lastResult = VK_INCOMPLETE;
      while(lastResult == VK_INCOMPLETE)
         {
         Profile( vkEnumerateDeviceExtensionProperties(handle, layer[i].properties.layerName, &layer[i].extensionsCount, nullptr) )
         if (IsWarning(lastResult))
            assert(0);
      
         if (layer[i].extensionsCount == 0)
            break;
      
         // Allocate array of Layer Extension descriptors
         if (layer[i].extension)
            delete [] layer[i].extension;
         layer[i].extension = new VkExtensionProperties[layer[i].extensionsCount];
         Profile( vkEnumerateDeviceExtensionProperties(handle, layer[i].properties.layerName, &layer[i].extensionsCount, layer[i].extension) )
         }
      }

   delete [] layerProperties;

   // Acquire list of all global extensions not being part of any layer
   lastResult = VK_INCOMPLETE;
   while(lastResult == VK_INCOMPLETE)
      {
      Profile( vkEnumerateDeviceExtensionProperties(handle, nullptr, &globalExtensionsCount, nullptr) )
      if (IsWarning(lastResult))
         assert(0);
   
      if (globalExtensionsCount == 0)
         break;

      if (globalExtension)
         delete [] globalExtension;
      globalExtension = new VkExtensionProperties[globalExtensionsCount];
      Profile( vkEnumerateDeviceExtensionProperties(handle, nullptr, &globalExtensionsCount, globalExtension) )
      }

   // While creating device, we can choose to init as many Queue Families as we want (but each only once).
   // In each Queue Family we can specify how many Queues we want to init.
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
         priorities[priority] = 1.0f;

      queueFamilyInfo[i].pQueuePriorities   = priorities;
      }

   // Similarly to Queue Families, for now on, lets init all Layers and all Extensions available

   // Create array of pointers to all layer names
   char** layersPtrs = new char*[layersCount];
   uint32 index = 0;
   for(uint32 i=0; i<layersCount; ++i)  
      layersPtrs[index] = &layer[i].properties.layerName[0];

   // Calculate total amount of extensions supported by all Layers
   uint32 totalExtensionsCount = globalExtensionsCount;
   for(uint32 i=0; i<layersCount; ++i)
      totalExtensionsCount += layer[i].extensionsCount;  

   // Create array of pointers to this extension names
   char** extensionPtrs = new char*[totalExtensionsCount];
   index = 0;
   for(uint32 i=0; i<globalExtensionsCount; ++i, ++index)
      extensionPtrs[index] = &globalExtension[i].extensionName[0];
   for(uint32 i=0; i<layersCount; ++i)
      for(uint32 j=0; j<layer[i].extensionsCount; ++j, ++index)   
         extensionPtrs[index] = &layer[i].extension[j].extensionName[0];

   // Create Device Interface
   VkDeviceCreateInfo deviceInfo;
   deviceInfo.sType                     = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
   deviceInfo.pNext                     = nullptr;
   deviceInfo.flags                     = 0;                     // Reserved
   deviceInfo.queueCreateInfoCount      = queueFamiliesCount;
   deviceInfo.pQueueCreateInfos         = queueFamilyInfo;
   deviceInfo.enabledLayerCount         = layersCount;
   deviceInfo.ppEnabledLayerNames       = layersCount          ? reinterpret_cast<const char*const*>(layersPtrs) : nullptr;
   deviceInfo.enabledExtensionCount     = totalExtensionsCount;
   deviceInfo.ppEnabledExtensionNames   = totalExtensionsCount ? reinterpret_cast<const char*const*>(extensionPtrs) : nullptr;
   deviceInfo.pEnabledFeatures          = nullptr;

   Profile( vkCreateDevice(handle, &deviceInfo, &defaultAllocCallbacks, &device) )

   // Bind Device Functions
   bindDeviceFunctionPointers();

   // Create Pipeline Cache

   // TODO: VkPipelineCache  pipelineCache;


   // Free temporary resources
   delete [] extensionPtrs;
   delete [] layersPtrs;
   }

   VulkanDevice::~VulkanDevice()
   {
   delete [] queueFamily;
   for(uint32 i=0; i<layersCount; ++i)
      delete [] layer[i].extension;
   delete [] layer;
   delete [] globalExtension;
   }

   void VulkanDevice::bindDeviceFunctionPointers(void)
   {
   // TODO: Order it by extensions etc.

   // Windows WA - Undefine Windows API defines
#ifdef CreateSemaphore
#undef CreateSemaphore
#endif
#ifdef CreateEvent
#undef CreateEvent
#endif

   // Vulkan 1.0
   //
#if 0 // TODO: Do we need to bind device functions, or are they auto init by loader ??
   bindDeviceFunction( DestroyInstance );
   bindDeviceFunction( GetPhysicalDeviceFeatures );
   bindDeviceFunction( GetPhysicalDeviceFormatProperties );
   bindDeviceFunction( GetPhysicalDeviceImageFormatProperties );
   bindDeviceFunction( GetPhysicalDeviceProperties );
   bindDeviceFunction( GetPhysicalDeviceQueueFamilyProperties );
   bindDeviceFunction( GetPhysicalDeviceMemoryProperties );
   bindDeviceFunction( GetDeviceProcAddr );
   bindDeviceFunction( CreateDevice );
   bindDeviceFunction( DestroyDevice );
   bindDeviceFunction( EnumerateDeviceExtensionProperties );
   bindDeviceFunction( EnumerateDeviceLayerProperties );
   bindDeviceFunction( GetDeviceQueue );
   bindDeviceFunction( QueueSubmit );
   bindDeviceFunction( QueueWaitIdle );
   bindDeviceFunction( DeviceWaitIdle );
   bindDeviceFunction( AllocateMemory );
   bindDeviceFunction( FreeMemory );
   bindDeviceFunction( MapMemory );
   bindDeviceFunction( UnmapMemory );
   bindDeviceFunction( FlushMappedMemoryRanges );
   bindDeviceFunction( InvalidateMappedMemoryRanges );
   bindDeviceFunction( GetDeviceMemoryCommitment );
   bindDeviceFunction( BindBufferMemory );
   bindDeviceFunction( BindImageMemory );
   bindDeviceFunction( GetBufferMemoryRequirements );
   bindDeviceFunction( GetImageMemoryRequirements );
   bindDeviceFunction( GetImageSparseMemoryRequirements );
   bindDeviceFunction( GetPhysicalDeviceSparseImageFormatProperties );
   bindDeviceFunction( QueueBindSparse );
   bindDeviceFunction( CreateFence );
   bindDeviceFunction( DestroyFence );
   bindDeviceFunction( ResetFences );
   bindDeviceFunction( GetFenceStatus );
   bindDeviceFunction( WaitForFences );
   bindDeviceFunction( CreateSemaphore );
   bindDeviceFunction( DestroySemaphore );
   bindDeviceFunction( CreateEvent );
   bindDeviceFunction( DestroyEvent );
   bindDeviceFunction( GetEventStatus );
   bindDeviceFunction( SetEvent );
   bindDeviceFunction( ResetEvent );
   bindDeviceFunction( CreateQueryPool );
   bindDeviceFunction( DestroyQueryPool );
   bindDeviceFunction( GetQueryPoolResults );
   bindDeviceFunction( CreateBuffer );
   bindDeviceFunction( DestroyBuffer );
   bindDeviceFunction( CreateBufferView );
   bindDeviceFunction( DestroyBufferView );
   bindDeviceFunction( CreateImage );
   bindDeviceFunction( DestroyImage );
   bindDeviceFunction( GetImageSubresourceLayout );
   bindDeviceFunction( CreateImageView );
   bindDeviceFunction( DestroyImageView );
   bindDeviceFunction( CreateShaderModule );
   bindDeviceFunction( DestroyShaderModule );
   bindDeviceFunction( CreatePipelineCache );
   bindDeviceFunction( DestroyPipelineCache );
   bindDeviceFunction( GetPipelineCacheData );
   bindDeviceFunction( MergePipelineCaches );
   bindDeviceFunction( CreateGraphicsPipelines );
   bindDeviceFunction( CreateComputePipelines );
   bindDeviceFunction( DestroyPipeline );
   bindDeviceFunction( CreatePipelineLayout );
   bindDeviceFunction( DestroyPipelineLayout );
   bindDeviceFunction( CreateSampler );
   bindDeviceFunction( DestroySampler );
   bindDeviceFunction( CreateDescriptorSetLayout );
   bindDeviceFunction( DestroyDescriptorSetLayout );
   bindDeviceFunction( CreateDescriptorPool );
   bindDeviceFunction( DestroyDescriptorPool );
   bindDeviceFunction( ResetDescriptorPool );
   bindDeviceFunction( AllocateDescriptorSets );
   bindDeviceFunction( FreeDescriptorSets );
   bindDeviceFunction( UpdateDescriptorSets );
   bindDeviceFunction( CreateFramebuffer );
   bindDeviceFunction( DestroyFramebuffer );
   bindDeviceFunction( CreateRenderPass );
   bindDeviceFunction( DestroyRenderPass );
   bindDeviceFunction( GetRenderAreaGranularity );
   bindDeviceFunction( CreateCommandPool );
   bindDeviceFunction( DestroyCommandPool );
   bindDeviceFunction( ResetCommandPool );
   bindDeviceFunction( AllocateCommandBuffers );
   bindDeviceFunction( FreeCommandBuffers );
   bindDeviceFunction( BeginCommandBuffer );
   bindDeviceFunction( EndCommandBuffer );
   bindDeviceFunction( ResetCommandBuffer );
   bindDeviceFunction( CmdBindPipeline );
   bindDeviceFunction( CmdSetViewport );
   bindDeviceFunction( CmdSetScissor );
   bindDeviceFunction( CmdSetLineWidth );
   bindDeviceFunction( CmdSetDepthBias );
   bindDeviceFunction( CmdSetBlendConstants );
   bindDeviceFunction( CmdSetDepthBounds );
   bindDeviceFunction( CmdSetStencilCompareMask );
   bindDeviceFunction( CmdSetStencilWriteMask );
   bindDeviceFunction( CmdSetStencilReference );
   bindDeviceFunction( CmdBindDescriptorSets );
   bindDeviceFunction( CmdBindIndexBuffer );
   bindDeviceFunction( CmdBindVertexBuffers );
   bindDeviceFunction( CmdDraw );
   bindDeviceFunction( CmdDrawIndexed );
   bindDeviceFunction( CmdDrawIndirect );
   bindDeviceFunction( CmdDrawIndexedIndirect );
   bindDeviceFunction( CmdDispatch );
   bindDeviceFunction( CmdDispatchIndirect );
   bindDeviceFunction( CmdCopyBuffer );
   bindDeviceFunction( CmdCopyImage );
   bindDeviceFunction( CmdBlitImage );
   bindDeviceFunction( CmdCopyBufferToImage );
   bindDeviceFunction( CmdCopyImageToBuffer );
   bindDeviceFunction( CmdUpdateBuffer );
   bindDeviceFunction( CmdFillBuffer );
   bindDeviceFunction( CmdClearColorImage );
   bindDeviceFunction( CmdClearDepthStencilImage );
   bindDeviceFunction( CmdClearAttachments );
   bindDeviceFunction( CmdResolveImage );
   bindDeviceFunction( CmdSetEvent );
   bindDeviceFunction( CmdResetEvent );
   bindDeviceFunction( CmdWaitEvents );
   bindDeviceFunction( CmdPipelineBarrier );
   bindDeviceFunction( CmdBeginQuery );
   bindDeviceFunction( CmdEndQuery );
   bindDeviceFunction( CmdResetQueryPool );
   bindDeviceFunction( CmdWriteTimestamp );
   bindDeviceFunction( CmdCopyQueryPoolResults );
   bindDeviceFunction( CmdPushConstants );
   bindDeviceFunction( CmdBeginRenderPass );
   bindDeviceFunction( CmdNextSubpass );
   bindDeviceFunction( CmdEndRenderPass );
   bindDeviceFunction( CmdExecuteCommands );
   bindDeviceFunction( CreateSwapchainKHR );
   bindDeviceFunction( DestroySwapchainKHR );
   bindDeviceFunction( GetSwapchainImagesKHR );
   bindDeviceFunction( AcquireNextImageKHR );
   bindDeviceFunction( QueuePresentKHR );
#endif
   }

   void VulkanDevice::unbindDeviceFunctionPointers(void)
   {
   // Vulkan 1.0
   //
#if 0 // TODO: Do we need to bind device functions, or are they auto init by loader ??
   vkCreateInstance                               = nullptr;
   vkDestroyInstance                              = nullptr;
   vkEnumeratePhysicalDevices                     = nullptr;
   vkGetPhysicalDeviceFeatures                    = nullptr;
   vkGetPhysicalDeviceFormatProperties            = nullptr;
   vkGetPhysicalDeviceImageFormatProperties       = nullptr;
   vkGetPhysicalDeviceProperties                  = nullptr;
   vkGetPhysicalDeviceQueueFamilyProperties       = nullptr;
   vkGetPhysicalDeviceMemoryProperties            = nullptr;
   vkGetDeviceProcAddr                            = nullptr;
   vkCreateDevice                                 = nullptr;
   vkDestroyDevice                                = nullptr;
   vkEnumerateInstanceExtensionProperties         = nullptr;
   vkEnumerateDeviceExtensionProperties           = nullptr;
   vkEnumerateInstanceLayerProperties             = nullptr;
   vkEnumerateDeviceLayerProperties               = nullptr;
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
   vkGetPhysicalDeviceSparseImageFormatProperties = nullptr;
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
#endif
   }








   VulkanAPI::VulkanAPI(string appName) :
      lastResult(VK_SUCCESS),
      layer(nullptr),
      layersCount(0),
      globalExtension(nullptr),
      globalExtensionsCount(0),
      devicesCount(0)
   {
   // TODO: Get pointer to fuction needed to acquire other function pointers
   // vkGetInstanceProcAddr = 
   bindInstanceFunction( EnumerateInstanceExtensionProperties );
   bindInstanceFunction( EnumerateInstanceLayerProperties );
   bindInstanceFunction( CreateInstance );
   bindInstanceFunction( EnumeratePhysicalDevices );

   VkLayerProperties* layerProperties = nullptr;

   // Acquire list of supported Vulkan Layers
   lastResult = VK_INCOMPLETE;
   while(lastResult == VK_INCOMPLETE)
      {
      Profile( vkEnumerateInstanceLayerProperties(&layersCount, nullptr) )
      if (IsWarning(lastResult))
         assert(0);

      if (layersCount == 0)
         break;

      // Allocate array of Layer Properties descriptors
      if (layerProperties)
         delete [] layerProperties;
      layerProperties = new VkLayerProperties[layersCount];
      Profile( vkEnumerateInstanceLayerProperties(&layersCount, layerProperties) )
      }
   layer = new LayerDescriptor[layersCount];

   // Acquire information about each Layer and supported extensions
   for(uint32 i=0; i<layersCount; ++i)
      {
      layer[i].properties = layerProperties[i];

      // Acquire list of all extensions supported by this layer
      lastResult = VK_INCOMPLETE;
      while(lastResult == VK_INCOMPLETE)
         {
         Profile( vkEnumerateInstanceExtensionProperties(layer[i].properties.layerName, &layer[i].extensionsCount, nullptr) )
         if (IsWarning(lastResult))
            assert(0);
      
         if (layer[i].extensionsCount == 0)
            break;
      
         // Allocate array of Layer Extension descriptors
         if (layer[i].extension)
            delete [] layer[i].extension;
         layer[i].extension = new VkExtensionProperties[layer[i].extensionsCount];
         Profile( vkEnumerateInstanceExtensionProperties(layer[i].properties.layerName, &layer[i].extensionsCount, layer[i].extension) )
         }
      }

   delete [] layerProperties;

   // Acquire list of all global extensions not being part of any layer
   lastResult = VK_INCOMPLETE;
   while(lastResult == VK_INCOMPLETE)
      {
      Profile( vkEnumerateInstanceExtensionProperties(nullptr, &globalExtensionsCount, nullptr) )
      if (IsWarning(lastResult))
         assert(0);
   
      if (globalExtensionsCount == 0)
         break;

      if (globalExtension)
         delete [] globalExtension;
      globalExtension = new VkExtensionProperties[globalExtensionsCount];
      Profile( vkEnumerateInstanceExtensionProperties(nullptr, &globalExtensionsCount, globalExtension) )
      }

   // Choose Layers and extensions that will be enabled for Vulkan interface.
   // For now on lets enable all Layers and only needed extensions.

   // Create array of pointers to all layer names
   uint32 enabledLayersCount = layersCount;
   char** layersPtrs = new char*[layersCount];
   for(uint32 i=0; i<layersCount; ++i)  
      layersPtrs[i] = &layer[i].properties.layerName[0];

   // Enable WSI SwapChain extension
   uint32 enabledExtensionsCount = 0;
   char** extensionPtrs = new char*[globalExtensionsCount];
   for(uint32 i=0; i<globalExtensionsCount; ++i)
      if (strcmp("VK_EXT_KHR_swapchain", globalExtension[i].extensionName) == 0 )
         extensionPtrs[enabledExtensionsCount++] = &globalExtension[i].extensionName[0];

   // We need at least SwapChain waorking to display anything
   if (!enabledExtensionsCount)
      {
      string info;
      info += "ERROR: Vulkan error: ";
      info += "       Cannot find a compatible Vulkan installable client driver (ICD).\n";
      info += "       Please check that your graphic card support Vulkan API and that you have latest graphic drivers installed.\n";
      Log << info.c_str();
      assert(0);
      }

   // This code would enable all Layers and all Extensions (lots of stuff)
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

   // Create Application API Instance
   VkApplicationInfo appInfo;
   appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
   appInfo.pNext              = nullptr;
   appInfo.pApplicationName   = appName.c_str();
   appInfo.applicationVersion = 1;
   appInfo.pEngineName        = "Ngine";
   appInfo.engineVersion      = 5;
   appInfo.apiVersion         = VK_API_VERSION;
   
   VkInstanceCreateInfo instInfo;
   instInfo.sType                     = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
   instInfo.pNext                     = nullptr;
   instInfo.flags                     = 0;                     // TODO: VkInstanceCreateFlags
   instInfo.pApplicationInfo          = &appInfo;
   instInfo.enabledLayerCount         = enabledLayersCount;
   instInfo.ppEnabledLayerNames       = enabledLayersCount     ? reinterpret_cast<const char*const*>(layersPtrs) : nullptr;
   instInfo.enabledExtensionCount     = enabledExtensionsCount;
   instInfo.ppEnabledExtensionNames   = enabledExtensionsCount ? reinterpret_cast<const char*const*>(extensionPtrs) : nullptr;

   // TODO: Needs to have separate aloc callbacks for API
   //Profile( vkCreateInstance(&instInfo, &defaultAllocCallbacks, &instance) )

   // Enumerate available physical devices
   Profile( vkEnumeratePhysicalDevices(instance, &devicesCount, nullptr) )
   VkPhysicalDevice* tempHandle = new VkPhysicalDevice[devicesCount];
   Profile( vkEnumeratePhysicalDevices(instance, &devicesCount, tempHandle) )

   // Create interfaces for all available physical devices
   device = new Ptr<VulkanDevice>[devicesCount];
   for(uint32 i=0; i<devicesCount; ++i)
      device[i] = Ptr<VulkanDevice>(new VulkanDevice(*tempHandle));
 
   // Bind Interface functions
   bindInterfaceFunctionPointers();

   delete [] tempHandle;
   }

   VulkanAPI::~VulkanAPI()
   {
   vkGetInstanceProcAddr                          = nullptr;
   vkEnumerateInstanceExtensionProperties         = nullptr;
   vkEnumerateInstanceLayerProperties             = nullptr;
   vkCreateInstance                               = nullptr;
   vkEnumeratePhysicalDevices                     = nullptr;

   vkGetPhysicalDeviceSurfaceSupportKHR           = nullptr;

   for(uint32 i=0; i<layersCount; ++i)
      delete [] layer[i].extension;
   delete [] layer;
   delete [] globalExtension;
   }



   void VulkanAPI::bindInterfaceFunctionPointers(void)
   {
   bindInstanceFunction( GetPhysicalDeviceSurfaceSupportKHR );
   }



// TODO: Window creation and bind !
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
//      string info;
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
//    WNDCLASSEX  win_class;
//
//    // Initialize the window class structure:
//    win_class.cbSize = sizeof(WNDCLASSEX);
//    win_class.style = CS_HREDRAW | CS_VREDRAW;
//    win_class.lpfnWndProc = WndProc;
//    win_class.cbClsExtra = 0;
//    win_class.cbWndExtra = 0;
//    win_class.hInstance = demo->connection; // hInstance
//    win_class.hIcon = LoadIcon(NULL, IDI_APPLICATION);
//    win_class.hCursor = LoadCursor(NULL, IDC_ARROW);
//    win_class.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
//    win_class.lpszMenuName = NULL;
//    win_class.lpszClassName = demo->name;
//    win_class.hIconSm = LoadIcon(NULL, IDI_WINLOGO);
//    // Register window class:
//    if (!RegisterClassEx(&win_class)) {
//        // It didn't work, so try to give a useful error:
//        printf("Unexpected error trying to start the application!\n");
//        fflush(stdout);
//        exit(1);
//    }
//    // Create window with the registered class:
//    RECT wr = { 0, 0, demo->width, demo->height };
//    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);
//    demo->window = CreateWindowEx(0,
//                                  demo->name,           // class name
//                                  demo->name,           // app name
//                                  WS_OVERLAPPEDWINDOW | // window style
//                                  WS_VISIBLE |
//                                  WS_SYSMENU,
//                                  100,100,              // x/y coords
//                                  wr.right-wr.left,     // width
//                                  wr.bottom-wr.top,     // height
//                                  NULL,                 // handle to parent
//                                  NULL,                 // handle to menu
//                                  demo->connection,     // hInstance
//                                  NULL);                // no extra parameters
//    if (!demo->window) {
//        // It didn't work, so try to give a useful error:
//        printf("Cannot create a window in which to draw!\n");
//        fflush(stdout);
//        exit(1);
//    }
//
//
//   // Attach Vulkan to Window (WSI)
//    VkResult err;
//    uint32_t i;
//
//
//
//   // Construct the WSI surface description:
//   VkSurfaceDescriptionWindowKHR surfaceInfo;
//   surfaceInfo.sType           = VK_STRUCTURE_TYPE_SURFACE_DESCRIPTION_WINDOW_KHR;
//   surfaceInfo.pNext           = nullptr;
//#ifdef _WIN32
//   surfaceInfo.platform        = VK_PLATFORM_WIN32_KHR;
//   surfaceInfo.pPlatformHandle = demo->connection;
//   surfaceInfo.pPlatformWindow = demo->window;
//#else  // _WIN32
//   platformHandle.connection = connection;
//   platformHandle.root       = screen->root;
//
//   surfaceInfo.platform        = VK_PLATFORM_XCB_KHR;
//   surfaceInfo.pPlatformHandle = &platformHandle;
//   surfaceInfo.pPlatformWindow = &window;
//#endif // _WIN32
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


   }
}
#endif