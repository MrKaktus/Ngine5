/*

 Ngine v5.0
 
 Module      : Vulkan Blend State.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_VULKAN_BLEND_STATE
#define ENG_CORE_RENDERING_VULKAN_BLEND_STATE

#include "core/rendering/vulkan/vulkan.h"

#if defined(EN_PLATFORM_ANDROID) || defined(EN_PLATFORM_WINDOWS)

#include "core/rendering/blend.h"

namespace en
{
   namespace gpu
   {
   class BlendStateVK : public BlendState
      {
      public:
      VkPipelineColorBlendAttachmentState blendInfo[MaxColorAttachmentsCount];
      VkPipelineColorBlendStateCreateInfo state;

      BlendStateVK(const BlendStateInfo& desc,
                   const uint32 attachments, 
                   const BlendAttachmentInfo* color);
      };
   }
}
#endif

#endif