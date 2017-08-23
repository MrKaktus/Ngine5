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
// https://chromium.googlesource.com/android_tools/+/master-backup/ndk/docs/ANDROID-ATOMICS.html
#include <sys/atomics.h>
#endif
#ifdef EN_PLATFORM_BLACKBERRY
#include <atomic.h>
#endif
#ifdef EN_PLATFORM_OSX
#pragma push_macro("aligned")
#undef aligned
#include <libkern/OSAtomic.h>
#pragma pop_macro("aligned")
#endif
#ifdef EN_PLATFORM_WINDOWS
// Need to compile with Windows 10 SDK to prevent:
// c:\program files (x86)\windows kits\10\include\10.0.10240.0\um\ole2.h(39): fatal error C1083: Cannot open include file: 'coml2api.h': No such file or directory
// See more at:
// https://naughter.wordpress.com/2015/05/24/changes-in-the-windows-10-sdk-compared-to-windows-8-1-part-two/
//
#define WIN32_LEAN_AND_MEAN
#define NOGDI                  // <- TODO: To prevent overloading of SetLayout. This header ends in App space! This includes should be private !!
#include <windows.h>
#include <intrin.h>
#endif

// Full memory barrier is required if threads of the same application are
// distributed between several physical cores in symmetric-multiprocessing system
// (which is what we want and why Inc/Dec/Cmp functions should have it).

// As long as underlying CPU architecture support "two's complement" arithmetic:
// https://en.wikipedia.org/wiki/Two%27s_complement
// there is no difference between addition and subtraction of signed and unsigned
// integer types. x86 supports this by default, but it looks like on ARM you need
// to explicitly specify if data is signed or unsigned to use proper instruction.

// Available BlackBerry NDK bitwise atomic operations can be found here:
// https://developer.blackberry.com/native/reference/core/com.qnx.doc.neutrino.lib_ref/topic/a/atomic_set_value.html

// Generates full memory barrier. Returns new value.
forceinline uint32 AtomicIncrease(volatile uint32* value)
{
#ifdef EN_PLATFORM_ANDROID
// As it returns old value, we need to locally increment it again.
return __sync_fetch_and_add(value, 1u) + 1u;
// No memory barrier variant:
// return __atomic_inc((volatile int*)value);
#endif

#ifdef EN_PLATFORM_BLACKBERRY
// Memory barrier according to:
// http://www.qnx.com/developers/docs/6.3.0SP3/neutrino/lib_ref/a/atomic_add.html
atomic_add((volatile unsigned *)value, 1);
return *value;
#endif

#if defined(EN_PLATFORM_OSX_MINIMUM_10_12)
// As it returns old value, we need to locally increment it again.
return static_cast<uint32>(std::atomic_fetch_add((volatile std::atomic<uint32>*)value, 1u) + 1u);
#elif defined(EN_PLATFORM_OSX)
// Cast from Unsigned to Signed is safe as Mac's uses x86 CPU's
return static_cast<uint32>(OSAtomicIncrement32Barrier((volatile int*)value));
#endif

#ifdef EN_PLATFORM_WINDOWS
// Cast from Unsigned to Signed is safe as Windows runs on x86 CPU's
return static_cast<uint32>(InterlockedIncrement((volatile long*)value));
#endif

assert(0);
return 0;
}

// Generates full memory barrier. Returns new value.
forceinline sint32 AtomicIncrease(volatile sint32* value)
{
#ifdef EN_PLATFORM_ANDROID
return __sync_fetch_and_add(value, 1) + 1;
#endif

#ifdef EN_PLATFORM_BLACKBERRY
// TODO: There is no signed version in QNX.
//       Is BlackBerry using two's complement arithmetic?
#endif

#if defined(EN_PLATFORM_OSX_MINIMUM_10_12)
// As it returns old value, we need to locally increment it again.
return std::atomic_fetch_add((volatile std::atomic<sint32>*)value, 1) + 1;
#elif defined(EN_PLATFORM_OSX)
return OSAtomicIncrement32Barrier((volatile int32_t*)value);
#endif

#ifdef EN_PLATFORM_WINDOWS
return InterlockedIncrement((volatile long*)value);
#endif

assert(0);
return 0;
}

// Generates full memory barrier. Returns new (decremented) value.
forceinline uint32 AtomicDecrease(volatile uint32* value)
{
#ifdef EN_PLATFORM_ANDROID
// As it returns old value, we need to locally decrement it again.
return __sync_fetch_and_sub(value, 1u) - 1u;
// No memory barrier variant:
//return __atomic_dec((volatile int*)value);
#endif

#ifdef EN_PLATFORM_BLACKBERRY
// Memory barrier according to:
// http://www.qnx.com/developers/docs/6.3.2/neutrino/lib_ref/a/atomic_sub.html
atomic_sub((volatile unsigned *)value, 1);
return *value;
#endif

#if defined(EN_PLATFORM_OSX_MINIMUM_10_12)
// As it returns old value, we need to locally decrement it again.
return static_cast<uint32>(std::atomic_fetch_sub((volatile std::atomic<uint32>*)value, 1u) - 1u);
#elif defined(EN_PLATFORM_OSX)
// Cast from Unsigned to Signed is safe as Mac's uses x86 CPU's
return static_cast<uint32>(OSAtomicDecrement32Barrier((volatile int32_t*)value));
#endif

#ifdef EN_PLATFORM_WINDOWS
// Cast from Unsigned to Signed is safe as Windows runs on x86 CPU's
return static_cast<uint32>(InterlockedDecrement((volatile long*)value));
#endif

assert(0);
return 0;
}

// Generates full memory barrier. Returns new (decremented) value.
forceinline sint32 AtomicDecrease(volatile sint32* value)
{
#ifdef EN_PLATFORM_ANDROID
return __sync_fetch_and_sub(value, 1) - 1;
#endif

#ifdef EN_PLATFORM_BLACKBERRY
// TODO: There is no signed version in QNX.
//       Is BlackBerry using two's complement arithmetic?
#endif

#if defined(EN_PLATFORM_OSX_MINIMUM_10_12)
// As it returns old value, we need to locally decrement it again.
return std::atomic_fetch_sub((volatile std::atomic<sint32>*)value, 1) - 1;
#elif defined(EN_PLATFORM_OSX)
return OSAtomicDecrement32Barrier((volatile int32_t*)value);
#endif

#ifdef EN_PLATFORM_WINDOWS
return InterlockedDecrement((volatile long*)value);
#endif

assert(0);
return 0;
}

// Swaps value of pointed memory location with new one.
// Generates full memory barrier. Returns old value.
forceinline uint32 AtomicSwap(volatile uint32* dst, uint32 value)
{
#ifdef EN_PLATFORM_ANDROID
// Software implementation
uint32 oldValue;
do
{
oldValue = *dst;
}
while(__sync_val_compare_and_swap(dst, oldValue, value) != oldValue);
return oldValue;
// No memory barrier variant:
// return __atomic_swap(value, (volatile int*)dst);
#endif

#ifdef EN_PLATFORM_BLACKBERRY
// It is not supported on BlackBerry.
#endif


#if defined(EN_PLATFORM_OSX_MINIMUM_10_12)
// Software implementation
uint32 oldValue;
do
{
oldValue = *dst;
}
while(!std::atomic_compare_exchange_strong((volatile std::atomic<uint32>*)dst, &oldValue, value));
return oldValue;
#elif defined(EN_PLATFORM_OSX)
// Software implementation
uint32 oldValue;
do
{
oldValue = *dst;
}
while(!OSAtomicCompareAndSwap32Barrier(oldValue, value, (volatile int32_t*)dst));
return oldValue;
#endif

#ifdef EN_PLATFORM_WINDOWS
return InterlockedExchange(dst, value);
#endif

assert(0);
return 0;
}

// Swaps value of pointed memory location with new one.
// Generates full memory barrier. Returns old value.
forceinline uint64 AtomicSwap(volatile uint64* dst, uint64 value)
{
#ifdef EN_PLATFORM_ANDROID
// It is not supported on Android.
#endif

#ifdef EN_PLATFORM_BLACKBERRY
// It is not supported on BlackBerry.
#endif

#if defined(EN_PLATFORM_OSX_MINIMUM_10_12)
// Software implementation
uint64 oldValue;
do
{
oldValue = *dst;
}
while(!std::atomic_compare_exchange_strong((volatile std::atomic<uint64>*)dst, &oldValue, value));
return oldValue;
#elif defined(EN_PLATFORM_OSX)
// Software implementation
uint64 oldValue;
do
{
oldValue = *dst;
}
while(!OSAtomicCompareAndSwap64Barrier(oldValue, value, (volatile int64_t*)dst));
return oldValue;
#endif

#ifdef EN_PLATFORM_WINDOWS
return InterlockedExchange64((volatile LONGLONG*)dst, value);
#endif

assert(0);
return 0;
}

// Generates full memory barrier. Returns true if succesfull.
forceinline bool CompareAndSwap(volatile uint32* dst, uint32 src, uint32 cmp)
{
#ifdef EN_PLATFORM_ANDROID
return __sync_val_compare_and_swap(dst, cmp, src) == cmp;
// No memory barrier
//return __atomic_cmpxchg(cmp, src, (volatile int*)dst) == cmp;
#endif

#ifdef EN_PLATFORM_BLACKBERRY
// QNX implementation:
//
// /usr/include/<platform>/cmpxchg.h
// int _smp_cmpxchg(volatile unsigned *__dst, unsigned __cmp, unsigned __new);
//
// Unfortunately it's not supported by BlackBerry NDK.
#endif

#if defined(EN_PLATFORM_OSX_MINIMUM_10_12)
return std::atomic_compare_exchange_strong((volatile std::atomic<uint32>*)dst, &cmp, src);
#elif defined(EN_PLATFORM_OSX)
return OSAtomicCompareAndSwap32Barrier(cmp, src, (volatile int32_t*)dst);
#endif

#ifdef EN_PLATFORM_WINDOWS
// Functions returns initial value from pointed memory,
// if it is equal to cmp value then exchange was successful.
return InterlockedCompareExchange((volatile unsigned long*)dst, src, cmp) == cmp;
#endif

assert( 0 );
return false;
}

// Generates full memory barrier. Returns true if succesfull.
forceinline bool CompareAndSwap(volatile uint64* dst, uint64 src, uint64 cmp)
{
#ifdef EN_PLATFORM_ANDROID
// It is not supported on Android.
#endif

#if defined(EN_PLATFORM_OSX_MINIMUM_10_12)
return std::atomic_compare_exchange_strong((volatile std::atomic<uint64>*)dst, &cmp, src);
#elif defined(EN_PLATFORM_OSX)
return OSAtomicCompareAndSwap64Barrier(cmp, src, (volatile int64_t*)dst);
#endif

#ifdef EN_PLATFORM_BLACKBERRY
// It is not supported on BlackBerry.
#endif

#ifdef EN_PLATFORM_WINDOWS
// Functions returns initial value from pointed memory,
// if it is equal to cmp value then exchange was successful.
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
   
