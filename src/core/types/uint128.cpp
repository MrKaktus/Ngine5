/*

 Ngine v5.0
 
 Module      : Types.
 Requirements: none
 Description : 128 bit unsigned integer.

*/

#include "core/types/uint128.h"

namespace en
{
   uint128::uint128(const void* src)
   {
   if (src)
      {
      lo = *(const uint64*)src;
      hi = *((const uint64*)src + 1);
      }
   }

   uint128& uint128::operator=(const uint128& b)
   {
   if (this != &b) 
      { 
      lo = b.lo;
      hi = b.hi;
      }
   return *this;
   }
}