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

namespace en
{
   namespace exr
   {
   std::shared_ptr<en::gpu::Texture> load(const std::string& filename);
   }
}

#endif
