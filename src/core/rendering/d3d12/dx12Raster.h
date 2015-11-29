/*

 Ngine v5.0
 
 Module      : D3D12 Raster State.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_D3D12_RASTER_STATE
#define ENG_CORE_RENDERING_D3D12_RASTER_STATE

#if defined(EN_PLATFORM_WINDOWS)

#include "core/rendering/d3d12/dx12.h"
#include "core/rendering/raster.h"

namespace en
{
   namespace gpu
   {
   class RasterStateD3D12 : public RasterState
      {
      public:
      D3D12_RASTERIZER_DESC state;

      RasterStateD3D12(const RasterStateInfo& desc);
      };
   }
}
#endif

#endif