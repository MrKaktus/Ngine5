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

namespace en
{
namespace hdr
{

std::shared_ptr<en::gpu::Texture> load(const std::string& filename);

} // en::hdr
} // en

#endif
