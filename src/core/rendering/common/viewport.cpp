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

#include "core/rendering/viewport.h"

namespace en
{
namespace gpu
{

ViewportStateInfo::ViewportStateInfo() :
    rect(0.0f, 0.0f, 1.0f, 1.0f),
    depthRange(0.0f, 1.0f)
{
}

} // en::gpu
} // en
