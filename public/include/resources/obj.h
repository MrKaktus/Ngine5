/*

 Ngine v5.0
 
 Module      : OBJ files support
 Requirements: none
 Description : Supports reading models from *.obj files.

*/

#ifndef ENG_RESOURCES_OBJ
#define ENG_RESOURCES_OBJ

#include "core/defines.h"
#include "core/types.h"
#include "resources/resources.h"

namespace en
{
namespace obj
{

// TODO: Move those to config
extern bool optimizeIndexSize;   // Reduces index size UInt -> UShort -> UByte
extern bool optimizeIndexOrder;  // Optimizes indexes order for Post-Transform Vertex Cache Size

std::shared_ptr<en::resources::Model> load(const std::string& filename, const std::string& name);

} // en::obj
} // en

#endif
