/*

 Ngine v5.0
 
 Module      : Metal Pipeline.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_METAL_PIPELINE
#define ENG_CORE_RENDERING_METAL_PIPELINE

#include "core/defines.h"

#if defined(EN_MODULE_RENDERER_METAL)

#include "core/rendering/metal/metal.h"
#include "core/rendering/pipeline.h"

#include "core/rendering/metal/mtlDepthStencil.h"
#include "core/rendering/metal/mtlRaster.h"
#include "core/rendering/metal/mtlViewport.h"

namespace en
{
   namespace gpu
   {
   class PipelineMTL : public Pipeline
      {
      public:
      id <MTLRenderPipelineState> handle;

      // Metal Dynamic State (Static in Vulkan)
      DepthStencilStateMTL* depthStencil;
      RasterStateMTL   raster;
      ViewportStateMTL viewport;
      MTLPrimitiveType primitive;
      
      PipelineMTL(const id<MTLDevice> device, MTLRenderPipelineDescriptor* desc, NSError** result);
      virtual ~PipelineMTL();
      };
   }
}
#endif

#endif
