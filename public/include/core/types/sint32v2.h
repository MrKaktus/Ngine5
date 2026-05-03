/*

 Ngine v5.0
 
 Module      : Types.
 Requirements: none
 Description : 2 component sint32 vector.

*/

#ifndef ENG_CORE_TYPES_SINT32V2
#define ENG_CORE_TYPES_SINT32V2

#include "core/types/basic.h"

namespace en
{

class sint32v2
{
public:
    sint32 x;
    sint32 y;

    sint32v2(void);
    sint32v2(const sint32 x, const sint32 y);

    bool operator== (const sint32v2 b) const;
};

static_assert(sizeof(sint32v2) == 8, "en::sint32v2 size mismatch!");

} // en

#endif
