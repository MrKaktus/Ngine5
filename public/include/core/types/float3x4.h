/*

 Ngine v5.0
 
 Module      : Types
 Requirements: none
 Description : 3x4 float matrix.

*/

#ifndef ENG_CORE_TYPES_FLOAT3x4
#define ENG_CORE_TYPES_FLOAT3x4

#include "core/types.h"
#include "core/types/float3.h"
#include "core/types/float4.h"

namespace en
{
   class float3x4
      {
      public:
      float m[12];
      
      float3x4();           
      float3x4(float* src); 
      float3x4(float m00, float m01, float m02, float m03,
               float m10, float m11, float m12, float m13,
               float m20, float m21, float m22, float m23 ); 
      
      void   set(float* src);
      float4 row(uint8 r);
      float3 column(uint8 c);
      void   column(uint8 c, float3 f3);
      };
}

#endif
