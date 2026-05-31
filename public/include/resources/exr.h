/*

 Ngine v5.0
 
 Module      : EXR files support
 Requirements: none
 Description : Supports reading images from *.exr files.

*/

#ifndef ENG_RESOURCES_EXR
#define ENG_RESOURCES_EXR

#include "core/defines.h"
#include "core/types.h"
#include "core/rendering/texture.h"
#include "core/rendering/state.h"

namespace en
{
namespace exr
{

bool loadMetadata(const std::string& filename,
                  gpu::TextureState& storedTextureState,
                  gpu::ColorSpace& storedColorSpace);

bool load(const std::string& filename,
          uint8* const destination,
          const gpu::TextureState expectedState,
          const gpu::ImageMemoryAlignment alignment,
          const uint16 selectedMipmap,
          const uint16 selectedLayer);

} // en::exr
} // en

#endif
