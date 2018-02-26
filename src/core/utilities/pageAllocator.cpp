/*

 Ngine v5.0
 
 Module      : Page Allocator.
 Requirements: none
 Description : Allocates memory and reserver adress space range for future 
               growth of allocation on page granularity.

*/

#include "assert.h"

#include "core/utilities/pageAllocator.h"

#ifdef EN_PLATFORM_WINDOWS
// Only really needs WinBase.h for Virtual Memory
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <errno.h>
#include <stdlib.h>
#endif

namespace en
{
   void* allocate(const uint64 size, const uint64 maximumSize)
   {
   // Size and maximum size needs to be explicitly multiple of 4KB
   assert( size % 4096 == 0 );
   assert( maximumSize % 4096 == 0 );

   void* temp = nullptr;
#ifdef EN_PLATFORM_WINDOWS
   // First reserve max size, to which given allocation can grow
   temp = VirtualAlloc(nullptr,      // Reserve anywhere in adress space
                       maximumSize,  // Max size to which this allocation can grow
                       MEM_RESERVE,  // Reserve adress space without allocating memory
                       0);           // No page properties during reserve

   // Once adress space for max possible size is reserved, allocate initial size
   if (temp)
      {
      if (!VirtualAlloc(temp,            // Allocate pages at start of reserved space
                        size,            // Initial size to allocate
                        MEM_COMMIT,      // Pre-allocate memory (will truly allocate and clear on first access)
                        PAGE_READWRITE)) // No page properties during reserve
         {
         // Couldn't alllocate physical pages, reverting reservation and faulting
         VirtualFree(temp, 0, MEM_RELEASE);
         temp = nullptr;
         }
      }
#else
   // TODO: Implement Virtual Memory allocation!
   static_assert(0, "Virtual Memory allocation not implemented on this platform!");
#endif

   return temp;
   }

   bool reallocate(void* address, const uint64 currentSize, const uint64 newSize)
   {
   // Current and new size needs to be explicitly multiple of 4KB
   assert( currentSize % 4096 == 0 );
   assert( newSize % 4096 == 0 );

   void*  subAddress = static_cast<void*>(static_cast<uint8*>(address) + currentSize);
   uint64 growSize   = newSize - currentSize;
#ifdef EN_PLATFORM_WINDOWS
   if (!VirtualAlloc(subAddress,      // Allocate pages at end of already allocated section
                     growSize,        // Size to grow allocation by
                     MEM_COMMIT,      // Pre-allocate memory (will truly allocate and clear on first access)
                     PAGE_READWRITE)) // No page properties during reserve
      return false;
#else
   // TODO: Implement Virtual Memory allocation!
   static_assert(0, "Virtual Memory allocation not implemented on this platform!");
#endif

   return true;
   }

   void deallocate(void* address)
   {
#ifdef EN_PLATFORM_WINDOWS
   VirtualFree(address, 0, MEM_RELEASE);
#else
   // TODO: Implement Virtual Memory allocation!
   static_assert(0, "Virtual Memory allocation not implemented on this platform!");
#endif
   }
}