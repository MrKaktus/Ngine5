/*

 Ngine v5.0
 
 Module      : Types.
 Requirements: none
 Description : 4x4 float matrix.

*/

#include <assert.h>
#include <math.h>
#include <string.h>
#include "core/types.h"
#include "core/types/float3.h"
#include "core/types/float4.h"
#include "core/types/float4x4.h"
#include "utilities/gpcpu/gpcpu.h"

namespace en
{

// Stored in Column-Major order
#define mat(m,r,c) (m)[(c)*4+(r)]

// Stored in Row-Major order
// #define mat(m,r,c) (m)[(r)*4+(c)]

float4x4::float4x4()
{
    memset(m, 0, 64);

    mat(m,0,0) = 1.0f;
    mat(m,1,1) = 1.0f;
    mat(m,2,2) = 1.0f;
    mat(m,3,3) = 1.0f;
}

float4x4::float4x4(const float* src)
{
    memcpy(m, src, 64);
}

float4x4::float4x4(const float m00, const float m01, const float m02, const float m03,
                   const float m10, const float m11, const float m12, const float m13,
                   const float m20, const float m21, const float m22, const float m23,
                   const float m30, const float m31, const float m32, const float m33 )
{
    mat(m,0,0) = m00;  mat(m,0,1) = m01;  mat(m,0,2) = m02;  mat(m,0,3) = m03;
    mat(m,1,0) = m10;  mat(m,1,1) = m11;  mat(m,1,2) = m12;  mat(m,1,3) = m13;
    mat(m,2,0) = m20;  mat(m,2,1) = m21;  mat(m,2,2) = m22;  mat(m,2,3) = m23;
    mat(m,3,0) = m30;  mat(m,3,1) = m31;  mat(m,3,2) = m32;  mat(m,3,3) = m33;
} 

float4x4::float4x4(const float3 translation, const float3 rotation, const float3 s)
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
    // m = T * (r(X) * r(Y) * r(Z)) * S
    // Equations are performed from right to left.
   
    //m[0] = ( c * e ) * s.x;  m[4] = ( bd * e + a * f ) * s.y;  m[8]  = (-ad * e + b * f ) * s.z;  m[12] = translation.x;
    //m[1] = (-c * f ) * s.x;  m[5] = (-bd * f + a * e ) * s.y;  m[9]  = ( ad * f + b * e ) * s.z;  m[13] = translation.y;
    //m[2] =   d       * s.x;  m[6] = ( -b * c )         * s.y;  m[10] = (  a * c )         * s.z;  m[14] = translation.z;
    //m[3] = 0.0f;             m[7] = 0.0f;                      m[11] = 0.0f;                      m[15] = 1.0f;
   
   
    float4x4 T( 1, 0, 0, translation.x,
                0, 1, 0, translation.y,
                0, 0, 1, translation.z,
                0, 0, 0, 1 );
   
    //float4x4 RX( 1,  0,  0,  0,
    //             0,  a, -b,  0,
    //             0,  b,  a,  0,
    //             0,  0,  0,  1 );
    //
    //float4x4 RY( c,  0,  d,  0,
    //             0,  1,  0,  0,
    //            -d,  0,  c,  0,
    //             0,  0,  0,  1 );
    //
    //float4x4 RZ( e, -f,  0,  0,
    //             f,  e,  0,  0,
    //             0,  0,  1,  0,
    //             0,  0,  0,  1 );
   
    float4x4 R( ( c * e ),  ( bd * e + a * f ),  (-ad * e + b * f ),  0.0f,
                (-c * f ),  (-bd * f + a * e ),  ( ad * f + b * e ),  0.0f,
                  d      ,  ( -b * c )        ,  (  a * c )        ,  0.0f,
                0.0f     ,  0.0f              ,  0.0f              ,  1.0f );
   
    float4x4 S( s.x, 0,   0,   0,
                0,   s.y, 0,   0,
                0,   0,   s.z, 0,
                0,   0,   0,   1 );
   
    *this = mul( T, mul( R , S ) );
} 

void float4x4::set(const float* src)
{
    memcpy(m, src, 64);
}
   
float4 float4x4::row(const uint8 r) const
{
    assert( r < 4 );
    return float4(mat(m,r,0), mat(m,r,1), mat(m,r,2), mat(m,r,3));
}
   
float4 float4x4::column(const uint8 c) const
{
    assert( c < 4 );
    return float4(mat(m,0,c), mat(m,1,c), mat(m,2,c), mat(m,3,c));
}
   
void float4x4::column(const uint8 c, const float4 v4)
{
    assert( c < 4 );
    mat(m,0,c) = v4.x;
    mat(m,1,c) = v4.y;
    mat(m,2,c) = v4.z;
    mat(m,3,c) = v4.w;
}
   
void float4x4::column(const uint8 c, const float3 v3)
{
    assert( c < 4 );
    mat(m,0,c) = v3.x;
    mat(m,1,c) = v3.y;
    mat(m,2,c) = v3.z;
}
   
void float4x4::diagonal(const float4 f4)
{
    mat(m,0,0) = f4.x;
    mat(m,1,1) = f4.y;
    mat(m,2,2) = f4.z;
    mat(m,3,3) = f4.w;
}
   
void float4x4::diagonal(const float3 f3)
{
    mat(m,0,0) = f3.x;
    mat(m,1,1) = f3.y;
    mat(m,2,2) = f3.z;
    mat(m,3,3) = 1.0f;
}
   
float4x4 float4x4::invert(void)
{
    // TODO: Write it by partitioning method and using SIMD

    float4x4 f44Res;
    float* m = this->m;
    float* out = f44Res.m;

#define SWAP_ROWS(a, b) { float *_tmp = a; (a)=(b); (b)=_tmp; }

    float wtmp[4][8];
    float m0, m1, m2, m3, s;
    float *r0, *r1, *r2, *r3;

    r0 = wtmp[0], r1 = wtmp[1], r2 = wtmp[2], r3 = wtmp[3];

    r0[0] = mat(m,0,0); r0[1] = mat(m,0,1); r0[2] = mat(m,0,2); r0[3] = mat(m,0,3);
    r1[0] = mat(m,1,0); r1[1] = mat(m,1,1); r1[2] = mat(m,1,2); r1[3] = mat(m,1,3);   
    r2[0] = mat(m,2,0); r2[1] = mat(m,2,1); r2[2] = mat(m,2,2); r2[3] = mat(m,2,3);
    r3[0] = mat(m,3,0); r3[1] = mat(m,3,1); r3[2] = mat(m,3,2); r3[3] = mat(m,3,3);

    r0[4] = 1.0; r0[5] = r0[6] = r0[7] = 0.0;
    r1[5] = 1.0; r1[4] = r1[6] = r1[7] = 0.0;
    r2[6] = 1.0; r2[4] = r2[5] = r2[7] = 0.0;
    r3[7] = 1.0; r3[4] = r3[5] = r3[6] = 0.0;

    /* Choose myPivot, or die. */
    if (fabs(r3[0])>fabs(r2[0])) SWAP_ROWS(r3, r2);
    if (fabs(r2[0])>fabs(r1[0])) SWAP_ROWS(r2, r1);
    if (fabs(r1[0])>fabs(r0[0])) SWAP_ROWS(r1, r0);
    if (0.0 == r0[0])
    {
#ifdef _DEBUG
        return float4x4(); //  CRITCAL
#endif
    }

    /* Eliminate first variable. */
    m1 = r1[0]/r0[0]; m2 = r2[0]/r0[0]; m3 = r3[0]/r0[0];
    s = r0[1]; r1[1] -= m1 * s; r2[1] -= m2 * s; r3[1] -= m3 * s;
    s = r0[2]; r1[2] -= m1 * s; r2[2] -= m2 * s; r3[2] -= m3 * s;
    s = r0[3]; r1[3] -= m1 * s; r2[3] -= m2 * s; r3[3] -= m3 * s;
    s = r0[4];
    if (s != 0.0) { r1[4] -= m1 * s; r2[4] -= m2 * s; r3[4] -= m3 * s; }
    s = r0[5];
    if (s != 0.0) { r1[5] -= m1 * s; r2[5] -= m2 * s; r3[5] -= m3 * s; }
    s = r0[6];
    if (s != 0.0) { r1[6] -= m1 * s; r2[6] -= m2 * s; r3[6] -= m3 * s; }
    s = r0[7];
    if (s != 0.0) { r1[7] -= m1 * s; r2[7] -= m2 * s; r3[7] -= m3 * s; }

    /* Choose myPivot, or die. */
    if (fabs(r3[1])>fabs(r2[1])) SWAP_ROWS(r3, r2);
    if (fabs(r2[1])>fabs(r1[1])) SWAP_ROWS(r2, r1);
    if (0.0 == r1[1])
    {
#ifdef _DEBUG
        return float4x4(); //  CRITCAL
#endif
    }

    /* Eliminate second variable. */
    m2 = r2[1]/r1[1]; m3 = r3[1]/r1[1];
    r2[2] -= m2 * r1[2]; r3[2] -= m3 * r1[2];
    r2[3] -= m2 * r1[3]; r3[3] -= m3 * r1[3];
    s = r1[4]; if (0.0 != s) { r2[4] -= m2 * s; r3[4] -= m3 * s; }
    s = r1[5]; if (0.0 != s) { r2[5] -= m2 * s; r3[5] -= m3 * s; }
    s = r1[6]; if (0.0 != s) { r2[6] -= m2 * s; r3[6] -= m3 * s; }
    s = r1[7]; if (0.0 != s) { r2[7] -= m2 * s; r3[7] -= m3 * s; }

    /* Choose myPivot, or die. */
    if (fabs(r3[2])>fabs(r2[2])) SWAP_ROWS(r3, r2);
    if (0.0 == r2[2])
    {
#ifdef _DEBUG
        return float4x4(); //  CRITCAL
#endif
    }

    // Eliminate third variable
    m3 = r3[2]/r2[2];
    r3[3] -= m3 * r2[3], r3[4] -= m3 * r2[4],
        r3[5] -= m3 * r2[5], r3[6] -= m3 * r2[6],
        r3[7] -= m3 * r2[7];

    // Last check
    if (0.0f == r3[3])
    {
#ifdef _DEBUG
        return float4x4(); //  CRITCAL
#endif
    }

    s = 1.0f/r3[3];              /* Now back substitute row 3. */
    r3[4] *= s; r3[5] *= s; r3[6] *= s; r3[7] *= s;

    m2 = r2[3];                 /* Now back substitute row 2. */
    s  = 1.0f/r2[2];
    r2[4] = s * (r2[4] - r3[4] * m2), r2[5] = s * (r2[5] - r3[5] * m2),
        r2[6] = s * (r2[6] - r3[6] * m2), r2[7] = s * (r2[7] - r3[7] * m2);
    m1 = r1[3];
    r1[4] -= r3[4] * m1, r1[5] -= r3[5] * m1,
        r1[6] -= r3[6] * m1, r1[7] -= r3[7] * m1;
    m0 = r0[3];
    r0[4] -= r3[4] * m0, r0[5] -= r3[5] * m0,
        r0[6] -= r3[6] * m0, r0[7] -= r3[7] * m0;

    m1 = r1[2];                 /* Now back substitute row 1. */
    s  = 1.0f/r1[1];
    r1[4] = s * (r1[4] - r2[4] * m1), r1[5] = s * (r1[5] - r2[5] * m1),
        r1[6] = s * (r1[6] - r2[6] * m1), r1[7] = s * (r1[7] - r2[7] * m1);
    m0 = r0[2];
    r0[4] -= r2[4] * m0, r0[5] -= r2[5] * m0,
        r0[6] -= r2[6] * m0, r0[7] -= r2[7] * m0;

    m0 = r0[1];                 /* Now back substitute row 0. */
    s  = 1.0f/r0[0];
    r0[4] = s * (r0[4] - r1[4] * m0), r0[5] = s * (r0[5] - r1[5] * m0),
        r0[6] = s * (r0[6] - r1[6] * m0), r0[7] = s * (r0[7] - r1[7] * m0);

    mat(out,0,0) = r0[4]; mat(out,0,1) = r0[5]; mat(out,0,2) = r0[6]; mat(out,0,3) = r0[7];
    mat(out,1,0) = r1[4]; mat(out,1,1) = r1[5]; mat(out,1,2) = r1[6]; mat(out,1,3) = r1[7];
    mat(out,2,0) = r2[4]; mat(out,2,1) = r2[5]; mat(out,2,2) = r2[6]; mat(out,2,3) = r2[7];
    mat(out,3,0) = r3[4]; mat(out,3,1) = r3[5]; mat(out,3,2) = r3[6]; mat(out,3,3) = r3[7]; 

#undef SWAP_ROWS

    return f44Res;
}

}
