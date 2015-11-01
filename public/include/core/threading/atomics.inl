/*

 Ngine v5.0
 
 Module      : Atomic operations.
 Visibility  : Any version.
 Requirements: none
 Description : Implements multithreading atomic
               operations like CAS, INC, DEC etc.

*/

#include "assert.h"

#ifdef EN_PLATFORM_ANDROID
#include <sys/atomics.h>
#endif
#ifdef EN_PLATFORM_BLACKBERRY
#include <atomic.h>
#endif
#ifdef EN_PLATFORM_WINDOWS
#include <windows.h>
#include <intrin.h>
#endif

forceinline uint32 AtomicIncrease(volatile uint32* value)
{
// Increases pointed value in Thread-Safe atomic way.
#ifdef EN_PLATFORM_ANDROID
return __atomic_inc((volatile int*)value);
#endif
#ifdef EN_PLATFORM_BLACKBERRY
atomic_add((volatile unsigned *)value, 1);
return *value;
#endif
#ifdef EN_PLATFORM_WINDOWS
return InterlockedIncrement((volatile long*)value);
#endif

assert(0);
return 0;
}

forceinline uint32 AtomicIncrease(volatile sint32* value)
{
// Increases pointed value in Thread-Safe atomic way.
#ifdef EN_PLATFORM_ANDROID
return __atomic_inc((volatile int*)value);
#endif
#ifdef EN_PLATFORM_WINDOWS
return InterlockedIncrement((volatile long*)value);
#endif

assert(0);
return 0;
}

forceinline uint32 AtomicDecrease(volatile uint32* value)
{
// Descreases pointed value in Thread-Safe atomic way.
#ifdef EN_PLATFORM_ANDROID
return __atomic_dec((volatile int*)value);
#endif
#ifdef EN_PLATFORM_BLACKBERRY
atomic_sub((volatile unsigned *)value, 1);
return *value;
#endif
#ifdef EN_PLATFORM_WINDOWS
return InterlockedDecrement((volatile long*)value);
#endif

assert(0);
return 0;
}

forceinline uint32 AtomicDecrease(volatile sint32* value)
{
// Descreases pointed value in Thread-Safe atomic way.
#ifdef EN_PLATFORM_ANDROID
return __atomic_dec((volatile int*)value);
#endif
#ifdef EN_PLATFORM_WINDOWS
return InterlockedDecrement((volatile long*)value);
#endif

assert(0);
return 0;
}

forceinline uint32 AtomicSwap(volatile uint32* dst, uint32 value)
{
// Swaps value of pointed memory location with 
// new one in Thread-Safe atomic way. As a result 
// returns its old value.
#ifdef EN_PLATFORM_ANDROID
return __atomic_swap(value, (volatile int*)dst);
#endif
#ifdef EN_PLATFORM_BLACKBERRY
return atomic_set_value((volatile unsigned *)dst, value);
#endif
#ifdef EN_PLATFORM_WINDOWS
return InterlockedExchange(dst, value);
#endif

assert(0);
return 0;
}

forceinline bool CompareAndSwap(volatile uint32* dst, uint32 src, uint32 cmp)
{
 // Functions returns initial value from 
 // pointed memory, if it is equal to cmp
 // value then exchange was successful.
#ifdef EN_PLATFORM_ANDROID
 return __atomic_cmpxchg(cmp, src, (volatile int*)dst) == cmp;
#endif
#ifdef EN_PLATFORM_WINDOWS
 return InterlockedCompareExchange((volatile unsigned long*)dst, src, cmp) == cmp;
#endif

 assert( 0 );
 return false;
}

forceinline bool CompareAndSwap(volatile uint64* dst, uint64 src, uint64 cmp)
{
 // Functions returns initial value from 
 // pointed memory, if it is equal to cmp
 // value then exchange was successful.
#ifdef EN_PLATFORM_WINDOWS
 return InterlockedCompareExchange64((volatile LONGLONG*)dst, src, cmp) == cmp;
#endif

 assert( 0 );
 return false;
}

forceinline void ReadMemoryBarrier(void)
{
#ifdef EN_PLATFORM_WINDOWS
   #ifdef EN_COMPILER_INTEL
   void_mm_lfence();
   return;
   #endif
#endif

assert( 0 );
return;
}

forceinline void WriteMemoryBarrier(void)
{
#ifdef EN_PLATFORM_WINDOWS
   #ifdef EN_COMPILER_INTEL
   void_mm_sfence();
   return;
   #endif
#endif

assert( 0 );
return;
}

forceinline void FullMemoryBarrier(void)
{
#ifdef EN_PLATFORM_WINDOWS
   #ifdef EN_COMPILER_INTEL
   void_mm_mfence();
   return;
   #endif
   #ifdef EN_COMPILER_VISUAL_STUDIO
   _ReadWriteBarrier();
   return;
   #endif
#endif

assert( 0 );
return;
}
   