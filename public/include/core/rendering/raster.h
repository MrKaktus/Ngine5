/*

 Ngine v5.0
 
 Module      : Raster State.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/


#ifndef ENG_CORE_RENDERING_RASTER_STATE
#define ENG_CORE_RENDERING_RASTER_STATE

#include <memory>
using namespace std;

#include "core/defines.h"
#include "core/types.h"

#include "core/rendering/state.h"

namespace en
{
   namespace gpu
   {
   struct RasterStateInfo
      {
      bool  enableCulling;
      bool  enableDepthBias;
      bool  enableDepthClamp;
      bool  enableConservativeRasterization;
      bool  disableRasterizer;
      FillMode fillMode;
      Face  cullFace;
      NormalCalculationMethod frontFace;  // Method used to calculate Front Face Normal vector (default CounterClockWise)
      float depthBiasConstantFactor;      // Depth value added to given pixel's depth (prevents z-fighting on decals).
      float depthBiasClamp;               // More about depth bias and depth slopes:
      float depthBiasSlopeFactor;         // https://msdn.microsoft.com/en-us/library/windows/desktop/cc308048(v=vs.85).aspx
    //float pointSize;                    // In Metal in SL [[point_size]] (0.125 to 255.0)
    //float pointFadeThreshold;
      float lineWidth;                    // Deprecated in OpenGL, non-existent in Metal & Direct3D12

      RasterStateInfo();
      };

   class RasterState
      {
      public:
      virtual ~RasterState() {};                           // Polymorphic deletes require a virtual base destructor
      };
   }
}

#endif
