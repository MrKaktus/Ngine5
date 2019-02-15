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

#include "core/defines.h"
#include "core/types.h"

#include "core/rendering/state.h"

namespace en
{
namespace gpu
{
/// Default state:
///
/// - rect              = float4(0.0f, 0.0f, 1.0f, 1.0f);
/// - depthRange        = float2(0.0f, 1.0f)
///
/// Rect defines viewport rectangle as follows:
/// x - top left origin x coordinate
/// y - top left origin y coordinate
/// z - viewport width
/// w - viewport height
///
struct ViewportStateInfo
{
    float4 rect;
    float2 depthRange;

    ViewportStateInfo();
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

} // en::gpu
} // en

#endif
