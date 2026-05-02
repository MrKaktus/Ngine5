/*

 Ngine v5.0

 Module      : Types.
 Requirements: none
 Description : Universally unique identifier

*/

#ifndef ENG_CORE_TYPES_UUID
#define ENG_CORE_TYPES_UUID

#include <string>

#include "core/types/uint128.h"

// Replace any pre-existing UUID definition (for e.g. from Windows SDK)
#ifdef UUID
#undef UUID
#endif

namespace en
{

class UUID : public uint128
{
public:

    // All zeros by default
    UUID();

    // Auto-generates new UUID
    void init(void);

    std::string description(void);
};

static_assert(sizeof(UUID) == 16, "en::UUID size mismatch!");

} // en

#endif