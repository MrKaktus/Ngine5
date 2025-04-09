/*

 Ngine v5.0
 
 Module      : Types.
 Requirements: none
 Description : 3 component double vector.

*/

#include <math.h>
#include "core/types/double3.h"

namespace en
{

double3::double3(void) :
    x(0.0),
    y(0.0),
    z(0.0)
{
}

double3::double3(const double _x, const double _y, const double _z) :
    x(_x),
    y(_y),
    z(_z)
{
}

void double3::operator-= (const double3 b)
{
    x -= b.x;
    y -= b.y; 
    z -= b.z;
}
   
void double3::operator/= (const double b)
{
    x /= b;
    y /= b; 
    z /= b; 
}

void double3::operator+= (const double3 b)
{
    x += b.x;
    y += b.y; 
    z += b.z; 
}

void double3::operator*= (const double b)
{
    x *= b;
    y *= b; 
    z *= b; 
}

bool double3::operator== (const double3 b) const
{
    return (x == b.x) && (y == b.y) && (z == b.z);
}

bool double3::operator!= (const double3 b) const
{
    return (x != b.x) || (y != b.y) || (z != b.z);
}

double3 double3::operator- () const
{
    return double3(x * -1.0, y * -1.0, z * -1.0);
}

void double3::normalize(void)
{
    double tmp = sqrt(x*x + y*y + z*z);
    if (tmp) 
    {			
        x /= tmp;
        y /= tmp;
        z /= tmp;
    }
}
   
double double3::length(void) const
{
    return sqrt(x*x + y*y + z*z);
}

} // en