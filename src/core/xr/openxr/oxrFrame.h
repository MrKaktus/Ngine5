/*

 Ngine v5.0

 Module      : OpenXR Frame
 Requirements: none
 Description : XR context abstracts system specific
               backend implementation exposing XR 
               functionality (AR/MR/VR). It allows 
               programmer to use easy abstraction 
               layer on all platforms.

*/

#ifndef ENG_CORE_XR_OPENXR_FRAME
#define ENG_CORE_XR_OPENXR_FRAME

#include "core/xr/frame.h"

namespace en
{
namespace xr
{

class oxrFrame : public Frame
{
    public:
    uint32        texturesCount;

    /// Arrray of color textures that should be used to render this frame.
    gpu::Texture** colorTextures; 

    /// Arrray of depth textures that should be used to render this frame.
    gpu::Texture** depthTextures;

    virtual uint32 textures(void);
    virtual gpu::Texture* colorTexture(const uint32 index);
    virtual gpu::Texture* depthTexture(const uint32 index);
    virtual bool isPresented(void);
    virtual bool isValid(void);

    oxrFrame();
    oxrFrame(const uint32 texturesCount);
   ~oxrFrame();
};

} // en::xr
} // en

#endif