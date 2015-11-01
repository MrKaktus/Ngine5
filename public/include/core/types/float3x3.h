/*

 Ngine v5.0
 
 Module      : Types.
 Requirements: none
 Description : 3x3 float matrix.

*/

#ifndef ENG_CORE_TYPES_FLOAT3x3
#define ENG_CORE_TYPES_FLOAT3x3

namespace en
{
   class float3x3
      {
      public:
      float m[9];
      
      float3x3();           
      float3x3(float* src); 
      float3x3(float m00, float m01, float m02, 
               float m10, float m11, float m12, 
               float m20, float m21, float m22 ); 
      
      void set(float* src);
      };
}

#endif
