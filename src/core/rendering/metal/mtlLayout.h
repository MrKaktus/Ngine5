/*

 Ngine v5.0
 
 Module      : Metal Resource Layout.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_METAL_RESOURCE_LAYOUT
#define ENG_CORE_RENDERING_METAL_RESOURCE_LAYOUT

#include "core/defines.h"

#if defined(EN_MODULE_RENDERER_METAL)

#include "core/rendering/metal/metal.h"
#include "core/rendering/layout.h"

namespace en
{
   namespace gpu
   {
   //class MetalDevice;

   class SetLayoutMTL : public SetLayout
      {
      public:
      //MetalDevice* gpu;
      id<MTLArgumentEncoder> handle;

      SetLayoutMTL(MetalDevice* gpu, id<MTLArgumentEncoder> encoder);
      virtual ~SetLayoutMTL();
      };

   class PipelineLayoutMTL : public PipelineLayout
      {
      public:
      //MetalDevice* gpu;
      //VkPipelineLayout handle;

      PipelineLayoutMTL(/*MetalDevice* gpu, VkPipelineLayout handle*/);
      virtual ~PipelineLayoutMTL();
      };
   }
}
#endif

#endif
