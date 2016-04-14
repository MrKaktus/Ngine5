/*

 Ngine v5.0
 
 Module      : Vulkan Multisampling State.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_VULKAN_MULTISAMPLING_STATE
#define ENG_CORE_RENDERING_VULKAN_MULTISAMPLING_STATE

#include "core/rendering/vulkan/vulkan.h"

#if defined(EN_PLATFORM_ANDROID) || defined(EN_PLATFORM_WINDOWS)

#include "core/rendering/multisampling.h"

namespace en
{
   namespace gpu
   {
   class MultisamplingStateVK : public MultisamplingState
      {
      public:
      VkPipelineMultisampleStateCreateInfo state;

      MultisamplingStateVK(const uint32 samples,              // 1 - multisampling is disabled
                           const bool enableAplhaToCoverage,
                           const bool enableAlphaToOne);
      };
   }
}
#endif

#endif