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
      virtual ~SemaphoreVK();
      };

   // Fine grained synchronization primitive. 
   // Use it to sync between concurrently executed commands (for eg. Compute and 3D, or Copy and Compute).
   class Event
      {
      public:
      virtual ~Event() {};
      };

   // TODO: There are no Events equivalent in D3D12 nor Metal !!!!
   //       Keeps this API private until can be exposed, or emulated on all three backends.

   class EventVK : public Event
      {
      public:
      VulkanDevice* gpu;
      VkEvent       handle;

      EventVK(VulkanDevice* gpu);
      virtual ~EventVK();

      virtual bool signaled(void);
      virtual void signal(void);    // Signals event from CPU side
      virtual void unsignal(void);  // Disable event from CPU side
      };
   }
}
#endif

#endif