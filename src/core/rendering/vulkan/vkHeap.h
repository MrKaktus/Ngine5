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
      VulkanDevice*  gpu;
      VkDeviceMemory handle;
      uint32         memoryType;
      Allocator*     allocator;    // Allocation algorithm used to place resources on the Heap
      Nmutex         mapped;
      
      HeapVK(VulkanDevice* gpu, 
             const VkDeviceMemory handle, 
             const uint32 _memoryType, 
             const MemoryUsage _usage,
             const uint32 size);

      // Return parent device
      virtual Ptr<GpuDevice> device(void) const;

      // Create formatted Vertex buffer that can be bound to InputAssembler.
      virtual Ptr<Buffer> createBuffer(const uint32 elements,
                                       const Formatting& formatting,
                                       const uint32 step = 0u);
        
      // Create formatted Index buffer that can be bound to InputAssembler.
      virtual Ptr<Buffer> createBuffer(const uint32 elements,
                                       const Attribute format);

      // Create unformatted generic buffer of given type and size.
      // This method can still be used to create Vertex or Index buffers,
      // but it's adviced to use ones with explicit formatting.
      virtual Ptr<Buffer> createBuffer(const BufferType type,
                                       const uint32 size);
      
      virtual Ptr<Texture> createTexture(const TextureState state);

      virtual ~HeapVK();
      };
   }
}
#endif

#endif
