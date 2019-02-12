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
#include "core/memory/alignment.h"
//#include "core/threading/atomics.h"  // TODO: Replace with std::atomic operations
#include "core/utilities/NonCopyable.h"
#include "parallel/sharedAtomic.h"

namespace en
{
// Could use std::mutex, but it's cache line protection needs to be guaranteed
class cachealign Mutex : private SharedAtomic, NonCopyable
{
    public:
    Mutex() : SharedAtomic() {};
   ~Mutex() { unlock(); }; // It shouldn't matter at this point. It may keep application alive even if it reads memory after release.
      
    forceinline void lock(void)     { while(!tryLock()); }
    forceinline bool tryLock(void)  { uint32 expected = 0U; return std::atomic_compare_exchange_strong_explicit(&value, &expected, 1U, std::memory_order_seq_cst, std::memory_order_relaxed); } //  AtomicSwap((volatile uint32*)&value, (uint32)1) == 0;
    forceinline bool isLocked(void) { return std::atomic_load_explicit(&value, std::memory_order_relaxed) > 0U; }  // value > 0;
    forceinline void unlock(void)   { std::atomic_store_explicit(&value, 0U, std::memory_order_release); } // AtomicSwap((volatile uint32*)&value, (uint32)0);
};

static_assert(sizeof(Mutex) == cacheline, "en::Mutex size mismatch!");
}
#endif
