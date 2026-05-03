/*

 Ngine v5.0
 
 Module      : Types.
 Requirements: none
 Description : 128 bit unsigned integer.

*/

#include "core/types/uint128.h"

namespace en
{

uint128::uint128(const void* src)
{
    if (src)
    {
        qword[0] = *(const uint64*)src;
        qword[1] = *((const uint64*)src + 1);
    }
}

uint128& uint128::operator=(const uint128& b)
{
    if (this != &b) 
    { 
        qword[0] = b.qword[0];
        qword[1] = b.qword[1];
    }

    return *this;
}

} // en