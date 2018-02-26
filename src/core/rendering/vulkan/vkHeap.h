/*

 Ngine v5.0
 
 Module      : Vulkan Heap.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_VULKAN_HEAP
#define ENG_CORE_RENDERING_VULKAN_HEAP

#include "core/rendering/vulkan/vulkan.h"

#if defined(EN_MODULE_RENDERER_VULKAN)

#include "core/rendering/common/heap.h"
#include "core/rendering/common/device.h"
#include "core/utilities/basicAllocator.h"

namespace en
{
   namespace gpu
   {
   class VulkanDevice;

   class HeapVK : public CommonHeap
      {
      public:
      shared_ptr<VulkanDevice> gpu;
      VkDeviceMemory handle;
      uint32         memoryType;
      Allocator*     allocator;     // Allocation algorithm used to place resources on the Heap
      Mutex         mutex;         // Guards critical section during mapping
      uint32         mappingsCount;
      void*          mappedPtr;      

      HeapVK(shared_ptr<VulkanDevice> gpu,
             const VkDeviceMemory handle, 
             const uint32 _memoryType, 
             const MemoryUsage _usage,
             const uint32 size);

      // Return parent device
      virtual shared_ptr<GpuDevice> device(void) const;

      // Create unformatted generic buffer of given type and size.
      // This method can still be used to create Vertex or Index buffers,
      // but it's adviced to use ones with explicit formatting.
      virtual shared_ptr<Buffer> createBuffer(const BufferType type,
                                              const uint32 size);
      
      virtual shared_ptr<Texture> createTexture(const TextureState state);

      virtual ~HeapVK();
      };
   }
}
#endif

#endif
