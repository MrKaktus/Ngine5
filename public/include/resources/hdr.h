/*

 Ngine v5.0
 
 Module      : HDR files support
 Requirements: none
 Description : Supports reading images from *.hdr files.

*/

#ifndef ENG_RESOURCES_HDR
#define ENG_RESOURCES_HDR

#include "core/defines.h"
#include "core/types.h"
#include "core/rendering/texture.h"
#include "core/rendering/state.h"

namespace en
{
namespace hdr
{

enum Compression
{
    Unknown = 0,
    RLE_RGBE,
    RLE_XYZE
};

struct Metadata
{
    Compression compression;
    float exposure;
    uint64 offsetToData;
    bool columnOrder;
    bool positiveX;
    bool positiveY;
};

bool loadMetadata(const std::string& filename,
                  gpu::TextureState& storedTextureState,
                  gpu::ColorSpace& storedColorSpace,
                  Metadata& metadata);

bool load(const std::string& filename,
          uint8* const destination,                  ///< Pointer to buffer where image should be decompressed and decoded
          const uint32 width,                        ///< Expected width of surface
          const uint32 height,                       ///< Expected height of surface
          const gpu::Format format,                  ///< Expected format of surface
          const gpu::ImageMemoryAlignment alignment, ///< Alignment in which data is supposed to be ordered in memory
          const bool invertHorizontal = false);      ///< Determines if image should be flipped Horizontally

} // en::hdr
} // en

#endif
