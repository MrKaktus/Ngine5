/*

 Ngine v5.0
 
 Module      : Macros and defines for compilation.
 Requirements: none
 Description : Holds declarations of macros and definitions
               that allows optimal compilation of code
               without knowledge about software and
               hardware target platform.

*/

#ifndef ENG_CORE_DEFINES
#define ENG_CORE_DEFINES

// Compile-Time Assertion
#define CompileTimeAssert(predicate, file) _impl_CASSERT_LINE(predicate,__LINE__,file)
#define _impl_PASTE(a,b) a##b
#define _impl_CASSERT_LINE(predicate, line, file) \
    typedef char _impl_PASTE(assertion_failed_##file##_,line)[2*!!(predicate)-1];

// Determine target platform
#if defined(ANDROID)
   #define EN_PLATFORM_ANDROID
#elif defined(__QNX__)
   #define EN_PLATFORM_BLACKBERRY
#elif defined(TARGET_OS_IPHONE) || defined(TARGET_IPHONE_SIMULATOR)
   #define EN_PLATFORM_IOS
#elif defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__) || defined(TARGET_OS_MAC)
   #define EN_PLATFORM_OSX
#elif defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
   #define EN_PLATFORM_WINDOWS
#else
   CompileTimeAssert(false, unknown_platform)
#endif

// Define platform architecture
#if defined(_WIN64)
   #define EN_ARCHITECTURE_X64
#else
   #define EN_ARCHITECTURE_X86
#endif

// Determine used compilator
#if defined(__APPLE__)
   #define EN_COMPILER_CLANG
#elif defined(__GNUC__)
   #define EN_COMPILER_GCC
#elif defined(__INTEL_COMPILER)
   #define EN_COMPILER_INTEL
#elif defined(__QNX__)
   #define EN_COMPILER_QCC
#elif defined(_MSC_VER)
   #define EN_COMPILER_VISUAL_STUDIO
#endif

// Determine target renderer
#if defined(EN_PLATFORM_OSX) || defined(EN_PLATFORM_WINDOWS)
   #define EN_OPENGL_DESKTOP
   #define EN_DISCRETE_GPU
#elif defined(EN_PLATFORM_ANDROID) || defined(EN_PLATFORM_BLACKBERRY) || defined(EN_PLATFORM_IOS)
   #define EN_OPENGL_MOBILE
   #define EN_MOBILE_GPU
#endif

// Determine engine debugging
#if defined(_DEBUG) || defined(DEBUG)
   #define EN_DEBUG
#endif

// Forcing inlining
#if defined(EN_COMPILER_VISUAL_STUDIO)
   #define forceinline __forceinline
#else
   #define forceinline inline
#endif

// Cache data alignment
#define cacheline      64
#if defined(EN_COMPILER_VISUAL_STUDIO) || defined(EN_COMPILER_INTEL)
   #define cachealign  __declspec(align(cacheline))
#else
   #define cachealign
#endif

// Specific data alignment
#if defined(EN_COMPILER_VISUAL_STUDIO) || defined(EN_COMPILER_INTEL)
   #define aligned(value) __pragma( pack(push, value) )
   #define aligndefault __pragma(pack())
#elif defined(EN_COMPILER_CLANG) || defined(EN_COMPILER_GCC) || defined(EN_COMPILER_QCC)
   #define aligned(value) _Pragma("pack(push, value)")
   #define aligndefault _Pragma("pack()")
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
   #define aligned(value) _Pragma(tostring(concatenate(packPart1,concatenate(value,packPart2))))  
   #undef packPart1
   #undef packPart2

   #define aligndefault _Pragma("pack()")
#endif

// TODO: This should be moved to "platform.h" or similar init header
// System specyfic includes
#if defined(EN_PLATFORM_WINDOWS)
   #include <windows.h>
#endif

#if defined(EN_PLATFORM_BLACKBERRY)
   #ifndef nullptr
      #define nullptr 0
   #endif
#endif

#endif