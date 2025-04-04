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

// Stored in Column-Major order
class float4x4
{
    public:
    float m[16];
   
    float4x4(); // Matrix is set to identity matrix
    float4x4(const float* src); 
    float4x4(const float m00, const float m01, const float m02, const float m03,
             const float m10, const float m11, const float m12, const float m13,
             const float m20, const float m21, const float m22, const float m23,
             const float m30, const float m31, const float m32, const float m33); 

    // Generates cumulated scale, rotation and translation matrix.
    // Equations are performed from right to left, as follows:
    //
    // m = T * (r(X) * r(Y) * r(Z)) * S
    //
    // Right handed coordinate system is assumed.
    float4x4(const float3 translation, const float3 rotation, const float3 scale);
   
    void     set(const float* src);
    float4   row(const uint8 r) const;
    float4   column(const uint8 c) const;
    void     column(const uint8 c, const float4 v4);
    void     column(const uint8 c, const float3 v3);
    void     diagonal(const float4 v4);
    void     diagonal(const float3 v3);
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
        float a = cosf(radians(d));
        float b = sinf(radians(d));
   
        return float4x4( 1,  0,  0,  0,
                         0,  a, -b,  0,
                         0,  b,  a,  0,
                         0,  0,  0,  1 );
    }
   
    static float4x4 rotationY(float degrees)
    {
        float c = cosf(radians(degrees));
        float d = sinf(radians(degrees));
   
        return float4x4( c,  0,  d,  0,
                         0,  1,  0,  0,
                        -d,  0,  c,  0,
                         0,  0,  0,  1 );
    }
   
    static float4x4 rotationZ(float degrees)
    {
         float e = cosf(radians(degrees));
         float f = sinf(radians(degrees));

         return float4x4( e, -f,  0,  0,
                          f,  e,  0,  0,
                          0,  0,  1,  0,
                          0,  0,  0,  1 );
    }
   
    static float4x4 rotation(float3 rotation)
    {
        float a = cosf(radians(rotation.x));
        float b = sinf(radians(rotation.x));
        float c = cosf(radians(rotation.y));
        float d = sinf(radians(rotation.y));
        float e = cosf(radians(rotation.z));
        float f = sinf(radians(rotation.z));
   
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
        float a = cosf(radians(pitch));
        float b = sinf(radians(pitch));
        float c = cosf(radians(yaw));
        float d = sinf(radians(yaw));
        float e = cosf(radians(roll));
        float f = sinf(radians(roll));
   
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

} // en
#endif
