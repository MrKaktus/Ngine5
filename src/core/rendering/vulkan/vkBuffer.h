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

#if defined(EN_PLATFORM_ANDROID) || defined(EN_PLATFORM_WINDOWS)

#include "core/rendering/heap.h"
#include "core/rendering/common/buffer.h"

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
      Ptr<Heap>     heap;      // Memory backing heap
      uint64        offset;    // Offset in the heap

      BufferVK(VulkanDevice* gpu, const VkBuffer handle, const BufferType type, const uint32 size);
      virtual ~BufferVK();
      };
      
   class BufferViewVK : public BufferView
      {
      public:
      VulkanDevice* gpu;
      VkBuffer      parent; // Buffer from which this View is created TODO: Or maybe generic Ptr<Buffer / BufferVK> parent; ?
      VkBufferView  handle;
      
      BufferViewVK(VulkanDevice* gpu);
      virtual ~BufferViewVK();
      };
      
   Ptr<Buffer> createBuffer(VulkanDevice* gpu, const BufferType type, const uint32 size);
   Ptr<Buffer> createBufferAndPopulate(VulkanDevice* gpu, const BufferType type, const uint32 size, const void* data);

   }
}
#endif

#endif
