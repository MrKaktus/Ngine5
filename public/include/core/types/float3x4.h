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
   // Stored in Column-Major order
   class float3x4
      {
      public:
      float m[12];
      
      float3x4();           
      float3x4(const float* src); 
      float3x4(const float m00, const float m01, const float m02, const float m03,
               const float m10, const float m11, const float m12, const float m13,
               const float m20, const float m21, const float m22, const float m23); 
      
      void   set(const float* src);
      float4 row(const uint8 r) const;
      float3 column(const uint8 c) const;
      void   column(const uint8 c, const float3 f3);
      };
}

#endif
