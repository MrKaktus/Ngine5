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
   std::shared_ptr<en::gpu::Texture> load(const std::string& filename);
   bool             load(std::shared_ptr<en::gpu::Texture> dst, const uint16 layer, const std::string& filename);
   //bool             save(std::shared_ptr<en::gpu::Texture> texture, const std::string& filename);
   bool             save(const uint32 width, const uint32 height, const uint8* ptr, const std::string& filename);

   en::gpu::TextureState settings(const std::string& filename);
   bool load(const std::string& filename, en::gpu::TextureState& settings, void* dst);
   }
}

#endif
