/*

 Ngine v5.0
 
 Module      : Atomic operations.
 Visibility  : Any version.
 Requirements: none
 Description : Implements multithreading atomic
               operations like CAS, INC, DEC etc.

*/

#ifndef ENG_THREADING_ATOMICS
#define ENG_THREADING_ATOMICS

#include "core/defines.h"
#include "core/types.h"

// Increases pointed value in Thread-Safe atomic way.
forceinline uint32 AtomicIncrease(volatile uint32* value);
forceinline sint32 AtomicIncrease(volatile sint32* value);

// Descreases pointed value in Thread-Safe atomic way.
forceinline uint32 AtomicDecrease(volatile uint32* value);
forceinline sint32 AtomicDecrease(volatile sint32* value);

// Swaps value of pointed memory location with new one in Thread-Safe atomic way.
forceinline uint32 AtomicSwap(volatile uint32* dst, uint32 value);

// Compares pointed destination with reference value, if they are identical, 
// destination is overwritten by source value.
forceinline bool CompareAndSwap(volatile uint32* dst, uint32 newValue, uint32 expectedValue);
forceinline bool CompareAndSwap(volatile uint64* dst, uint64 newValue, uint64 expectedValue);

forceinline void ReadMemoryBarrier(void);
forceinline void WriteMemoryBarrier(void);
forceinline void FullMemoryBarrier(void);

#include "core/threading/atomics.inl"

#endif