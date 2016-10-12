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

#ifndef ENG_CORE_RENDERING_VULKAN_VIEWPORT_STENCIL_STATE
#define ENG_CORE_RENDERING_VULKAN_VIEWPORT_STENCIL_STATE

#include "core/rendering/vulkan/vulkan.h"

#if defined(EN_MODULE_RENDERER_VULKAN)

#include "core/rendering/viewport.h"

namespace en
{
   namespace gpu
   {
   class ViewportStateVK : public ViewportState
      {
      public:
      VkPipelineViewportStateCreateInfo state;
      VkViewport viewport[16];
      VkRect2D   scissor[16];

      ViewportStateVK(const uint32 count, 
                      const ViewportStateInfo* viewports,
                      const ScissorStateInfo* scissors);
      };
   }
}

#endif

#endif
