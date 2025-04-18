/*

 Ngine v5.0
 
 Module      : Metal Viewport-Scissor State.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#include "core/rendering/metal/mtlViewport.h"

#if defined(EN_MODULE_RENDERER_METAL)

#include "core/rendering/metal/mtlDevice.h"
#include "core/rendering/state.h"

namespace en
{
namespace gpu
{

ViewportStateMTL::ViewportStateMTL()
{
    viewport.originX = 0.0f;
    viewport.originY = 0.0f;
    viewport.width   = 0.0f;
    viewport.height  = 0.0f;
    viewport.znear   = 0.0f;
    viewport.zfar    = 0.0f;
    scissor.x        = 0;
    scissor.y        = 0;
    scissor.width    = 0;
    scissor.height   = 0;
}
   
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
    scissor.x        = static_cast<NSUInteger>(scissors[0].x);
    scissor.y        = static_cast<NSUInteger>(scissors[0].y);
    scissor.width    = static_cast<NSUInteger>(scissors[0].width);
    scissor.height   = static_cast<NSUInteger>(scissors[0].height);
}

ViewportStateMTL::~ViewportStateMTL()
{
}

ViewportState* MetalDevice::createViewportState(const uint32 count,
                                                const ViewportStateInfo* viewports,
                                                const ScissorStateInfo* scissors)
{
    assert( count );
    assert( viewports );
    assert( scissors );
   
    return new ViewportStateMTL(count, viewports, scissors);
}
   
} // en::gpu
} // en

#endif
