/*

 Ngine v5.0
 
 Module      : Types.
 Requirements: none
 Description : 2 component float vector.

*/

#include <math.h>
#include "core/types/float2.h"

namespace en
{
   float2::float2(void) :
      x(0.0f),
      y(0.0f)
   {
   }
   
   float2::float2(const float _x, const float _y) :
      x(_x),
      y(_y)
   {
   }
   
   void float2::operator-= (const float2 b)
   {
   x -= b.x;
   y -= b.y; 
   }
   
   void float2::operator+= (const float2 b)
   {
   x += b.x;
   y += b.y; 
   }
   
   bool float2::operator== (const float2 b)
   {
   return ((x == b.x) && (y == b.y));
   }
   
   float2 float2::operator/ (const float2 b) const
   {
   return float2(x/b.x, y/b.y);
   }
   
   void float2::normalize(void)
   {
   float tmp = sqrt(x*x + y*y);
   if (tmp) 
      {			
      x /= tmp;
      y /= tmp;
      }
   }
   
   float float2::length(void) const
   {
   return sqrt(x*x + y*y);
   }
}