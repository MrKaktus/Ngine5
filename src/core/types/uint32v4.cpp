/*

 Ngine v5.0
 
 Module      : Types.
 Requirements: none
 Description : 4 component uint32 vector.

*/

#include "core/types/uint32v4.h"

namespace en
{
   uint32v4::uint32v4(void) :
      x(0),
      y(0),
      width(0),
      height(0)
   {
   }
   
   uint32v4::uint32v4(const uint32 _x, const uint32 _y, const uint32 _z, const uint32 _w) :
      x(_x),
      y(_y),
      width(_z),
      height(_w)
   {
   }
}