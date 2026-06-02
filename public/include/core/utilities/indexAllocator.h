/*

 Ngine v5.0

 Module      : Index Allocator.
 Requirements: none
 Description : Allocates indexes from specified range [0..capacity].
               Its optimized for allocations from big range which are
               expected not to deplete it. If range is depleted then
               slower code path tries to reuse previously freed indexes.
               Thus it is expected that allocations are common and free
               calls are rare.

*/

#ifndef ENG_CORE_UTILITIES_INDEX_ALLOCATOR
#define ENG_CORE_UTILITIES_INDEX_ALLOCATOR

#include <atomic>
#include <vector>
#include <mutex>

#include "core/types.h"

namespace en
{

class IndexAllocator
{
    uint32 capacity; 

    std::atomic<uint32> next{0};

    std::vector<uint32> recycled;
    std::mutex lockAllocator;

    bool tryAllocateFresh(uint32& index);
    bool tryAllocateRecycled(uint32& index);

public:

    IndexAllocator(const uint32 capacity);

    bool allocate(uint32& index);
    void free(const uint32 index);
};

} // en

#endif
