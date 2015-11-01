/*

 Ngine v5.0
 
 Module      : Types.
 Requirements: none
 Description : 3x4 float matrix.

*/

#include <string.h>
#include "core/types/float3x4.h"

namespace en
{
   float3x4::float3x4()
   {
   memset(&m[0], 0, 48);
   m[0] = 1.0f;
   m[4] = 1.0f;
   m[8] = 1.0f;
   }
   
   float3x4::float3x4(float* src)
   {
   memcpy(m, src, 48);
   }
   
   float3x4::float3x4(float m00, float m01, float m02, float m03,
                      float m10, float m11, float m12, float m13,
                      float m20, float m21, float m22, float m23 )
   {
   m[0] = m00;  m[3] = m01;  m[6] = m02;  m[9]  = m03;
   m[1] = m10;  m[4] = m11;  m[7] = m12;  m[10] = m13;
   m[2] = m20;  m[5] = m21;  m[8] = m22;  m[11] = m23;
   } 
   
   void float3x4::set(float* src)
   {
   memcpy(m, src, 48);
   }
   
   float4 float3x4::row(uint8 r)
   {
   r %= 3;
   return float4(m[r], m[r+3], m[r+6], m[r+9]);
   }
   
   float3 float3x4::column(uint8 c)
   {
   c %= 4;
   return float3(m[c*3], m[c*3+1], m[c*3+2]);
   }
   
   void float3x4::column(uint8 c, float3 f3)
   {
   c %= 4;
   m[c*3]   = f3.x;
   m[c*3+1] = f3.y;
   m[c*3+2] = f3.z;
   }
}
