/*

 Ngine v5.0
 
 Module      : Pipeline.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_VULKAN_PIPELINE
#define ENG_CORE_RENDERING_VULKAN_PIPELINE

#include "core/rendering/vulkan/vulkan.h"

#if defined(EN_PLATFORM_ANDROID) || defined(EN_PLATFORM_WINDOWS)

#include "core/rendering/pipeline.h"

namespace en
{
   namespace gpu
   {
   class PipelineVK : public Pipeline
      {
      public:
      VkPipeline id;

      PipelineVK();
      virtual ~PipelineVK();
      };

   }
}
#endif

#endif