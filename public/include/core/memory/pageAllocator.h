/*

 Ngine v5.0
 
 Module      : Page Allocator.
 Requirements: none
 Description : Allocates memory and reserver adress space range for future 
               growth of allocation on page granularity.

*/

#ifndef ENG_CORE_MEMORY_PAGE_ALLOCATOR
#define ENG_CORE_MEMORY_PAGE_ALLOCATOR

#include "core/defines.h"
#include "core/types.h"

namespace en
{

// Allocated memory is always aligned to 4KB
void* virtualAllocate(const uint64 size, const uint64 maximumSize);

// Increases allocation size without copying data, or changing location
bool  virtualReallocate(void* address, const uint64 currentSize, const uint64 newSize);

void  virtualDeallocate(void* address, const uint64 maximumSize);

} // en
#endif
