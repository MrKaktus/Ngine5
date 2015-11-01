/*

 Ngine v5.0
 
 Module      : Forsyth algorithm
 Requirements: none
 Description : Contains set of functions that optimize
               3D model meshes for faster rendering by
               graphic card.

*/

#ifndef ENG_RESOURCES_FORSYTH
#define ENG_RESOURCES_FORSYTH

#include "core/defines.h"
#include "core/types.h"

namespace en
{
   namespace Forsyth
   {
   void optimize(const vector<uint32>& input, vector<uint32>& output, uint32 vertexes);
   }
}

#endif