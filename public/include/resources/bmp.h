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
   Ptr<en::gpu::Texture> load(const string& filename);
   bool             load(Ptr<en::gpu::Texture> dst, const uint16 layer, const string& filename);
   //bool             save(Ptr<en::gpu::Texture> texture, const string& filename);
   bool             save(const uint32 width, const uint32 height, const uint8* ptr, const string& filename);

   en::gpu::TextureState settings(const string& filename);
   bool load(const string& filename, en::gpu::TextureState& settings, void* dst);
   }
}

#endif