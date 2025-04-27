/*

 Ngine v5.0
 
 Module      : MTL files support
 Requirements: none
 Description : Supports reading materials from *.mtl files.

*/

#ifndef ENG_RESOURCES_MTL
#define ENG_RESOURCES_MTL

#include "core/defines.h"
#include "core/types.h"
#include "resources/resources.h"

namespace en
{
namespace mtl
{

bool load(const std::string& filename, 
          const std::string& name, 
          resources::Material& material);

} // en::mtl
} // en

#endif