/*

 Ngine v5.0
 
 Module      : TGA files support
 Requirements: none
 Description : Supports reading images from *.tga files.

*/

#ifndef ENG_RESOURCES_TGA
#define ENG_RESOURCES_TGA

#include "core/defines.h"
#include "core/types.h"
#include "core/rendering/texture.h"

namespace en
{
   namespace tga
   {
   bool load(shared_ptr<en::gpu::Texture> dst, const uint16 layer, const string& filename);
   shared_ptr<en::gpu::Texture> load(const string& filename);
   }
}

#endif
