/*

 Ngine v5.0
 
 Module      : Threading primitive.
 Visibility  : Full version only.
 Requirements: none
 Description : Supports programmer with platform independent
               implementation of mutex threading primitive.

*/

#ifndef ENG_THREADING_MUTEX
#define ENG_THREADING_MUTEX

#include "core/defines.h"
#include "core/types.h"
#include "core/utilities/alignment.h"
#include "core/threading/atomics.h"

namespace en
{
   // In the past it was aligned to cacheline size (64bytes) making it extremly
   // expensive in terms of memory usage. It was supposed to prevent false 
   // sharing case, where two mutexes modified by two threads share cache line
   // but that should be handled by CPU cache synchronization in fact as atomic
   // operations are used on lock.
   class Mutex
      {
      public:
      volatile uint32 lockValue;
      
      public:
      Mutex();
     ~Mutex();
      
      forceinline void lock(void);
      forceinline bool tryLock(void);
      forceinline bool isLocked(void);
      forceinline void unlock(void);
      };
   
#include "threading/mutex.inl"
}

#endif
