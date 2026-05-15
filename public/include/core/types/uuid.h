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

    // Initializes UUID from its text description in format:
    // XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX
    // Both lowercase and uppercase letters are allowed in any combination.
    // Returns false if provided string is incorrectly formatted.
    bool init(std::string& description);

    std::string description(void);
};

static_assert(sizeof(UUID) == 16, "en::UUID size mismatch!");

} // en

// Declaring and defining hash operator for UUID so that it can be used with std::unordered_map
namespace std
{
    template<>
    struct hash<en::UUID>
    {
        size_t operator()(const en::UUID& id) const noexcept
        {
            return static_cast<size_t>(id.qword[0]);
        }
    };
}

#endif