/*

 Ngine v5.0
 
 Module      : Vulkan Synchronization.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_VULKAN_SYNCHRONIZATION
#define ENG_CORE_RENDERING_VULKAN_SYNCHRONIZATION

#include "core/rendering/vulkan/vulkan.h"

#if defined(EN_MODULE_RENDERER_VULKAN)

#include "core/rendering/synchronization.h"

namespace en
{
   namespace gpu
   {
   class VulkanDevice;



   // It's currently just a handle to synchronize command buffers execution.
   // Similar to MTLFence in Metal, but works between CB's submissions.
   class SemaphoreVK : public Semaphore
      {
      public:
      VulkanDevice* gpu;
      VkSemaphore   handle;
      
      SemaphoreVK(VulkanDevice* _gpu);
     ~SemaphoreVK();
      };

   }
}
#endif

#endif