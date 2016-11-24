/*

 Ngine v5.0
 
 Module      : Vulkan Pipeline.
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

#if defined(EN_MODULE_RENDERER_VULKAN)

#include "core/rendering/pipeline.h"

namespace en
{
   namespace gpu
   {
   class VulkanDevice;

   class PipelineVK : public Pipeline
      {
      public:
      VulkanDevice* gpu;
      VkPipeline    handle;
      bool          graphics; // Graphics or Compute?

      PipelineVK(VulkanDevice* gpu, VkPipeline handle, bool _graphics);
      virtual ~PipelineVK();
      };

   }
}
#endif

#endif
