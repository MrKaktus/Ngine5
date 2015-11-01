/*

 Ngine v5.0
 
 Module      : Types.
 Requirements: none
 Description : 3 component float vector.

*/

#ifndef ENG_CORE_TYPES_FLOAT3
#define ENG_CORE_TYPES_FLOAT3

#include "core/types/double3.h"

namespace en
{
   class float3
      {
      public:
      union { float x; float r; float u; };
      union { float y; float g; float v; };
      union { float z; float b; float w; };
      
      float3(void);                       
      float3(const float* src); 
      float3(double3 d3);                
      float3(float x, float y, float z);   
      
      void   operator-= (float3 b);          
      void   operator+= (float3 b);  
      void   operator/= (float b);
      void   operator*= (float b);
      bool   operator!= (float3 b);
      float3 operator-  ();      
      operator double3();
      
      void   normalize(void);               
      float  length(void);
      };
}

#endif
