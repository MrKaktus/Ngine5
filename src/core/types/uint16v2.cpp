/*

 Ngine v5.0
 
 Module      : Types.
 Requirements: none
 Description : 2 component uint16 vector.

*/

#include "core/types/uint16v2.h"

namespace en
{

uint16v2::uint16v2(void) :
    x(0),
    y(0)
{
}

uint16v2::uint16v2(const uint16 _x, const uint16 _y) :
    x(_x),
    y(_y)
{
}

bool uint16v2::operator== (const uint16v2 b) const
{
    return ((x == b.x) && (y == b.y));
}

} // en