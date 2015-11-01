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
   extern bool optimizeIndexSize;   // Reduces index size UInt -> UShort -> UByte
   extern bool optimizeIndexOrder;  // Optimizes indexes order for Post-Transform Vertex Cache Size

   Ptr<en::resource::Model> load(const string& filename, const string& name);
   }
}

#endif