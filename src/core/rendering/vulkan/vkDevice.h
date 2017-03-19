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

#ifndef ENG_CORE_RENDERING_VULKAN_DEVICE
#define ENG_CORE_RENDERING_VULKAN_DEVICE

#include "core/rendering/vulkan/vulkan.h"

#if defined(EN_MODULE_RENDERER_VULKAN) 

#include <string>
#include "core/rendering/common/device.h"
#include "core/rendering/common/display.h"
#include "core/rendering/sampler.h"



#include "core/rendering/vulkan/vkInputLayout.h"
#include "core/rendering/vulkan/vkBlend.h"
#include "core/rendering/vulkan/vkRaster.h"
#include "core/rendering/vulkan/vkMultisampling.h"
#include "core/rendering/vulkan/vkViewport.h"
#include "core/rendering/vulkan/vkDepthStencil.h"
#include "core/rendering/vulkan/vkPipeline.h"


#ifdef EN_DEBUG
namespace en
{
   namespace gpu
   {
   extern bool IsError(const VkResult result);
   extern bool IsWarning(const VkResult result);
   }
}
#endif

// gpu     - pointer to class storing pointer to called function.
//           Should be VulkanDevice*, or VulkanGPU*.
// command - Vulkan API function call to execute.
//
// Result of function call is stored per GPU, per Thread.
//
#ifdef EN_DEBUG
   #ifdef EN_PROFILER_TRACE_GRAPHICS_API

   #define Profile( _gpu, command )                                                 \
           {                                                                        \
           uint32 thread = Scheduler.core();                                        \
           Log << "[" << setw(2) << thread << "] ";                                 \
           Log << "Vulkan GPU " << setbase(16) << _gpu << ": " << #command << endl; \
           _gpu->lastResult[thread] = _gpu->command;                                \
           if (en::gpu::IsError(_gpu->lastResult[thread]))                          \
              assert( 0 );                                                          \
           en::gpu::IsWarning(_gpu->lastResult[thread]);                            \
           }

   #define ProfileNoRet( _gpu, command )                                            \
           {                                                                        \
           uint32 thread = Scheduler.core();                                        \
           Log << "[" << setw(2) << thread << "] ";                                 \
           Log << "Vulkan GPU " << setbase(16) << _gpu << ": " << #command << endl; \
           _gpu->command;                                                           \
           }

   #else 

   #define Profile( _gpu, command )                                    \
           {                                                           \
           uint32 thread = Scheduler.core();                           \
           _gpu->lastResult[thread] = _gpu->command;                   \
           if (en::gpu::IsError(_gpu->lastResult[thread]))             \
              assert( 0 );                                             \
           en::gpu::IsWarning(_gpu->lastResult[thread]);               \
           }

   #define ProfileNoRet( _gpu, command )                               \
           _gpu->command;

   #endif
   
#else // Release

   #define Profile( _gpu, command )                                   \
           _gpu->lastResult[Scheduler.core()] = _gpu->command;

   #define ProfileNoRet( _gpu, command )                              \
           _gpu->command;

#endif



using namespace std;

namespace en
{
   namespace gpu
   {
#if defined(EN_PLATFORM_LINUX)
   #define LoadProcAddress dlsym
#endif

#if defined(EN_PLATFORM_WINDOWS)
   #define LoadProcAddress GetProcAddress
#endif

   #define DeclareFunction(function)                                             \
   PFN_##function function;

   class VulkanAPI;

   class VulkanDevice : public CommonDevice
      {
      public:
      VkResult                         lastResult[MaxSupportedWorkerThreads];
      VulkanAPI*                       api;          // Vulkan API (for Vulkan Instance calls)
      uint32                           index;        // This device number on the list
      VkDevice                         device;
      VkPhysicalDevice                 handle;
      VkPhysicalDeviceFeatures         features;
      VkPhysicalDeviceProperties       properties;
      VkPhysicalDeviceMemoryProperties memory;
      VkQueueFamilyProperties*         queueFamily;
      uint32                           queueFamiliesCount;
      uint32*                          queueFamilyIndices;
      uint32                           queuesCount[underlyingType(QueueType::Count)];
      uint32                           queueTypeToFamily[underlyingType(QueueType::Count)];
      volatile VkCommandPool           commandPool[MaxSupportedWorkerThreads][underlyingType(QueueType::Count)];
      VkExtensionProperties*           globalExtension;
      uint32                           globalExtensionsCount;

      VkAllocationCallbacks            defaultAllocCallbacks;
      VkPipelineCache                  pipelineCache;          // Shared between the threads. Reuses PSO's between app runs (HDD storage used).
      bool                             rebuildCache;           // Indicates if driver cache should be re-saved to disk
      uint64                           memoryRAM;
      uint64                           memoryDriver;


      // Command Buffers Management
      //----------------------------

      uint32             commandBuffersExecuting[MaxSupportedWorkerThreads];
      Ptr<CommandBuffer> commandBuffers[MaxSupportedWorkerThreads][MaxCommandBuffersExecuting];

      void addCommandBufferToQueue(Ptr<CommandBuffer> command);
      void clearCommandBuffersQueue(void);

      // Memory Management
      //-------------------

      // Engine declares ideal order and count of types it wants to use for each memory usage (it can exclude some types for some usages deliberately).
      // Then those initial lists are validated at runtime with available memory types reported by device.
      
      uint32 memoryTypePerUsageCount[4];                 // Count of found and used memory types in hierarchy for each usage.
      uint32 memoryTypePerUsage[4][VK_MAX_MEMORY_TYPES]; // Memory types ordered from best suitable to least suitable ones for given memory usage (Type index in memory array).

      void initMemoryManager();


      // Device Function Pointers
      #include "core/rendering/vulkan/vulkan10.h"

      // Helper functions
      void  loadDeviceFunctionPointers(void);
      void  clearDeviceFunctionPointers(void);
      void* loadPipelineCache(uint64& size);

      // CPU memory allocation for given GPU device control
      friend void* VKAPI_PTR defaultAlloc(
          void*                                       pUserData,
          size_t                                      size,
          size_t                                      alignment,
          VkSystemAllocationScope                     allocationScope);
      
      friend void* VKAPI_PTR defaultRealloc(
          void*                                       pUserData,
          void*                                       pOriginal,
          size_t                                      size,
          size_t                                      alignment,
          VkSystemAllocationScope                     allocationScope);
      
      friend void VKAPI_PTR defaultFree(
          void*                                       pUserData,
          void*                                       pMemory);
      
      friend void VKAPI_PTR defaultIntAlloc(
          void*                                       pUserData,
          size_t                                      size,
          VkInternalAllocationType                    allocationType,
          VkSystemAllocationScope                     allocationScope);
      
      friend void VKAPI_PTR defaultIntFree(
          void*                                       pUserData,
          size_t                                      size,
          VkInternalAllocationType                    allocationType,
          VkSystemAllocationScope                     allocationScope);

      public:
      VulkanDevice(VulkanAPI* api, const uint32 index, const VkPhysicalDevice handle);
     ~VulkanDevice();

      virtual void init(void);

      virtual uint32 displays(void) const;

      virtual Ptr<Display> display(uint32 index) const;

      virtual Ptr<Window> createWindow(const WindowSettings& settings, 
                                       const string title);



      virtual uint32 queues(const QueueType type) const;

      // Creates Command Buffer from the given Command Queue of given type.
      // When this buffer is commited for execution it will execute on that queue.
      virtual Ptr<CommandBuffer> createCommandBuffer(const QueueType type = QueueType::Universal,
                                                     const uint32 parentQueue = 0u);




      virtual Ptr<Heap>   createHeap(const MemoryUsage usage, const uint32 size);

      virtual Ptr<Sampler> createSampler(const SamplerState& state);
      
      virtual Ptr<Texture> createSharedTexture(Ptr<SharedSurface> backingSurface);

      virtual Ptr<Shader>  createShader(const ShaderStage stage,
                                        const string& source);


      virtual Ptr<Pipeline> createPipeline(const PipelineState& pipelineState);


      virtual Ptr<InputLayout> createInputLayout(const DrawableType primitiveType,
                                                 const uint32 controlPoints,
                                                 const uint32 usedAttributes,
                                                 const uint32 usedBuffers,
                                                 const AttributeDesc* attributes,
                                                 const BufferDesc* buffers);

      virtual Ptr<SetLayout> createSetLayout(const uint32 count, 
                                             const ResourceGroup* group,
                                             const ShaderStages stagesMask = ShaderStages::All);

      virtual Ptr<PipelineLayout> createPipelineLayout(const uint32 sets,
                                                       const Ptr<SetLayout>* set,
                                                       const uint32 immutableSamplers = 0u,
                                                       const Ptr<Sampler>* sampler = nullptr,
                                                       const ShaderStages stagesMask = ShaderStages::All);


      virtual Ptr<ColorAttachment> createColorAttachment(const Format format, 
                                                         const uint32 samples = 1u);

      virtual Ptr<DepthStencilAttachment> createDepthStencilAttachment(const Format depthFormat, 
                                                                       const Format stencilFormat = Format::Unsupported,
                                                                       const uint32 samples = 1u);

      virtual Ptr<RenderPass> createRenderPass(const Ptr<ColorAttachment> swapChainSurface,
                                               const Ptr<DepthStencilAttachment> depthStencil);

      virtual Ptr<RenderPass> createRenderPass(const uint32 attachments,
                                               const Ptr<ColorAttachment>* color,
                                               const Ptr<DepthStencilAttachment> depthStencil);

      virtual Ptr<Semaphore> createSemaphore(void);
         

        
      virtual Ptr<RasterState>        createRasterState(const RasterStateInfo& state);

      virtual Ptr<MultisamplingState> createMultisamplingState(const uint32 samples,
                                                               const bool enableAlphaToCoverage,
                                                               const bool enableAlphaToOne);

      virtual Ptr<DepthStencilState>  createDepthStencilState(const DepthStencilStateInfo& desc);

      virtual Ptr<BlendState>         createBlendState(const BlendStateInfo& state,
                                                       const uint32 attachments,
                                                       const BlendAttachmentInfo* color);

      virtual Ptr<ViewportState>      createViewportState(const uint32 count,
                                                          const ViewportStateInfo* viewports,
                                                          const ScissorStateInfo* scissors);
      };



   // Vulkan API Layer description
   struct LayerDescriptor
      {
      VkLayerProperties      properties;
      VkExtensionProperties* extension;
      uint32                 extensionsCount;

      LayerDescriptor();
      };
      
   // Vulkan API Interface
   class VulkanAPI : public GraphicAPI
      {
      public:
#if defined(EN_PLATFORM_WINDOWS)
      HMODULE                          library;    // Vulkan dynamic library handle.
#endif
#if defined(EN_PLATFORM_LINUX)
      void*                            library;
#endif
      VkResult                         lastResult[MaxSupportedWorkerThreads];
      LayerDescriptor*                 layer;
      uint32                           layersCount;
      VkExtensionProperties*           globalExtension;
      uint32                           globalExtensionsCount;

      VkInstance                       instance;    // Application Vulkan API Instance
      Ptr<VulkanDevice>*               device;      // Physical Device Interfaces
      uint32                           devicesCount;

      // API Independent, OS Dependent - Windowing System
      Ptr<CommonDisplay>*              displayArray;
      Ptr<CommonDisplay>               virtualDisplay;
      uint32                           displaysCount;
      uint32                           displayPrimary;
  
      // OS Function Pointer
      DeclareFunction( vkGetInstanceProcAddr )
   
      // Vulkan Function Pointers
      DeclareFunction( vkEnumerateInstanceExtensionProperties )
      DeclareFunction( vkEnumerateInstanceLayerProperties )
      DeclareFunction( vkCreateInstance )
   
      // Vulkan Instance Function Pointers
      DeclareFunction( vkEnumeratePhysicalDevices )
      DeclareFunction( vkGetPhysicalDeviceFeatures )
      DeclareFunction( vkGetPhysicalDeviceProperties )
      DeclareFunction( vkGetPhysicalDeviceMemoryProperties )
      DeclareFunction( vkGetPhysicalDeviceFormatProperties )
      DeclareFunction( vkGetPhysicalDeviceImageFormatProperties )
      DeclareFunction( vkGetPhysicalDeviceQueueFamilyProperties )
      DeclareFunction( vkEnumerateDeviceExtensionProperties )
      DeclareFunction( vkCreateDevice )
      DeclareFunction( vkGetDeviceProcAddr )
      DeclareFunction( vkDestroyInstance )

      // VK_KHR_surface
      DeclareFunction( vkGetPhysicalDeviceSurfaceSupportKHR )
      DeclareFunction( vkGetPhysicalDeviceSurfaceCapabilitiesKHR )
      DeclareFunction( vkGetPhysicalDeviceSurfaceFormatsKHR )
      DeclareFunction( vkGetPhysicalDeviceSurfacePresentModesKHR )
      DeclareFunction( vkDestroySurfaceKHR )

      // VK_KHR_win32_surface
      DeclareFunction( vkCreateWin32SurfaceKHR )
      DeclareFunction( vkGetPhysicalDeviceWin32PresentationSupportKHR )

      // VK_EXT_debug_report
      DeclareFunction( vkCreateDebugReportCallbackEXT )
      DeclareFunction( vkDestroyDebugReportCallbackEXT )
      DeclareFunction( vkDebugReportMessageEXT )

      public:
      VulkanAPI(string appName);
      virtual ~VulkanAPI();

      void loadInterfaceFunctionPointers(void);
      void clearInterfaceFunctionPointers(void);

      // TODO: Those could be moved to CommonGraphicAPI
      virtual uint32 devices(void) const;
      virtual Ptr<GpuDevice> primaryDevice(void) const;

      virtual uint32 displays(void) const;
      virtual Ptr<Display> primaryDisplay(void) const;
      virtual Ptr<Display> display(uint32 index) const;
      };
   }
}
#endif

#endif
