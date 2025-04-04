/*

 Ngine v5.0

 Module      : Headset.
 Requirements: none
 Description : XR context abstracts system specific
               backend implementation exposing XR 
               functionality (AR/MR/VR). It allows 
               programmer to use easy abstraction 
               layer on all platforms.

*/

#ifndef ENG_CORE_XR_HEADSET
#define ENG_CORE_XR_HEADSET

#include "core/rendering/device.h"
#include "core/rendering/texture.h"
#include "core/xr/presentationSession.h"

namespace en
{
namespace xr
{

class Headset
{
    public:
    virtual gpu::GpuDevice* device(void) = 0;
    virtual PresentationSession* createPresentationSession(const PresentationSessionDescriptor& descriptor) = 0;

    virtual ~Headset() {};
};

} // en::xr

// Subsystem main entry:
//extern xr::Interface* XR;

} // en

#endif
