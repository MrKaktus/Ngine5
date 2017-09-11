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
   
   uint32v2::uint32v2(const uint32 _x, const uint32 _y) :
      x(_x),
      y(_y)
   {
   }

   bool uint32v2::operator== (const uint32v2 b) const
   {
   return ((x == b.x) && (y == b.y));
   }
}