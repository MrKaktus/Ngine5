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


// Vulkan calls can be performed only inside Vulkan Device class.
// "lastResult" is Vulkan Device variable.
#ifdef EN_DEBUG
namespace en
{
   namespace gpu
   {
   extern bool IsError(const VkResult result);
   extern bool IsWarning(const VkResult result);
   }
}

   #ifdef EN_PROFILER_TRACE_GRAPHICS_API
   #define Profile( x )                        \
           {                                   \
           Log << "Vulkan: " << #x << endl;    \
           lastResult = x;                     \
           if (en::gpu::IsError(lastResult))   \
              assert(0);                       \
           en::gpu::IsWarning(lastResult);     \
           }
   #define ProfileNoRet( x )                   \
           {                                   \
           Log << "Vulkan: " << #x << endl;    \
           x;                                  \
           }
   #else 
   #define Profile( x )                        \
           {                                   \
           lastResult = x;                     \
           if (en::gpu::IsError(lastResult))   \
              assert(0);                       \
           en::gpu::IsWarning(lastResult);     \
           }
   #define ProfileNoRet( x )                   \
           x;                                  
   #endif
#else
   #define Profile( x ) lastResult = x; /* Nothing in Release */
#endif



using namespace std;

namespace en
{
   namespace gpu
   {
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











   // Vulkan API Layer description
   struct LayerDescriptor
      {
      VkLayerProperties      properties;
      VkExtensionProperties* extension;
      uint32                 extensionsCount;

      LayerDescriptor();
      };



   class winDisplay : public Display
      {
      public:
      uint32v2  observedResolution; // Display resolution when app started
      uint32v2* modeResolution;     // Resolutions of display modes supported by this display
      uint32    modesCount;         // Count of display modes supported by this display (from the list of modes supported by the driver)
      uint32    index;              // Index of this display on Windows displays list
      bool      resolutionChanged;  // Flag if app changed display resolution (allows restoration of original resolution on exit)
      
      winDisplay();
     ~winDisplay();
      };
     
   class MetalDevice;
   
   class WindowVK : public winWindow
      {
      public:
      VkSwapchainKHR swapChain;
      
      virtual bool movable(void);
      virtual void move(const uint32v2 position);
      virtual void resize(const uint32v2 size);
      virtual void active(void);
      virtual void transparent(const float opacity);
      virtual void opaque(void);
      virtual Ptr<Texture> surface(void);
      virtual void display(void);
      
      WindowVK(const MetalDevice* gpu, const WindowSettings& settings, const string title); //id<MTLDevice> device
      virtual ~WindowVK();
      };






   // TODO: Move it to Thread Pool Scheduler
   #define MaxSupportedWorkerThreads 64
   
   class VulkanDevice : public CommonDevice
      {
      public:
      VkResult                         lastResult;   // Per thread ?
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
      LayerDescriptor*                 layer;
      uint32                           layersCount;
      VkExtensionProperties*           globalExtension;
      uint32                           globalExtensionsCount;

      VkAllocationCallbacks            defaultAllocCallbacks;
      VkPipelineCache                  pipelineCache;
      uint64                           memoryRAM;
      uint64                           memoryDriver;

      // Device Function Pointers
 //  #include "core/rendering/vulkan/vulkan10.h"

      // Helper functions
      void bindDeviceFunctionPointers(void);
      void unbindDeviceFunctionPointers(void);

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
      VulkanDevice(const VkPhysicalDevice handle);
     ~VulkanDevice();


      bool getMemoryType(uint32 allowedTypes, VkFlags properties, uint32* memoryType);
      VkDeviceMemory allocMemory(VkMemoryRequirements requirements, VkFlags properties);


      virtual Ptr<Buffer> create(const uint32 elements, const Formatting& formatting, const uint32 step = 0);
      virtual Ptr<Buffer> create(const uint32 elements, const Attribute format);
      virtual Ptr<Buffer> create(const BufferType type, const uint32 size);
      


      Ptr<Texture>  Create(const TextureState& state);   // This should be done out of Heap
      Ptr<Sampler>  Create(const SamplerState& state);
      Ptr<Pipeline> Create(const Ptr<InputAssembler> inputAssembler,
                           const Ptr<ViewportState>  viewportState,
                           const Ptr<RasterState>    rasterState,
                           const Ptr<MultisamplingState> multisamplingState,                        
                           const Ptr<DepthStencilState> depthStencilState,
                           const Ptr<BlendState>     blendState,
                           const Ptr<PipelineLayout> pipelineLayout);
      };

   class VulkanAPI : public GraphicAPI
      {
      public:
#if defined(EN_PLATFORM_WINDOWS)
      HMODULE                          library;    // Vulkan dynamic library handle.
#endif
#if defined(EN_PLATFORM_LINUX)
      void*                            library;
#endif
      VkResult                         lastResult;
      LayerDescriptor*                 layer;
      uint32                           layersCount;
      VkExtensionProperties*           globalExtension;
      uint32                           globalExtensionsCount;

      VkInstance                       instance;    // Application Vulkan API Instance
      Ptr<VulkanDevice>*               device;      // Physical Device Interfaces
      uint32                           devicesCount;

      // API Independent, OS Dependent - Windowing System
      Ptr<Screen>*                     display;
      Ptr<Screen>                      virtualDisplay;
      uint32                           displaysCount;
      uint32                           displayPrimary;
  




      PFN_vkGetInstanceProcAddr                          vkGetInstanceProcAddr; 
      PFN_vkEnumerateInstanceExtensionProperties         vkEnumerateInstanceExtensionProperties;  
      PFN_vkEnumerateInstanceLayerProperties             vkEnumerateInstanceLayerProperties; 
      PFN_vkCreateInstance                               vkCreateInstance;
      PFN_vkEnumeratePhysicalDevices                     vkEnumeratePhysicalDevices; 

      PFN_vkGetPhysicalDeviceSurfaceSupportKHR           vkGetPhysicalDeviceSurfaceSupportKHR;

      public:
      VulkanAPI(string appName);
     ~VulkanAPI();

      void bindInterfaceFunctionPointers(void);
      };
   }
}
#endif

#endif
