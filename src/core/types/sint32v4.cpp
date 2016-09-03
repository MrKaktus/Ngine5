/*

 Ngine v5.0
 
 Module      : Types.
 Requirements: none
 Description : 4 component sint32 vector.

*/

#include "core/types/sint32v4.h"

namespace en
{
   sint32v4::sint32v4(void) :
      x(0),
      y(0),
      width(0),
      height(0)
   {
   }
   
   sint32v4::sint32v4(sint32 _x, sint32 _y, sint32 _z, sint32 _w) :
      x(_x),
      y(_y),
      width(_z),
      height(_w)
   {
   }
}
