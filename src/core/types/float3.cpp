/*

 Ngine v5.0
 
 Module      : Types.
 Requirements: none
 Description : 3 component float vector.

*/

#include <math.h>
#include <string.h>
#include "core/types/float3.h"

namespace en
{

float3::float3(void)
{
    memset(&x, 0, 12);
}

float3::float3(const float* src)
{
    memcpy(&x, src, 12);
}

float3::float3(const double3 v3) :
    x(float(v3.x)),
    y(float(v3.y)),
    z(float(v3.z))
{
}

float3::float3(const float _x, const float _y, const float _z) :
    x(_x),
    y(_y),
    z(_z)
{
}

void float3::operator-= (const float3 b)
{
    x -= b.x;
    y -= b.y; 
    z -= b.z;
}

void float3::operator+= (const float3 b)
{
    x += b.x;
    y += b.y; 
    z += b.z; 
}

void float3::operator/= (const float b)
{
    x /= b;
    y /= b; 
    z /= b; 
}

void float3::operator*= (const float b)
{
    x *= b;
    y *= b; 
    z *= b; 
}

bool float3::operator!= (const float3 b) const
{
    return (x != b.x) || (y != b.y) || (z != b.z);
}

float3 float3::operator- () const
{
    return float3(x * -1.0f, y * -1.0f, z * -1.0f);
}

float3::operator double3() const
{
    return double3((double)x, (double)y, (double)z);
}

void float3::normalize(void)
{
    float tmp = sqrt(x*x + y*y + z*z);
    if (tmp) 
    {			
        x /= tmp;
        y /= tmp;
        z /= tmp;
    }
}
   
float float3::length(void) const
{
    return sqrt(x*x + y*y + z*z);
}

} // en