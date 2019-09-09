/*

 Ngine v5.0

 Module      : OpenXR FrameState
 Requirements: none
 Description : XR context abstracts system specific
               backend implementation exposing XR 
               functionality (AR/MR/VR). It allows 
               programmer to use easy abstraction 
               layer on all platforms.

*/

#ifndef ENG_CORE_XR_OPENXR_FRAME_STATE
#define ENG_CORE_XR_OPENXR_FRAME_STATE

#include "core/xr/frameState.h"
#include "core/xr/openxr/oxrView.h"

namespace en
{
namespace xr
{
class oxrFrameState : public FrameState
{
    public:
    oxrView* viewsArray;      
    uint32   viewsCount;

    oxrFrameState();
    oxrFrameState(const uint32 viewsCount);
    virtual ~oxrFrameState();

    virtual uint32 views(void);
    virtual View* view(uint32 index);
};

} // en::xr
} // en
#endif
