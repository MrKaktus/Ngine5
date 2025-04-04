/*

 Ngine v5.0
 
 Module      : Types.
 Requirements: none
 Description : 128 bit unsigned integer.

*/

#ifndef ENG_CORE_TYPES_UINT128
#define ENG_CORE_TYPES_UINT128

#include "core/types/basic.h"

namespace en
{

class uint128
{
    public:
    uint64 lo;
    uint64 hi;

    inline uint128() : lo(0), hi(0) {}
    uint128(const void* src);
    inline uint128(uint64 hi, uint64 lo) : lo(lo), hi(hi) {}

    // copy assignment
    inline void operator=(const uint8& b)  { hi = 0; lo = b; };
    inline void operator=(const uint16& b) { hi = 0; lo = b; };
    inline void operator=(const uint32& b) { hi = 0; lo = b; };
    inline void operator=(const uint64& b) { hi = 0; lo = b; };
    uint128& operator=(const uint128& b);

    // comparison
    inline bool operator< (const uint128& rhs) const { return hi == rhs.hi ? lo < rhs.lo : hi < rhs.hi; }
    inline bool operator> (const uint128& rhs) const { return rhs < *this; }
    inline bool operator<=(const uint128& rhs) const { return !(*this > rhs); }
    inline bool operator>=(const uint128& rhs) const { return !(*this < rhs); }
    inline bool operator==(const uint128& rhs) const { return (hi == rhs.hi) && (lo == rhs.lo); }
    inline bool operator!=(const uint128& rhs) const { return !(*this == rhs); }

    // TODO: Rest of operators
    //void    operator-= (const uint128 b);          
    //void    operator+= (const uint128 b);  
    //void    operator/= (const uint128 b);
    //void    operator*= (const uint128 b);
    //bool    operator!= (const uint128 b) const;
    //uint128 operator-  () const;      
};
   
static_assert(sizeof(uint128) == 16, "en::uint128 size mismatch!");

} // en

#endif
