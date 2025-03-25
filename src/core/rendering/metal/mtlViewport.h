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

#ifndef ENG_CORE_RENDERING_METAL_VIEWPORT_STENCIL_STATE
#define ENG_CORE_RENDERING_METAL_VIEWPORT_STENCIL_STATE

#include "core/defines.h"

#if defined(EN_MODULE_RENDERER_METAL)

#include "core/rendering/metal/metal.h"
#include "core/rendering/viewport.h"

namespace en
{
namespace gpu
{

class ViewportStateMTL : public ViewportState
{
    public:
    MTLViewport viewport;
    MTLScissorRect scissor;
    
    ViewportStateMTL();
    ViewportStateMTL(const uint32 count,
                     const ViewportStateInfo* viewports,
                     const ScissorStateInfo* scissors);
    virtual ~ViewportStateMTL();
};

} // en::gpu
} // en
#endif

#endif
