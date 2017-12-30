/*

 Ngine v5.0
 
 Module      : Vulkan Raster State.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_VULKAN_RASTER_STATE
#define ENG_CORE_RENDERING_VULKAN_RASTER_STATE

#include "core/rendering/vulkan/vulkan.h"

#if defined(EN_MODULE_RENDERER_VULKAN)

#include "core/rendering/raster.h"

namespace en
{
   namespace gpu
   {
   class RasterStateVK : public RasterState
      {
      public:
      VkPipelineRasterizationStateCreateInfo state;

      RasterStateVK(const RasterStateInfo& desc);
      };
   }
}
#endif

#endif
