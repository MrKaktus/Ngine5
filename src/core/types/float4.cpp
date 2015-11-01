/*

 Ngine v5.0
 
 Module      : Types.
 Requirements: none
 Description : 4 component float vector.

*/

#include <math.h>
#include <string.h>
#include "core/types/float3.h"
#include "core/types/float4.h"

namespace en
{
   float4::float4(void)
   {
   x = 0.0f;
   y = 0.0f;
   z = 0.0f;
   w = 1.0f;
   }
   
   float4::float4(float* src)
   {
   memcpy(&x, src, 16);
   }
   
   float4::float4(float3 f3, float w)
   {
   this->x = f3.x;
   this->y = f3.y;
   this->z = f3.z;
   this->w = w;
   }
   
   float4::float4(double3 d3, float w)
   {
   this->x = float(d3.x);
   this->y = float(d3.y);
   this->z = float(d3.z);
   this->w = w;
   }
   
   float4::float4(const float x, const float y, const float z, const float w)
   {
   this->x = x;
   this->y = y;
   this->z = z;
   this->w = w;
   }
   
   void float4::operator-= (float4 b)
   {
   x -= b.x;
   y -= b.y; 
   z -= b.z;
   w -= b.w;
   }
   
   void float4::operator+= (float4 b)
   {
   x += b.x;
   y += b.y; 
   z += b.z; 
   w += b.w;
   }
   
   // Channels
   float3 float4::xyz(void)
   {
   return float3(x, y, z);
   }
      
   void float4::normalize(void)
   {
   float tmp = sqrt(x*x + y*y + z*z);
   if (tmp) 
      {			
      x /= tmp;
      y /= tmp;
      z /= tmp;
      }
   }
}