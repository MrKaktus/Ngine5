/*

 Ngine v5.0
 
 Module      : Types.
 Requirements: none
 Description : 4 component double vector.

*/

#include <math.h>
#include "core/types/double3.h"
#include "core/types/double4.h"

namespace en
{
   double4::double4(void)
   {
   x = 0.0;
   y = 0.0;
   z = 0.0;
   w = 1.0;
   }
   
   double4::double4(float3 f3)
   {
   x = (double)f3.x;
   y = (double)f3.y;
   z = (double)f3.z;
   w = 1.0;
   } 
   
   double4::double4(double3 d3, double w=1.0)
   {
   this->x = d3.x;
   this->y = d3.y;
   this->z = d3.z;
   this->w = w;
   }
   
   double4::double4(double x=0.0, double y=0.0, double z=0.0, double w=1.0)
   {
   this->x = x;
   this->y = y;
   this->z = z;
   this->w = w;
   }
   
   void double4::operator-= (double4 b)
   {
   x -= b.x;
   y -= b.y; 
   z -= b.z;
   w -= b.w;
   }
   
   void double4::operator+= (double4 b)
   {
   x += b.x;
   y += b.y; 
   z += b.z; 
   w += b.w;
   }
   
   // Channels
   double3 double4::xyz(void)
   {
   return double3(x, y, z);
   }
       
   void double4::normalize(void)
   {
   double tmp = sqrt(x*x + y*y + z*z);
   if (tmp) 
      {			
      x /= tmp;
      y /= tmp;
      z /= tmp;
      }
   }
}