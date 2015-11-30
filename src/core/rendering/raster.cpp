/*

 Ngine v5.0
 
 Module      : Common Raster.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#include "core/rendering/raster.h"

namespace en
{
   namespace gpu
   {
   // D3D12 Defaults:
   //
   // FillMode                  Solid
   // CullMode                  Back
   // FrontCounterClockwise     FALSE
   // DepthBias                 0
   // SlopeScaledDepthBias      0.0f
   // DepthBiasClamp            0.0f
   // DepthClipEnable           TRUE
   // MultisampleEnable         FALSE
   // AntialiasedLineEnable     FALSE
   // ForcedSampleCount         0
   // ConservativeRaster        D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF


   RasterStateInfo::RasterStateInfo() :
      enableCulling(false),
      enableDepthBias(false),
      enableDepthClamp(true),
      enableConservativeRasterization(false),
      disableRasterizer(false),
      fillMode(Solid),
      cullFace(BackFace),
      frontFace(CounterClockWise),
      depthBiasConstantFactor(0.0f),
      depthBiasClamp(0.0f),
      depthBiasSlopeFactor(0.0f),
    //pointSize(1.0f),
    //pointFadeThreshold(1.0f),
      lineWidth(1.0f)
   {
   }

   }
}