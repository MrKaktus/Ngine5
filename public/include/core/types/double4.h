/*

 Ngine v5.0
 
 Module      : Types.
 Requirements: none
 Description : 4 component double vector.

*/

#ifndef ENG_CORE_TYPES_DOUBLE4
#define ENG_CORE_TYPES_DOUBLE4

#include "core/types/float3.h"

namespace en
{
   class double4
      {
      public:
      union { double x; double r; };
      union { double y; double g; };
      union { double z; double b; };
      union { double w; double a; };
      
      double4(void);                                   
      double4(float3 f3);
      double4(double3 d3, double w);                   
      double4(double x, double y, double z, double w); 
      
      void operator-= (double4 b);      
      void operator+= (double4 b);         
      
      double3 xyz(void);
      void normalize(void);                       
      };
}

#endif
