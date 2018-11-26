/*

 Ngine v5.0
 
 Module      : MATERIAL files support
 Requirements: none
 Description : Supports reading materials from *.material files.

*/

#ifndef ENG_RESOURCES_MATERIAL
#define ENG_RESOURCES_MATERIAL

#include "core/defines.h"
#include "core/types.h"
#include "resources/resources.h"

namespace en
{
   namespace material
   {
   en::resource::Material load(const std::string& filename);
   }
}

#endif