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
// ( deprecated, use static_assert instead )
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
   static_assert(false, "Unknown target platform!");
#endif

// Determine target platform version
#if defined(EN_PLATFORM_OSX)
   #if __MAC_OS_X_VERSION_MAX_ALLOWED >= __MAC_10_12
      #define EN_PLATFORM_OSX_MINIMUM_10_12
   #endif
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
   // Objective-C object allocator and reference count initialization
   #define allocateObjectiveC(x)    \
   [[[x alloc] init] autorelease]

#if __has_feature(objc_arc)
   // When ARC is enabled, app shouldn't need to do anything
   // (resources should be released when going out of scope)
   #define deallocateObjectiveC(x)
#else
   // Auto-release pool to ensure that Metal ARC will flush garbage collector
   #define deallocateObjectiveC(x)  \
   @autoreleasepool                 \
   {                                \
      [x release];                  \
      x = nullptr;                  \
   }
#endif


#endif

#endif
