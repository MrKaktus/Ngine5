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
#define CompileTimeAssert(predicate, message) _impl_CASSERT_LINE(predicate,__LINE__,message)
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

// Input modules

#if defined(EN_PLATFORM_WINDOWS)
#define EN_MODULE_KINECT
#endif

// Disable until Oculus is piped through using new Rendering frontend.

//#if defined(EN_PLATFORM_WINDOWS)
//#define EN_MODULE_OCULUS
//#endif


// Disable until Rendering abstraction is finished!
/*
#if defined(EN_PLATFORM_OSX) || defined(EN_PLATFORM_WINDOWS)
#define EN_MODULE_OPENVR
#endif
*/

#if defined(EN_PLATFORM_WINDOWS)
#define EN_MODULE_REALSENSE
#endif

// Rendering backend modules

/* 
// Removed opengl32.lib; from linking
#if defined(EN_PLATFORM_WINDOWS) 
#define EN_MODULE_RENDERER_OPENGL
#endif
*/

#if defined(EN_PLATFORM_ANDROID) || defined(EN_PLATFORM_BLACKBERRY)
#define EN_MODULE_RENDERER_OPENGLES
#endif

#if defined(EN_PLATFORM_ANDROID) || defined(EN_PLATFORM_LINUX) || defined(EN_PLATFORM_WINDOWS)
#define EN_MODULE_RENDERER_VULKAN
#endif

#if defined(EN_PLATFORM_WINDOWS)
#define EN_MODULE_RENDERER_DIRECT3D12
#endif

#if defined(EN_PLATFORM_IOS) || defined(EN_PLATFORM_OSX)
#define EN_MODULE_RENDERER_METAL
#endif

// Determine target renderer
// TODO: DEPRECATED, Remove
#if defined(EN_PLATFORM_OSX) || defined(EN_PLATFORM_WINDOWS)
   #define EN_OPENGL_DESKTOP
   #define EN_DISCRETE_GPU
#elif defined(EN_PLATFORM_ANDROID) || defined(EN_PLATFORM_BLACKBERRY) || defined(EN_PLATFORM_IOS)
   #define EN_OPENGL_MOBILE
   #define EN_MOBILE_GPU
#endif

#if defined(EN_PLATFORM_BLACKBERRY)
   #ifndef nullptr
      #define nullptr 0
   #endif
#endif

#if defined(EN_PLATFORM_IOS) || defined(EN_PLATFORM_OSX)
   // Enable Automatic Reference Counting, as Metal is leaking without it
   #if __has_feature(objc_arc)
      #define APPLE_ARC
   #endif
#endif

#endif
