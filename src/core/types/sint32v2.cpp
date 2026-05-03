/*

 Ngine v5.0
 
 Module      : Types.
 Requirements: none
 Description : 2 component sint32 vector.

*/

#include "core/types/sint32v2.h"

namespace en
{

sint32v2::sint32v2(void) :
    x(0),
    y(0)
{
}

sint32v2::sint32v2(const sint32 _x, const sint32 _y) :
    x(_x),
    y(_y)
{
}

bool sint32v2::operator== (const sint32v2 b) const
{
    return ((x == b.x) && (y == b.y));
}

} // en