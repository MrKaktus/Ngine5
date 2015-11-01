/*

 Ngine v5.0
 
 Module      : Types.
 Requirements: none
 Description : 3x3 float matrix.

*/

#include <string.h>
#include "core/types/float3x3.h"

namespace en
{
   float3x3::float3x3()
   {
   memset(&m[0], 0, 36);
   m[0] = 1.0f;
   m[4] = 1.0f;
   m[8] = 1.0f;
   }
   
   float3x3::float3x3(float* src)
   {
   memcpy(m, src, 36);
   }
   
   float3x3::float3x3(float m00, float m01, float m02, 
                      float m10, float m11, float m12, 
                      float m20, float m21, float m22 )
   {
   m[0] = m00;  m[3] = m01;  m[6] = m02; 
   m[1] = m10;  m[4] = m11;  m[7] = m12;  
   m[2] = m20;  m[5] = m21;  m[8] = m22;  
   } 
   
   void float3x3::set(float* src)
   {
   memcpy(m, src, 36);
   }
}