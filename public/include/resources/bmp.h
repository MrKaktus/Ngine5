/*

 Ngine v5.0
 
 Module      : BMP files support
 Requirements: none
 Description : Supports reading images from *.bmp files.

*/

#ifndef ENG_RESOURCES_BMP
#define ENG_RESOURCES_BMP

#include "core/defines.h"
#include "core/types.h"
#include "core/rendering/texture.h"

namespace en
{
namespace bmp
{

bool load(const std::string& filename,
          uint8* const destination,                  ///< Pointer to buffer where image should be decompressed and decoded
          const uint32 width,                        ///< Expected width of surface
          const uint32 height,                       ///< Expected height of surface
          const gpu::Format format,                  ///< Expected format of surface
          const gpu::ImageMemoryAlignment alignment, ///< Alignment in which data is supposed to be ordered in memory
          const bool invertHorizontal = false);      ///< Determines if image should be flipped Horizontally

bool save(const std::string& filename,
          const uint8* source,
          const uint32 width, 
          const uint32 height);

} // en::bmp
} // en
#endif
