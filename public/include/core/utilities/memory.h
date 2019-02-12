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
#include "core/memory/alignment.h"

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
/*
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
   T* allocate(const uint32 count, const uint32 alignment)
   {
   T* temp = nullptr;
   uint32 size = count * sizeof(T);
#ifdef EN_PLATFORM_WINDOWS
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
#ifdef EN_PLATFORM_WINDOWS
   _aligned_free(ptr);
#else
   free(ptr);
#endif
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
   temp = allocate<T>(newCount, alignment);
   if (temp)
      {
      memcpy(temp, memory, oldCount * sizeof(T));
      deallocate<T>(memory);
      }
#endif
   return temp;
   }
//*/
}

#endif
