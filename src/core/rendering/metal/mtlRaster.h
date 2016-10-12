/*

 Ngine v5.0
 
 Module      : Metal Raster State.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_METAL_RASTER_STATE
#define ENG_CORE_RENDERING_METAL_RASTER_STATE

#include "core/defines.h"

#if defined(EN_MODULE_RENDERER_METAL)

#include "core/rendering/metal/metal.h"
#include "core/rendering/raster.h"

namespace en
{
   namespace gpu
   {
   class RasterStateMTL : public RasterState
      {
      public:
      MTLCullMode culling;
      MTLWinding frontFace;
      MTLTriangleFillMode fillMode;
      MTLDepthClipMode depthClamp;
      float depthBiasConstantFactor;
      float depthBiasClamp;
      float depthBiasSlopeFactor;
      bool  enableRasterization;
      
      RasterStateMTL(const RasterStateInfo& desc);
      virtual ~RasterStateMTL();
      };
   }
}
#endif

#endif
