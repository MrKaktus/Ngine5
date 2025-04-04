/*

 Ngine v5.0
 
 Module      : Types.
 Requirements: none
 Description : 2 component uint16 vector.

*/

#ifndef ENG_CORE_TYPES_UINT16V2
#define ENG_CORE_TYPES_UINT16V2

#include "core/types/basic.h"

namespace en
{

class uint16v2
{
    public:
    union { uint16 x; uint16 width;  uint16 base;  uint16 first; };
    union { uint16 y; uint16 height; uint16 count; };
   
    uint16v2(void);
    uint16v2(const uint16 x, const uint16 y);

    bool operator== (const uint16v2 b) const;
};

static_assert(sizeof(uint16v2) == 4, "en::uint16v2 size mismatch!");

} // en

#endif
