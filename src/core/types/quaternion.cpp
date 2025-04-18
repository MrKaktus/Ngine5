/*

 Ngine v5.0
 
 Module      : Types.
 Requirements: none
 Description : Quaternion versor.

*/

#include <math.h>   // sin, cos
#include <string.h> // memset, memcmp

#include "core/types/quaternion.h"
#include "utilities/gpcpu/gpcpu.h"

namespace en
{

Quaternion::Quaternion(void)
{
    memset(this, 0, sizeof(Quaternion));
}
   
Quaternion::Quaternion(const float degrees, const float3 vector)
{
    float angle = radians(degrees) / 2.0f;
    float sinus = sinf(angle);
    q0 = cosf(angle);
    q1 = vector.x * sinus;
    q2 = vector.y * sinus;
    q3 = vector.z * sinus;
}
   
Quaternion::~Quaternion(void)
{
}

float Quaternion::magnitude(void) const
{
    return sqrtf(s*s + x*x + y*y + z*z);
}
   
void Quaternion::normalize(void)
{
    float temp = magnitude();
    q0 /= temp;
    q1 /= temp;
    q2 /= temp;
    q3 /= temp;
}
   
float4x4 Quaternion::matrix(void)
{
    float4x4 m;
   
    float xx = x * x;
    float xy = x * y;
    float xz = x * z;
    float xs = x * s;
   
    float yy = y * y;
    float yz = y * z;
    float ys = y * s;
   
    float zz = z * z;
    float zs = z * s;
   
    m.m[0] = 1.0f - 2.0f * ( yy + zz );   m.m[4] =        2.0f * ( xy + zs );   m.m[8]  =        2.0f * ( xz - ys );   m.m[12] = 0.0f;
    m.m[1] =        2.0f * ( xy - zs );   m.m[5] = 1.0f - 2.0f * ( xx + zz );   m.m[9]  =        2.0f * ( yz + xs );   m.m[13] = 0.0f;
    m.m[2] =        2.0f * ( xz + ys );   m.m[6] =        2.0f * ( yz - xs );   m.m[10] = 1.0f - 2.0f * ( xx + yy );   m.m[14] = 0.0f;
    m.m[3] = 0.0f;                        m.m[7] = 0.0f;                        m.m[11] = 0.0f;                        m.m[15] = 1.0f;
   
    return m;
}
   
bool operator ==(const Quaternion a, const Quaternion b)
{
    return memcmp(static_cast<const void*>(&a), static_cast<const void*>(&b), sizeof(Quaternion)) == 0;
}
   
Quaternion operator+ (const Quaternion a, const Quaternion b)
{
    Quaternion temp;
   
    temp.q0 = a.q0 + b.q0;
    temp.q1 = a.q1 + b.q1;
    temp.q2 = a.q2 + b.q2;
    temp.q3 = a.q3 + b.q3;
   
    return temp;
}
   
Quaternion operator- (const Quaternion a, const Quaternion b)
{
    Quaternion temp;
   
    temp.q0 = a.q0 - b.q0;
    temp.q1 = a.q1 - b.q1;
    temp.q2 = a.q2 - b.q2;
    temp.q3 = a.q3 - b.q3;
   
    return temp;
}
   
Quaternion mul(const Quaternion a, const Quaternion b)
{
    Quaternion temp;
   
    temp.q0 = (b.q0 * a.q0) - (b.q1 * a.q1) - (b.q2 * a.q2) - (b.q3 * a.q3);
    temp.q1 = (b.q0 * a.q1) + (b.q1 * a.q0) - (b.q2 * a.q3) + (b.q3 * a.q2);
    temp.q2 = (b.q0 * a.q2) + (b.q1 * a.q3) + (b.q2 * a.q0) - (b.q3 * a.q1);
    temp.q3 = (b.q0 * a.q3) - (b.q1 * a.q2) + (b.q2 * a.q1) + (b.q3 * a.q0);
   
    return temp;
}

} // en