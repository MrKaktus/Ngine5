/*
 
 Ngine v5.0
 
 Module      : Data aligment defines.
 Requirements: none
 Description : Ensures data between markers will be accordingly aligned in memory.
 
 */

#ifndef ENG_CORE_MEMORY_ALIGNMENT
#define ENG_CORE_MEMORY_ALIGNMENT

#include "core/defines.h"

// Cache data alignment
#define cacheline      64
#if defined(EN_COMPILER_VISUAL_STUDIO) || defined(EN_COMPILER_INTEL)
#define cachealign  __declspec(align(cacheline))
#else
#define cachealign  __attribute__ ((aligned(cacheline)))
#endif

// Specific data alignment
#if defined(EN_COMPILER_VISUAL_STUDIO) || defined(EN_COMPILER_INTEL)
#define alignTo(value) __pragma( pack(push, value) )
#define alignToDefault __pragma(pack())
#elif defined(EN_COMPILER_CLANG) || defined(EN_COMPILER_GCC) || defined(EN_COMPILER_QCC)
#define alignTo(value) _Pragma("pack(push, value)")
#define alignToDefault _Pragma("pack(pop)")
#else
// Concatenates preprocessor tokens A and B before stage of macro-expanding.
#define concatenate_before(a,b) a ## b
// Concatenates preprocessor tokens A and B after stage of macro-expanding.
#define concatenate(a,b) concatenate_before(a,b)
// Turn A into a string literal before stage of macro-expanding.
#define tostring_before(a) #a
// Turn A into a string literal after stage of macro-expanding.
#define tostring(a) tostring_before(a)

#define packPart1 pack(push
#define packPart2 )
#define alignTo(value) _Pragma(tostring(concatenate(packPart1,concatenate(value,packPart2))))
#undef packPart1
#undef packPart2

#define alignToDefault _Pragma("pack()")
#endif

#endif
