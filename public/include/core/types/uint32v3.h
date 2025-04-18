/*

 Ngine v5.0
 
 Module      : Types.
 Requirements: none
 Description : 3 component uint32 vector.

*/

#ifndef ENG_CORE_TYPES_UINT32V3
#define ENG_CORE_TYPES_UINT32V3

#include "core/types/basic.h"

namespace en
{

class uint32v3
{
    public:
    union { uint32 x; uint32 width;  };
    union { uint32 y; uint32 height; };
    union { uint32 z; uint32 depth;  };
   
    uint32v3(void);
    uint32v3(const uint32 x, const uint32 y, const uint32 z);
};

static_assert(sizeof(uint32v3) == 12, "en::uint32v3 size mismatch!");

} // en

#endif
