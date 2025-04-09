/*

 Ngine v5.0
 
 Module      : Types.
 Requirements: none
 Description : 3x4 float matrix.

*/

#include <string.h>
#include "assert.h"
#include "core/types/float3x4.h"

namespace en
{

// Stored in Column-Major order
#define mat(m,r,c) (m)[(c)*3+(r)]

// Stored in Row-Major order
// #define mat(m,r,c) (m)[(r)*4+(c)]

float3x4::float3x4()
{
    memset(&m[0], 0, 48);
    mat(m,0,0) = 1.0f;
    mat(m,1,1) = 1.0f;
    mat(m,2,2) = 1.0f;
}
   
float3x4::float3x4(const float* src)
{
    memcpy(m, src, 48);
}

float3x4::float3x4(const float m00, const float m01, const float m02, const float m03,
                   const float m10, const float m11, const float m12, const float m13,
                   const float m20, const float m21, const float m22, const float m23)
{
    mat(m,0,0) = m00;  mat(m,0,1) = m01;  mat(m,0,2) = m02;  mat(m,0,3) = m03;
    mat(m,1,0) = m10;  mat(m,1,1) = m11;  mat(m,1,2) = m12;  mat(m,1,3) = m13;
    mat(m,2,0) = m20;  mat(m,2,1) = m21;  mat(m,2,2) = m22;  mat(m,2,3) = m23;
} 

void float3x4::set(const float* src)
{
    memcpy(m, src, 48);
}

float4 float3x4::row(const uint8 r) const
{
    assert( r < 3 );
    return float4(mat(m,r,0), mat(m,r,1), mat(m,r,2), mat(m,r,3));
}

float3 float3x4::column(const uint8 c) const
{
    assert( c < 4 );
    return float3(mat(m,0,c), mat(m,1,c), mat(m,2,c));
}

void float3x4::column(const uint8 c, const float3 v3)
{
    assert( c < 4 );
    mat(m,0,c) = v3.x;
    mat(m,1,c) = v3.y;
    mat(m,2,c) = v3.z;
}

} // en
