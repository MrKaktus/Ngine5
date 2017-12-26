/*

 Ngine v5.0
 
 Module      : Vulkan Buffer.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_VULKAN_BUFFER
#define ENG_CORE_RENDERING_VULKAN_BUFFER

#include "core/rendering/vulkan/vulkan.h"

#if defined(EN_MODULE_RENDERER_VULKAN)

#include "core/rendering/common/buffer.h"
#include "core/rendering/vulkan/vkHeap.h"

namespace en
{
   namespace gpu
   {
   class VulkanDevice;
   
   class BufferVK : public CommonBuffer
      {
      public:
      VulkanDevice* gpu;
      VkBuffer      handle;
      VkMemoryRequirements memoryRequirements; // Memory requirements of this Buffer
      shared_ptr<HeapVK>   heap;               // Memory backing heap
      uint64               offset;             // Offset in the heap

      BufferVK(VulkanDevice* gpu, const VkBuffer handle, const BufferType type, const uint32 size);
      virtual ~BufferVK();
      
      virtual void* map(void);
      virtual void* map(const uint64 offset, const uint64 size);
      virtual void  unmap(void);
      };

   // Vulkan Buffer View is created only for Linear Textures backed by Buffers - not supported currently.
   //
#if 0
   class BufferViewVK : public CommonBufferView
      {
      public:
      shared_ptr<BufferVK> buffer; // Parent buffer
      VkBufferView  handle;  

      BufferViewVK(shared_ptr<BufferVK> parent,
                   const VkBufferView view,
                   const Format       format,
                   const uint32       offset,
                   const uint32       length);

      shared_ptr<Buffer> parent(void) const;
   
      virtual ~BufferViewVK();
      };
#endif

   shared_ptr<BufferVK> createBuffer(const HeapVK* heap, const BufferType type, const uint32 size);
   }
}
#endif

#endif
