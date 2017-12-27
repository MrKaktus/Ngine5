/*

 Ngine v5.0
 
 Module      : Viewport-Stencil State.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/


#ifndef ENG_CORE_RENDERING_VIEWPORT_STENCIL_STATE
#define ENG_CORE_RENDERING_VIEWPORT_STENCIL_STATE

#include <memory>
using namespace std;

#include "core/defines.h"
#include "core/types.h"

#include "core/rendering/state.h"

namespace en
{
   namespace gpu
   {
   struct ViewportStateInfo
      {
      float4 rect;
      float2 depthRange;
      };

   typedef uint32v4 ScissorStateInfo;

   class ViewportState
      {
      public:
      virtual ~ViewportState() {};
      };

   // glViewport() does not clip, unlike the viewport in D3D
   // Set the scissor rect to match viewport area, unless it is explicitly set smaller, to emulate D3D.
   // In D3D12 viewports and scissors are separate dynamic states.
   }
}

#endif
