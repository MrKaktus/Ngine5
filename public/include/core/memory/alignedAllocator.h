/*

 Ngine v5.0

 Module      : Aligned Allocator.
 Requirements: none
 Description : Set of functions and containers allowing custom
               memory alocation, alocation to cache line aligment
               and memory pools management.

*/

#ifndef ENG_CORE_MEMORY_ALIGNED_ALLOCATOR
#define ENG_CORE_MEMORY_ALIGNED_ALLOCATOR

#include "core/defines.h"
#include "core/types.h"
#include "core/memory/alignment.h"

namespace en
{

template <typename T>
T* allocate(uint32 count = 1)
{
    T* temp = nullptr;
#if defined(EN_PLATFORM_WINDOWS)
    temp = static_cast<T*>(_aligned_malloc(count * sizeof(T), cacheline));
#else
    sint32 ret = posix_memalign((void **)(&temp), cacheline, count * sizeof(T));
    if (ret == ENOMEM)
    {
        return nullptr;
    }
    if (ret == EINVAL)
    {
        return nullptr;
    }
#endif

    return temp;
}

template <typename T>
T* allocate(const uint32 count, const uint32 alignment)
{
    T* temp = nullptr;
    uint32 size = count * sizeof(T);
#if defined(EN_PLATFORM_WINDOWS)
    temp = static_cast<T*>(_aligned_malloc(size, alignment));
#else
    int ret = posix_memalign((void **)(&temp), alignment, size);
    if (ret == ENOMEM)
        return nullptr;
    if (ret == EINVAL)
        return nullptr;
#endif
    return temp;
}

template <typename T>
void deallocate(T* ptr)
{
#if defined(EN_PLATFORM_WINDOWS)
    _aligned_free(ptr);
#else
    free(ptr);
#endif
}

/// Tries to grow alocation without copying backing memory. If returned pointer
/// to new location in adress space is not null, original memory pointer is invalid.
/// Otherwise memory pointer is still valid, and memory contents didn't changed.
/// Alignment needs to be the same, as for originally allocated memory block.
/// Try to avoid this function, and use page allocator instead.
template <typename T>
T* reallocate(T* memory, const uint32 alignment, const uint32 oldCount, const uint32 newCount)
{
    T* temp = nullptr;
#if defined(EN_PLATFORM_WINDOWS)
    temp = static_cast<T*>(_aligned_realloc(memory, newCount * sizeof(T), alignment));
#else
    // TODO: Use Virtual Memory pages remapping to avoid memcpy
    temp = allocate<T>(newCount, alignment);
    if (temp)
    {
        memcpy(temp, memory, oldCount * sizeof(T));
        deallocate<T>(memory);
    }
#endif
    return temp;
}

} // en
#endif