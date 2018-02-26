/*

 Ngine v5.0
 
 Module      : Memory management.
 Requirements: none
 Description : Set of functions and containers allowing custom
               memory alocation, alocation to cache line aligment
               and memory pools management.

*/

#ifndef ENG_CORE_UTILITIES_MEMORY
#define ENG_CORE_UTILITIES_MEMORY

#include "core/defines.h"
#include "core/types.h"
#include "core/utilities/alignment.h"

#include "assert.h"

#ifdef EN_PLATFORM_WINDOWS
#include <malloc.h>

// Only really needs WinBase.h for Virtual Memory
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#elif defined(EN_PLATFORM_BLACKBERRY)
#include <errno.h>
#include <stdlib.h>
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
   // TODO: Implement Virtual Memory growing!
   static_assert(0, "Virtual Memory allocation not implemented on this platform!");
#endif

   return temp;
   }

   bool grow(void* address, const uint64 currentSize, const uint64 newSize)
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
   // TODO: Implement Virtual Memory growing!
   static_assert(0, "Virtual Memory allocation not implemented on this platform!");
#endif

   return true;
   }

   void deallocate(void* address)
   {
#ifdef EN_PLATFORM_WINDOWS
   VirtualFree(address, 0, MEM_RELEASE);
#else
   // TODO: Implement Virtual Memory growing!
   static_assert(0, "Virtual Memory allocation not implemented on this platform!");
#endif
   }


   // DEPRECATED:


   template <typename T>
   T* allocate(uint32 count = 1)
   {
   T* temp = nullptr;
#ifdef EN_PLATFORM_WINDOWS
   temp = static_cast<T*>(_aligned_malloc(count * sizeof(T), cacheline));
#else
   sint32 ret = posix_memalign((void **)(&temp), cacheline, count * sizeof(T)); 
   if (ret == ENOMEM)
      return nullptr;
   if (ret == EINVAL)
      return nullptr;
#endif

   return temp;
   }

   template <typename T>
   T* allocate(const uint32 alignment, const uint32 count)
   {
   T* temp;
#ifdef EN_PLATFORM_WINDOWS
   temp = static_cast<T*>(_aligned_malloc(count * sizeof(T), alignment));
#else
   sint32 ret = posix_memalign((void **)(&temp), alignment, count * sizeof(T)); 
   if (ret == ENOMEM)
      return nullptr;
   if (ret == EINVAL)
      return nullptr;
#endif
   return temp;
   }

   // Tries to grow alocation without copying backing memory. If returned pointer
   // to new location in adress space is not null, memory pointer is invalid.
   // Otherwise memory pointer is still valid, and memory contents didn't changed.
   // Alignment needs to be the same, as for originally allocated memory block.
   template <typename T>
   T* reallocate(T* memory, const uint32 alignment, const uint32 oldCount, const uint32 newCount)
   {
   T* temp;
#ifdef EN_PLATFORM_WINDOWS
   temp = static_cast<T*>(_aligned_realloc(memory, newCount * sizeof(T), alignment));
#else
   // TODO: Use Virtual Memory pages remapping to avoid memcpy
   temp = allocate<T>(alignment, newCount);
   if (temp)
      {
      memcpy(temp, memory, oldCount * sizeof(T));
      deallocate<T>(memory);
      }
#endif
   return temp;
   }

   template <typename T>
   void deallocate(T* ptr)
   {
#ifdef EN_PLATFORM_WINDOWS
   _aligned_free(ptr);
#else
   free(ptr);
#endif
   }
}

#endif
