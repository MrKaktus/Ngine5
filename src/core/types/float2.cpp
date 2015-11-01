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
   float2::float2(void)
   {
   x = 0.0f;
   y = 0.0f;
   }
   
   float2::float2(float x=0.0f, float y=0.0f)
   {
   this->x = x;
   this->y = y;
   }
   
   void float2::operator-= (float2 b)
   {
   x -= b.x;
   y -= b.y; 
   }
   
   void float2::operator+= (float2 b)
   {
   x += b.x;
   y += b.y; 
   }
   
   bool float2::operator== (float2 b)
   {
   return ((x == b.x) && (y == b.y));
   }
   
   float2 float2::operator/ (float2 b)
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
   
   float float2::length(void)
   {
   return sqrt(x*x + y*y);
   }
}