/*

 Ngine v5.0
 
 Module      : Types.
 Requirements: none
 Description : 128 bit unsigned integer.

*/

#ifndef ENG_CORE_TYPES_UINT128
#define ENG_CORE_TYPES_UINT128

#include <string> // description

#include "core/types/basic.h"

namespace en
{

class uint128
{
    public:
    uint64 qword[2];

    inline uint128() { qword[0] = 0; qword[1] = 0; }
    uint128(const void* src);
    inline uint128(uint64 hi, uint64 lo) { qword[0] = lo; qword[1] = hi; }

    // copy assignment
    inline void operator=(const uint8& b)  { qword[1] = 0; qword[0] = b; };
    inline void operator=(const uint16& b) { qword[1] = 0; qword[0] = b; };
    inline void operator=(const uint32& b) { qword[1] = 0; qword[0] = b; };
    inline void operator=(const uint64& b) { qword[1] = 0; qword[0] = b; };
    uint128& operator=(const uint128& b);

    // comparison
    inline bool operator< (const uint128& rhs) const { return qword[1] == rhs.qword[1] ? qword[0] < rhs.qword[0] : qword[1] < rhs.qword[1]; }
    inline bool operator> (const uint128& rhs) const { return rhs < *this; }
    inline bool operator<=(const uint128& rhs) const { return !(*this > rhs); }
    inline bool operator>=(const uint128& rhs) const { return !(*this < rhs); }
    inline bool operator==(const uint128& rhs) const { return (qword[1] == rhs.qword[1]) && (qword[0] == rhs.qword[0]); }
    inline bool operator!=(const uint128& rhs) const { return !(*this == rhs); }

    // TODO: Rest of operators
    //void    operator-= (const uint128 b);          
    //void    operator+= (const uint128 b);  
    //void    operator/= (const uint128 b);
    //void    operator*= (const uint128 b);
    //bool    operator!= (const uint128 b) const;
    //uint128 operator-  () const;

    // Initializes uint128 from its text description in format:
    // 0xXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
    // Both lowercase and uppercase letters are allowed in any combination.
    // Returns false if provided string is incorrectly formatted.
    bool init(std::string& description);

    std::string description(void);
};
   
static_assert(sizeof(uint128) == 16, "en::uint128 size mismatch!");

} // en

#endif
