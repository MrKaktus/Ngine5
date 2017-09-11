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
   double4::double4(void) :
      x(0.0),
      y(0.0),
      z(0.0),
      w(1.0)
   {
   }
   
   double4::double4(const float3 v3) :
      x(double(v3.x)),
      y(double(v3.y)),
      z(double(v3.z)),
      w(1.0)
   {
   } 
   
   double4::double4(const double3 v3, const double w) :
      x(v3.x),
      y(v3.y),
      z(v3.z),
      w(w)
   {
   }
   
   double4::double4(const double _x, const double _y, const double _z, const double _w) :
      x(_x),
      y(_y),
      z(_z),
      w(_w)
   {
   }
   
   void double4::operator-= (const double4 b)
   {
   x -= b.x;
   y -= b.y; 
   z -= b.z;
   w -= b.w;
   }
   
   void double4::operator+= (const double4 b)
   {
   x += b.x;
   y += b.y; 
   z += b.z; 
   w += b.w;
   }
   
   // Channels
   double3 double4::xyz(void) const
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