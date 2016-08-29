/*

 Ngine v5.0
 
 Module      : Vulkan API
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.
               This module implements OpenGL backend.

*/

#ifndef ENG_CORE_RENDERING_VULKAN
#define ENG_CORE_RENDERING_VULKAN

#include "core/defines.h"

#if defined(EN_PLATFORM_ANDROID) || defined(EN_PLATFORM_LINUX) || defined(EN_PLATFORM_WINDOWS)


// Specify which Windowing System Interface we will use
// (depending on OS for which this source code is compiled)
#if defined(EN_PLATFORM_ANDROID)
#define VK_USE_PLATFORM_ANDROID_KHR
#endif

#if defined(EN_PLATFORM_LINUX)
// TODO: Pick one on Linux (as usual it's complete mess)
// For XCB library
#define VK_USE_PLATFORM_XCB_KHR
// For XLIB library
#define VK_USE_PLATFORM_XLIB_KHR
// For Mir Windowing System
#define VK_USE_PLATFORM_MIR_KHR
// For Wayland Windowing System
#define VK_USE_PLATFORM_WAYLAND_KHR
#endif

#if defined(EN_PLATFORM_WINDOWS)
#define VK_USE_PLATFORM_WIN32_KHR
#endif

// Specify that we declare function pointers on our own
// to load them from Dynamically Linked Library.
#define VK_NO_PROTOTYPES

#include "Vulkan/vulkan.h"

// Vulkan renderer is Multi-Threaded
#include "threading/scheduler.h"

#endif

#endif
