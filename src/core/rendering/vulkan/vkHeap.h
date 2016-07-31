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

#if defined(EN_PLATFORM_ANDROID) || defined(EN_PLATFORM_WINDOWS)

#include "core/rendering/common/heap.h"
#include "core/rendering/common/device.h"

namespace en
{
   namespace gpu
   {
   class HeapVK : public CommonHeap
      {
      public:
      const CommonDevice* gpu;
      VkDeviceMemory      handle;

      HeapVK(const CommonDevice* gpu, const VkDeviceMemory handle, const uint32 size);

      // Create formatted Vertex buffer that can be bound to InputAssembler.
      virtual Ptr<Buffer> create(const uint32 elements,
                                 const Formatting& formatting,
                                 const uint32 step = 0,
                                 const void* data = nullptr);
        
      // Create formatted Index buffer that can be bound to InputAssembler.
      virtual Ptr<Buffer> create(const uint32 elements,
                                 const Attribute format,
                                 const void* data = nullptr);

      // Create unformatted generic buffer of given type and size.
      // This method can still be used to create Vertex or Index buffers,
      // but it's adviced to use ones with explicit formatting.
      virtual Ptr<Buffer> create(const BufferType type,
                                 const uint32 size,
                                 const void* data = nullptr);
      
      virtual Ptr<Texture> create(const TextureState state);

      virtual ~HeapVK();
      };
   }
}
#endif

#endif