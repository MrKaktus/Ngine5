/*

 Ngine v5.0

 Module      : Frame
 Requirements: none
 Description : XR context abstracts system specific
               backend implementation exposing XR 
               functionality (AR/MR/VR). It allows 
               programmer to use easy abstraction 
               layer on all platforms.

*/

#ifndef ENG_CORE_XR_FRAME
#define ENG_CORE_XR_FRAME

#include "core/rendering/texture.h"

namespace en
{
namespace xr
{

class Frame
{
    public:
    virtual uint32 textures(void) = 0;
    virtual gpu::Texture* colorTexture(const uint32 index) = 0;
    virtual gpu::Texture* depthTexture(const uint32 index) = 0;
    virtual bool isPresented(void) = 0;
    virtual bool isValid(void) = 0;

    virtual ~Frame() {};
};

} // en::xr
} // en

#endif
