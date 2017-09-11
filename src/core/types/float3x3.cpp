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
   // Stored in Column-Major order
   #define mat(m,r,c) (m)[(c)*3+(r)]

   // Stored in Row-Major order
   // #define mat(m,r,c) (m)[(r)*3+(c)]

   float3x3::float3x3()
   {
   memset(&m[0], 0, 36);
   mat(m,0,0) = 1.0f;
   mat(m,1,1) = 1.0f;
   mat(m,2,2) = 1.0f;
   }
   
   float3x3::float3x3(const float* src)
   {
   memcpy(m, src, 36);
   }
   
   float3x3::float3x3(const float m00, const float m01, const float m02, 
                      const float m10, const float m11, const float m12, 
                      const float m20, const float m21, const float m22)
   {
   mat(m,0,0) = m00;  mat(m,0,1) = m01;  mat(m,0,2) = m02; 
   mat(m,1,0) = m10;  mat(m,1,1) = m11;  mat(m,1,2) = m12;  
   mat(m,2,0) = m20;  mat(m,2,1) = m21;  mat(m,2,2) = m22;  
   } 
   
   void float3x3::set(const float* src)
   {
   memcpy(m, src, 36);
   }
}