/*

 Ngine v5.0
 
 Module      : Metal Depth-Stencil State.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_METAL_DEPTH_STENCIL_STATE
#define ENG_CORE_RENDERING_METAL_DEPTH_STENCIL_STATE

#if defined(EN_PLATFORM_IOS) || defined(EN_PLATFORM_OSX)

#include "core/rendering/metal/metal.h"
#include "core/rendering/depthStencil.h"

namespace en
{
   namespace gpu
   {
   class DepthStencilStateMTL : public DepthStencilState
      {
      public:
      id<MTLDepthStencilState> state;
      uint32v2 reference;

      DepthStencilStateMTL();
     ~DepthStencilStateMTL();
      };

   }
}
#endif

#endif