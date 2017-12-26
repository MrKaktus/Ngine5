/*

 Ngine v5.0
 
 Module      : Vulkan Viewport-Scissor State.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#include "core/rendering/vulkan/vkViewport.h"

#if defined(EN_MODULE_RENDERER_VULKAN)

#include "core/rendering/state.h"
#include "core/rendering/vulkan/vkDevice.h"

namespace en
{
   namespace gpu
   { 
   ViewportStateVK::ViewportStateVK(const uint32 count, 
                                    const ViewportStateInfo* viewports,
                                    const ScissorStateInfo* scissors)
   {
   assert( count <= 16 );
   memset(&viewport[0], 0, 16 * sizeof(VkViewport));
   memset(&scissor[0],  0, 16 * sizeof(VkRect2D));

   memcpy(&viewport[0], &viewports[0], count * sizeof(ViewportStateInfo));
   memcpy(&scissor[0],  &scissors[0],  count * sizeof(ScissorStateInfo));

   // Patch Viewports, to WA Vulkan's upside-down NDC Y axis.
   // Viewport Height is negated, and position is changed to lower-left 
   // corner, so that coordinates from NDC are flipped around X axis, and 
   // image is properly rendered to destination.
   for(uint32 i=0; i<count; ++i)
      {
      viewport[i].y      = viewport[i].y + viewport[i].height;
      viewport[i].height = -viewport[i].height;
      }

   // Set state
   state.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
   state.pNext         = nullptr;
   state.flags         = 0u;
   state.viewportCount = count;
   state.pViewports    = viewport;
   state.scissorCount  = count;
   state.pScissors     = scissor;
   }

   shared_ptr<ViewportState> VulkanDevice::createViewportState(const uint32 count,
                                                               const ViewportStateInfo* viewports,
                                                               const ScissorStateInfo* scissors)
   {
   return make_shared<ViewportStateVK>(count, viewports, scissors);
   }

   }
}
#endif
