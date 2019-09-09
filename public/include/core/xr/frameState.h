/*

 Ngine v5.0

 Module      : FrameState
 Requirements: none
 Description : XR context abstracts system specific
               backend implementation exposing XR 
               functionality (AR/MR/VR). It allows 
               programmer to use easy abstraction 
               layer on all platforms.

*/

#ifndef ENG_CORE_XR_FRAME_STATE
#define ENG_CORE_XR_FRAME_STATE

#include "core/types.h"
#include "core/xr/view.h"

namespace en
{
namespace xr
{
class FrameState
{
    public:
    uint64 targetPresentationEvent;
    uint64 compositorSyncValue;
    uint64 renderingFrame;
    uint64 shadingRate;

    virtual uint32 views(void) = 0;
    virtual View* view(uint32 index) = 0;

    virtual ~FrameState() {};
};

} // en::xr
} // en
#endif