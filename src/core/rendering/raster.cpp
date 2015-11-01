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

#if defined(EN_PLATFORM_WINDOWS) 
#include "core/rendering/d3d12/dx12Raster.h"
#endif
#if defined(EN_PLATFORM_IOS) || defined(EN_PLATFORM_OSX)
#include "core/rendering/metal/mtlRaster.h"
#endif
#if defined(EN_PLATFORM_WINDOWS) 
//#include "core/rendering/vulkan/vkRaster.h"
#endif

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

   Ptr<RasterState> Create(const RasterStateInfo& state)
   {
#if defined(EN_PLATFORM_WINDOWS) 
   return ptr_dynamic_cast<RasterState, RasterStateD3D12>(new RasterStateD3D12(state));

  // return ptr_dynamic_cast<RasterState, RasterStateVK>(new RasterStateVK(state));

#elif defined(EN_PLATFORM_IOS) || defined(EN_PLATFORM_OSX)
   return ptr_dynamic_cast<RasterState, RasterStateMTL>(new RasterStateMTL(state));
#endif
   }

   }
}