/*

 Ngine v5.0
 
 Module      : Memory management.
 Requirements: none
 Description : Set of functions and containers allowing custom
               memory alocation, alocation to cache line aligment
               and momory pools management.

*/

#ifndef ENG_CORE_UTILITIES_MEMORY
#define ENG_CORE_UTILITIES_MEMORY

#include "core/defines.h"
#include "core/types.h"
#include "core/utilities/alignment.h"

#include "assert.h"

#ifdef EN_PLATFORM_WINDOWS
#include <malloc.h>
#elif defined(EN_PLATFORM_BLACKBERRY)
#include <errno.h>
#include <stdlib.h>
#else
#include <errno.h>
#include <stdlib.h>
#endif

namespace en
{
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
   T* allocate(uint32 alignment, uint32 count)
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

   template <typename T>
   T* reallocate(T* memory, uint32 alignment, uint32 count)
   {
   T* temp;
#ifdef EN_PLATFORM_WINDOWS
   temp = static_cast<T*>(_aligned_realloc(memory, count * sizeof(T), alignment));
#else
   // THERE IS NO MEM ALIGNED REALLOC ON UNIX SYSTEMS !
   assert(0);
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
