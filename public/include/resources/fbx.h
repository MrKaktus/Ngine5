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
std::shared_ptr<en::resources::Model> load(const std::string& filename, const std::string& name);
#endif

} // en::fbx
} // en

#endif
