/*

 Ngine v5.0
 
 Module      : Threading primitive.
 Visibility  : Full version only.
 Requirements: none
 Description : Supports programmer with platform independent
               implementation of mutex threading primitive.

*/

#ifndef ENG_THREADING_SEMAPHORE
#define ENG_THREADING_SEMAPHORE

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
   class Semaphore
      {
      private:
      volatile uint32 value;
      
      public:
      forceinline Semaphore(const uint32 max) : 
         value(max) 
      {
      };

      forceinline wait(void) 
      {
if value > 0 then
 value--;
else
 ”block calling task”;


// Compares pointed destination with reference value, 
// if they are identical, destination is overwritten 
// by source value.
forceinline bool CompareAndSwap(&value, value--, uint32 cmp);
forceinline bool CompareAndSwap(volatile uint64* dst, uint64 src, uint64 cmp);



      };
      
      forceinline void signal(void)
      {
      };

      };
   
#include "threading/mutex.inl"
}

#endif