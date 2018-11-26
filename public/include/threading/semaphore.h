/*

 Ngine v5.0
 
 Module      : Threading primitive.
 Visibility  : Full version only.
 Requirements: none
 Description : Supports programmer with platform independent
               implementation of semaphore primitive.

*/

#ifndef ENG_THREADING_SEMAPHORE
#define ENG_THREADING_SEMAPHORE

#include "core/defines.h"
#include "core/types.h"
#include "parallel/sharedAtomic.h"

#include "core/threading/atomics.h"
#include "core/utilities/NonCopyable.h"

namespace en
{
   // TODO: See: https://stackoverflow.com/questions/4792449/c0x-has-no-semaphores-how-to-synchronize-threads
   //
   class cachealign Semaphore : private SharedAtomic, NonCopyable
      {
      public:
      forceinline Semaphore(const uint32 max) : value(max) {}

/*

TODO: Implement using std::atomics

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

*/

      };
      
      forceinline void signal(void)
      {
      };

      };
   
   static_assert(sizeof(Semaphore) == 64, "en::Semaphore size mismatch!");

#include "threading/mutex.inl"
}

#endif