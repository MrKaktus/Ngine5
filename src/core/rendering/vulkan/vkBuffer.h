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

#include "core/rendering/buffer.h"

namespace en
{
   namespace gpu
   {
   class BufferVK : public Buffer
      {
      public:
      VkDevice device;
      VkBuffer handle;
      
      BufferVK();
      virtual ~BufferVK();
      };
      
   class BufferViewVK : public BufferView
      {
      public:
      VkDevice     device;
      VkBuffer     parent; // Buffer from which this View is created TODO: Or maybe generic Ptr<Buffer / BufferVK> parent; ?
      VkBufferView handle;
      
      BufferViewVK();
      virtual ~BufferViewVK();
      };
   }
}
#endif

#endif