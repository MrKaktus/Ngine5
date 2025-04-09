/*

 Ngine v5.0
 
 Module      : Types.
 Requirements: none
 Description : 4 component float vector.

*/

#include <math.h>
#include <string.h>
#include "core/types/float3.h"
#include "core/types/float4.h"

namespace en
{

float4::float4(void) :
    x(0.0f),
    y(0.0f),
    z(0.0f),
    w(1.0f)
{
}

float4::float4(const float* src)
{
    memcpy(&x, src, 16);
}

float4::float4(const float3 v3, const float _w) :
    x(v3.x),
    y(v3.y),
    z(v3.z),
    w(_w)
{
}

float4::float4(const double3 v3, const float _w) :
    x(float(v3.x)),
    y(float(v3.y)),
    z(float(v3.z)),
    w(_w)
{
}

float4::float4(const float _x, const float _y, const float _z, const float _w) :
    x(_x),
    y(_y),
    z(_z),
    w(_w)
{
}

void float4::operator= (const float4 b)
{
    x = b.x;
    y = b.y;
    z = b.z;
    w = b.w;
}
   
void float4::operator-= (const float4 b)
{
    x -= b.x;
    y -= b.y; 
    z -= b.z;
    w -= b.w;
}

void float4::operator+= (const float4 b)
{
    x += b.x;
    y += b.y; 
    z += b.z; 
    w += b.w;
}

// Channels
float3 float4::xyz(void) const
{
    return float3(x, y, z);
}
   
void float4::normalize(void)
{
    float tmp = sqrt(x*x + y*y + z*z);
    if (tmp) 
    {			
        x /= tmp;
        y /= tmp;
        z /= tmp;
    }
}

} // en
