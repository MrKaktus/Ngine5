/*

 Ngine v5.0
 
 Module      : Viewport-Scissor State.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#include "core/defines.h"

#if defined(EN_PLATFORM_IOS) || defined(EN_PLATFORM_OSX)

#include "core/rendering/metal/mtlViewport.h"
#include "core/rendering/state.h"

namespace en
{
   namespace gpu
   { 
   ViewportStateMTL::ViewportStateMTL(const uint32 count, 
                                      const ViewportStateInfo* viewports,
                                      const ScissorStateInfo* scissors)
   {
   viewport.originX = static_cast<double>(viewports[0].rect.x);
   viewport.originY = static_cast<double>(viewports[0].rect.y);
   viewport.width   = static_cast<double>(viewports[0].rect.z);
   viewport.height  = static_cast<double>(viewports[0].rect.w);
   viewport.znear   = static_cast<double>(viewports[0].depthRange.x);
   viewport.zfar    = static_cast<double>(viewports[0].depthRange.y);

   // Save it on creation of Render Command Encoder 
   }

   }
}
#endif