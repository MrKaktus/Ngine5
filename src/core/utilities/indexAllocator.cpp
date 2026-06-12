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

#include "core/utilities/indexAllocator.h"

namespace en
{

IndexAllocator::IndexAllocator(const uint32 _capacity) :
    size(_capacity)
{
}

bool IndexAllocator::tryAllocateFresh(uint32& index)
{
    uint32 current = next.load(std::memory_order_relaxed);

    while (current < size)
    {
        // current gets updated automatically on failure
        if (next.compare_exchange_weak(current, current + 1, std::memory_order_relaxed))
        {
            index = current;
            return true;
        }
    }

    return false;
}

bool IndexAllocator::tryAllocateRecycled(uint32& index)
{
    std::lock_guard lock(lockAllocator);

    if (recycled.empty())
    {
        return false;
    }

    index = recycled.back();
    recycled.pop_back();
    return true;
}

bool IndexAllocator::allocate(uint32& index)
{
    if (!tryAllocateFresh(index))
    {
        return true;
    }

    return tryAllocateRecycled(index);
}

void IndexAllocator::free(const uint32 index)
{
    std::lock_guard lock(lockAllocator);
    recycled.push_back(index);
}

} // en
