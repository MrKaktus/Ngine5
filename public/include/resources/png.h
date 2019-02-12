/*

 Ngine v5.0
 
 Module      : PNG file support
 Requirements: none
 Description : Supports reading images from *.png files.

*/

#ifndef ENG_RESOURCES_PNG
#define ENG_RESOURCES_PNG

#include "core/defines.h"
#include "core/types.h"
#include "core/rendering/state.h"
#include "core/rendering/texture.h"

namespace en
{
namespace png
{
bool load(const std::string& filename, 
          uint8* const destination,                  ///< Pointer to buffer where image should be decompressed and decoded
          const uint32 width,                        ///< Expected width of surface
          const uint32 height,                       ///< Expected height of surface
          const gpu::Format format,                  ///< Expected format of surface
          const gpu::ImageMemoryAlignment alignment, ///< Alignment in which data is supposed to be ordered in memory
          const bool invertHorizontal = false);      ///< Determines if image should be flipped Horizontally

} // en::png
} // en

#endif
