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

#if defined(EN_PLATFORM_ANDROID) || defined(EN_PLATFORM_WINDOWS)

#include <string>
#include "core/rendering/common/device.h"
#include "core/rendering/sampler.h"



#include "core/rendering/vulkan/vkInputAssembler.h"
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
           lastResult[Scheduler.core()] = _gpu->command;

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


   














/// TEMP START

   class PipelineLayout : public SafeObject<PipelineLayout>
      {
      public:
      virtual ~PipelineLayout();                              // Polymorphic deletes require a virtual base destructor
      };

   class PipelineLayoutVK : public PipelineLayout
      {
      public:
      VkPipelineLayout state;

      PipelineLayoutVK();
      };

/// TEMP END

























   // TODO: Move it to Thread Pool Scheduler
   #define MaxSupportedWorkerThreads 64
   
   class VulkanAPI;

   class VulkanDevice : public CommonDevice
      {
      public:
      VkResult                         lastResult[MaxSupportedWorkerThreads];
      VulkanAPI*                       api;          // Vulkan API (for Vulkan Instance calls)
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
      VkPipelineCache                  pipelineCache;
      uint64                           memoryRAM;
      uint64                           memoryDriver;

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
      void loadDeviceFunctionPointers(void);
      void clearDeviceFunctionPointers(void);

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
      VulkanDevice(VulkanAPI* api, const VkPhysicalDevice handle);
     ~VulkanDevice();


      //bool getMemoryType(uint32 allowedTypes, VkFlags properties, uint32* memoryType);
      //VkDeviceMemory allocMemory(VkMemoryRequirements requirements, VkFlags properties);






      virtual uint32 displays(void) const;

      virtual Ptr<Display> display(uint32 index) const;

      virtual Ptr<Window> create(const WindowSettings& settings, 
                                 const string title);



      virtual uint32 queues(const QueueType type) const;

      // Creates Command Buffer from the given Command Queue of given type.
      // When this buffer is commited for execution it will execute on that queue.
      virtual Ptr<CommandBuffer> createCommandBuffer(const QueueType type = QueueType::Universal,
                                                     const uint32 parentQueue = 0u) = 0;




      virtual Ptr<Heap>   create(const MemoryUsage usage, 
                                 const uint32 size);

      virtual Ptr<Sampler>  create(const SamplerState& state);

      virtual Ptr<Pipeline> create(const Ptr<InputAssembler> inputAssembler,
                                   const Ptr<ViewportState>  viewportState,
                                   const Ptr<RasterState>    rasterState,
                                   const Ptr<MultisamplingState> multisamplingState,
                                   const Ptr<DepthStencilState> depthStencilState,
                                   const Ptr<BlendState>     blendState,
                                   const Ptr<PipelineLayout> pipelineLayout);

      virtual Ptr<InputAssembler> create(const DrawableType primitiveType,
                                         const uint32 controlPoints,
                                         const Ptr<Buffer> buffer);

      virtual Ptr<InputAssembler> create(const DrawableType primitiveType,
                                         const uint32 controlPoints,
                                         const uint32 usedAttributes,
                                         const uint32 usedBuffers,
                                         const AttributeDesc* attributes,
                                         const BufferDesc* buffers);



      virtual Ptr<ColorAttachment> createColorAttachment(const Ptr<TextureView> textureView) = 0;

      virtual Ptr<DepthStencilAttachment> createDepthStencilAttachment(const Ptr<TextureView> depth,
                                                                       const Ptr<TextureView> stencil) = 0;

      // Creates simple render pass with one color destination
      virtual Ptr<RenderPass> create(const Ptr<ColorAttachment> color,
                                     const Ptr<DepthStencilAttachment> depthStencil);
      
      virtual Ptr<RenderPass> create(const uint32 _attachments,
                                     const Ptr<ColorAttachment> color[MaxColorAttachmentsCount],
                                     const Ptr<DepthStencilAttachment> depthStencil);
        
      // Creates render pass which's output goes to window framebuffer
      virtual Ptr<RenderPass> create(const Ptr<Texture> framebuffer,
                                     const Ptr<DepthStencilAttachment> depthStencil);
      
      // Creates render pass which's output is resolved from temporary MSAA target to window framebuffer
      virtual Ptr<RenderPass> create(const Ptr<Texture> temporaryMSAA,
                                     const Ptr<Texture> framebuffer,
                                     const Ptr<DepthStencilAttachment> depthStencil);

      // Internal universal method to create Render Pass
      Ptr<RenderPass> createRenderPass(const uint32 attachments,
                                       const Ptr<ColorAttachment> color[MaxColorAttachmentsCount],
                                       const Ptr<DepthStencilAttachment> depthStencil,
                                       const uint32v2 explicitResolution,
                                       const uint32   explicitLayers);
         
         

      virtual Ptr<DepthStencilState>  create(const DepthStencilStateInfo& desc);

      virtual Ptr<MultisamplingState> create(const uint32 samples,
                                             const bool enableAlphaToCoverage,
                                             const bool enableAlphaToOne);

      virtual Ptr<RasterState>        create(const RasterStateInfo& state);

      virtual Ptr<BlendState>         create(const BlendStateInfo& state,
                                             const uint32 attachments,
                                             const BlendAttachmentInfo* color);

      virtual Ptr<ViewportState>      create(const uint32 count,
                                             const ViewportStateInfo* viewports,
                                             const ScissorStateInfo* scissors);
      };








   class winDisplay : public CommonDisplay
      {
      public:
      uint32    index;              // Index of this display on Windows displays list
      bool      resolutionChanged;  // Flag if app changed display resolution (allows restoration of original resolution on exit)
      
      winDisplay();
     ~winDisplay();
      };
     
   // TODO: This should be moved to platform specific section
#if defined(EN_PLATFORM_WINDOWS)
   class winWindow : public CommonWindow
      {
      public:
      HINSTANCE AppInstance; // Application handle (helper handle)
      HWND hWnd;             // Window handle

      winWindow(const Ptr<winDisplay> selectedDisplay,
                const uint32v2 selectedResolution,
                const WindowSettings& settings,
                const string title);

      virtual bool movable(void);
      virtual void move(const uint32v2 position);
      virtual void resize(const uint32v2 size);
      virtual void active(void);

      virtual ~winWindow();
      };
#endif

   class WindowVK : public winWindow
      {
      public:
      VulkanDevice*  gpu;
      VkSurfaceKHR   swapChainSurface; 
      VkSwapchainKHR swapChain;
     
      WindowVK(VulkanDevice* gpu,
               const Ptr<CommonDisplay> selectedDisplay,
               const uint32v2 selectedResolution,
               const WindowSettings& settings,
                      const string title);
      
      virtual bool movable(void);
      virtual void move(const uint32v2 position);
      virtual void resize(const uint32v2 size);
      virtual void active(void);
      virtual void transparent(const float opacity);
      virtual void opaque(void);
      virtual Ptr<Texture> surface(void);
      virtual void display(void);
      
      virtual ~WindowVK();
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
      Ptr<CommonDisplay>*              display;
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
      DeclareFunction( vkGetPhysicalDeviceSurfaceSupportKHR )

      public:
      VulkanAPI(string appName);
     ~VulkanAPI();

      void loadInterfaceFunctionPointers(void);
      void clearInterfaceFunctionPointers(void);

      virtual uint32 devices(void) const;
      virtual Ptr<Display>   primaryDisplay(void) const;
      virtual Ptr<GpuDevice> primaryDevice(void) const;
      };
   }
}
#endif

#endif
