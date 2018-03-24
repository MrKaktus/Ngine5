/*

 Ngine v5.0
 
 Module      : Page Allocator.
 Requirements: none
 Description : Allocates memory and reserver adress space range for future 
               growth of allocation on page granularity.

*/

#ifndef ENG_CORE_UTILITIES_PAGE_ALLOCATOR
#define ENG_CORE_UTILITIES_PAGE_ALLOCATOR

#include "core/defines.h"
#include "core/types.h"

namespace en
{
   // Allocated memory is always aligned to 4KB
   void* allocate(const uint64 size, const uint64 maximumSize);
 
   // Increases allocation size without copying data, or changing location
   bool reallocate(void* address, const uint64 currentSize, const uint64 newSize);

   void deallocate(void* address, const uint64 maximumSize);
}
#endif
