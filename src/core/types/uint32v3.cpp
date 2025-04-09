/*

 Ngine v5.0
 
 Module      : Types.
 Requirements: none
 Description : 3 component uint32 vector.

*/

#include "core/types/uint32v3.h"

namespace en
{

uint32v3::uint32v3(void) :
    x(0),
    y(0),
    z(0)
{
}

uint32v3::uint32v3(const uint32 _x, const uint32 _y, const uint32 _z) :
    x(_x),
    y(_y),
    z(_z)
{
}

} // en