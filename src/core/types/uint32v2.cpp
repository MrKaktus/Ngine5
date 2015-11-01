/*

 Ngine v5.0
 
 Module      : Types.
 Requirements: none
 Description : 2 component uint32 vector.

*/

#include "core/types/uint32v2.h"

namespace en
{
   uint32v2::uint32v2(void) :
      x(0),
      y(0)
   {
   }
   
   uint32v2::uint32v2(uint32 a, uint32 b) :
      x(a),
      y(b)
   {
   }
}