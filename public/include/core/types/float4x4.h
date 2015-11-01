/*

 Ngine v5.0
 
 Module      : Types.
 Requirements: none
 Description : 4x4 float matrix.

*/

#ifndef ENG_CORE_TYPES_FLOAT4X4
#define ENG_CORE_TYPES_FLOAT4X4

#include <math.h>
#include "core/types/basic.h"
#include "core/types/float4.h"
#include "utilities/utilities.h"

namespace en
{
   class float4x4
      {
      public:
      float m[16];
      
      float4x4();
      float4x4(float* src); 
      float4x4(float3 translation, float3 rotation, float3 scale);
      float4x4(float m00, float m01, float m02, float m03,
               float m10, float m11, float m12, float m13,
               float m20, float m21, float m22, float m23,
               float m30, float m31, float m32, float m33 ); 
      
      void     set(float* src);
      float4   row(uint8 r);
      float4   column(uint8 c);
      void     column(uint8 c, float4 f4);
      void     column(uint8 c, float3 f3);
      void     diagonal(float4 f4);
      void     diagonal(float3 f3);
      float4x4 invert(void);
      
      static float4x4 translation(float3 vector)
      {
      return float4x4( 1, 0, 0, vector.x,
                       0, 1, 0, vector.y,
                       0, 0, 1, vector.z,
                       0, 0, 0, 1 );
      }
      
      static float4x4 rotationX(float d)
      {
      float a = cos(radians(d));
      float b = sin(radians(d));
      
      return float4x4( 1,  0,  0,  0,
                       0,  a, -b,  0,
                       0,  b,  a,  0,
                       0,  0,  0,  1 );
      }
      
      static float4x4 rotationY(float degrees)
      {
      float c = cos(radians(degrees));
      float d = sin(radians(degrees));
      
      return float4x4( c,  0,  d,  0,
                       0,  1,  0,  0,
                      -d,  0,  c,  0,
                       0,  0,  0,  1 );
      }
      
      static float4x4 rotationZ(float degrees)
      {
      float e = cos(radians(degrees));
      float f = sin(radians(degrees));
      
      return float4x4( e, -f,  0,  0,
                       f,  e,  0,  0,
                       0,  0,  1,  0,
                       0,  0,  0,  1 );
      }
      
      static float4x4 rotation(float3 rotation)
      {
      float a = cos(radians(rotation.x));
      float b = sin(radians(rotation.x));
      float c = cos(radians(rotation.y));
      float d = sin(radians(rotation.y));
      float e = cos(radians(rotation.z));
      float f = sin(radians(rotation.z));
      
      float ad = a * d;
      float bd = b * d;
      
      // Matrix construction:
      // m = r(X) * r(Y) * r(Z)
      // Equations are performed from right to left.
      return float4x4 ( ( c * e ),  ( bd * e + a * f ),  (-ad * e + b * f ),  0.0f,
                        (-c * f ),  (-bd * f + a * e ),  ( ad * f + b * e ),  0.0f,
                          d      ,  ( -b * c )        ,  (  a * c )        ,  0.0f,
                        0.0f     ,  0.0f              ,  0.0f              ,  1.0f );
      }
      
      static float4x4 rotation(float pitch, float yaw, float roll)
      {
      float a = cos(radians(pitch));
      float b = sin(radians(pitch));
      float c = cos(radians(yaw));
      float d = sin(radians(yaw));
      float e = cos(radians(roll));
      float f = sin(radians(roll));
      
      float ad = a * d;
      float bd = b * d;
      
      // Matrix construction:
      // m = r(X) * r(Y) * r(Z)
      // Equations are performed from right to left.
      return float4x4( ( c * e ),  ( bd * e + a * f ),  (-ad * e + b * f ),  0.0f,
                       (-c * f ),  (-bd * f + a * e ),  ( ad * f + b * e ),  0.0f,
                         d      ,  ( -b * c )        ,  (  a * c )        ,  0.0f,
                       0.0f     ,  0.0f              ,  0.0f              ,  1.0f );
      }
      
      static float4x4 scale(float3 scale)
      {
      return float4x4( scale.x, 0,       0,       0,
                       0,       scale.y, 0,       0,
                       0,       0,       scale.z, 0,
                       0,       0,       0,       1 );
      }
      
      };
}

#endif
