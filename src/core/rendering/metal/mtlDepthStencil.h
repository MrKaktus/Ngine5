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

#include "core/defines.h"

#if defined(EN_MODULE_RENDERER_METAL)

#include "core/rendering/metal/metal.h"
#include "core/rendering/depthStencil.h"

namespace en
{
namespace gpu
{

class MetalDevice;
   
class DepthStencilStateMTL : public DepthStencilState
{
    public:
    id<MTLDepthStencilState> state;
    uint32v2 reference;

    DepthStencilStateMTL(const MetalDevice* gpu, const DepthStencilStateInfo& desc);
    virtual ~DepthStencilStateMTL();
};

} // en::gpu
} // en
#endif

#endif
