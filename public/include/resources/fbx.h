/*

 Ngine v5.0
 
 Module      : FBX files support
 Requirements: none
 Description : Supports reading models from *.fbx files.

*/

#ifndef ENG_RESOURCES_FBX
#define ENG_RESOURCES_FBX

#include "core/defines.h"
#include "core/types.h"
#include "resources/resources.h"

namespace en
{
   namespace fbx
   {
#if defined(EN_PLATFORM_OSX) || defined(EN_PLATFORM_WINDOWS)
   // Compatible with Autodesk FBX SDK 2014.1
   shared_ptr<en::resource::Model> load(const string& filename, const string& name);
#endif
   }
}

#endif
