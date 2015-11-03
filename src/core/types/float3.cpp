/*

 Ngine v5.0
 
 Module      : Types.
 Requirements: none
 Description : 3 component float vector.

*/

#include <math.h>
#include <string.h>
#include "core/types/float3.h"

namespace en
{
   float3::float3(void)
   {
   memset(&x, 0, 12);
   }
   
   float3::float3(const float* src)
   {
   memcpy(&x, src, 12);
   }
   
   float3::float3(double3 d3)
   {
   x = (float)d3.x;
   y = (float)d3.y;
   z = (float)d3.z;
   }
   
   float3::float3(float x, float y, float z)
   {
   this->x = x;
   this->y = y;
   this->z = z;
   }
   
   void float3::operator-= (float3 b)
   {
   x -= b.x;
   y -= b.y; 
   z -= b.z;
   }
   
   void float3::operator+= (float3 b)
   {
   x += b.x;
   y += b.y; 
   z += b.z; 
   }
   
   void float3::operator/= (float b)
   {
   x /= b;
   y /= b; 
   z /= b; 
   }
   
   void float3::operator*= (float b)
   {
   x *= b;
   y *= b; 
   z *= b; 
   }
   
   bool float3::operator!= (float3 b)
   {
   return (x != b.x) || (y != b.y) || (z != b.z);
   }
   
   float3 float3::operator- ()
   {
   return float3(x * -1.0f, y * -1.0f, z * -1.0f);
   }
   
   float3::operator double3()
   {
   return double3((double)x, (double)y, (double)z);
   }
   
   void float3::normalize(void)
   {
   float tmp = sqrt(x*x + y*y + z*z);
   if (tmp) 
      {			
      x /= tmp;
      y /= tmp;
      z /= tmp;
      }
   }
   
   float float3::length(void)
   {
   return sqrt(x*x + y*y + z*z);
   }
}