

#include "assert.h"

#include "core/xr/openxr/oxrFrame.h"

namespace en
{
namespace xr
{
oxrFrame::oxrFrame() :
    texturesCount(0),
    colorTextures(nullptr),
    depthTextures(nullptr)
{
    // Texture pointers are assigned by parent Presentation Session
}

oxrFrame::oxrFrame(const uint32 _texturesCount) :
    texturesCount(_texturesCount),
    colorTextures(new gpu::Texture*[texturesCount]),
    depthTextures(new gpu::Texture*[texturesCount])
{
    // Texture pointers are assigned by parent Presentation Session
}

oxrFrame::~oxrFrame()
{
    // Textures themselves are not released as Frame object only
    // references textures owned by Presentation Session.

    // Release arryas of pointers to textures
    delete [] colorTextures;
    delete [] depthTextures;
}

uint32 oxrFrame::textures(void)
{
    return texturesCount;
}

gpu::Texture* oxrFrame::colorTexture(const uint32 index)
{
    assert( index < texturesCount );

    return colorTextures[index];
}

gpu::Texture* oxrFrame::depthTexture(const uint32 index)
{
    assert( index < texturesCount );

    return depthTextures[index];
}

bool oxrFrame::isPresented(void)
{
    // TODO: Finish!
    return true;
}

bool oxrFrame::isValid(void)
{
    // TODO: Finish!
    return true;
}

} // en::xr
} // en