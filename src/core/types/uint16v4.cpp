/*

 Ngine v5.0
 
 Module      : Types.
 Requirements: none
 Description : 4 component uint16 vector.

*/

#include "core/types/uint16v4.h"

namespace en
{
   uint16v4::uint16v4(void) :
      x(0),
      y(0),
      z(0),
      w(0)
   {
   }
   
   uint16v4::uint16v4(const uint16 _x, const uint16 _y, const uint16 _z, const uint16 _w) :
      x(_x),
      y(_y),
      z(_z),
      w(_w)
   {
   }

   bool uint16v4::operator== (const uint16v4 b) const
   {
   return ((x == b.x) && (y == b.y) && (z == b.z) && (w == b.w));
   }
}

