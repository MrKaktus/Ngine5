/*

 Ngine v5.0
 
 Module      : Types.
 Requirements: none
 Description : 2 component sint16 vector.

*/

#include "core/types/sint16v2.h"

namespace en
{
   sint16v2::sint16v2(void) :
      x(0),
      y(0)
   {
   }
   
   sint16v2::sint16v2(const sint16 _x, const sint16 _y) :
      x(_x),
      y(_y)
   {
   }

   bool sint16v2::operator== (const sint16v2 b) const
   {
   return ((x == b.x) && (y == b.y));
   }
}