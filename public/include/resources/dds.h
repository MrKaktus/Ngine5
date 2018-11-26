/*

 Ngine v5.0
 
 Module      : DDS file support
 Requirements: none
 Description : Supports reading images from *.dds files.

*/

#ifndef ENG_RESOURCES_DDS
#define ENG_RESOURCES_DDS

#include "core/defines.h"
#include "core/types.h"
#include "core/rendering/state.h"

namespace en
{
   namespace dds
   {
   //std::shared_ptr<gpu::Texture> load(const std::string& filename, 
   //                      const gpu::ColorSpace colorSpace = gpu::ColorSpaceLinear, 
   //                      const bool invertHorizontal = false);

   std::shared_ptr<gpu::Texture> load(const std::string& filename);
   }
}

#endif
