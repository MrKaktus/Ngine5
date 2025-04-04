/*

 Ngine v5.0
 
 Module      : Vulkan Validation.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_VULKAN_VALIDATE
#define ENG_CORE_RENDERING_VULKAN_VALIDATE

#include "core/defines.h"
#include "core/rendering/vulkan/vulkan.h"

#ifdef EN_DEBUG
namespace en
{
namespace gpu
{

extern bool IsError(const VkResult result);
extern bool IsWarning(const VkResult result);

} // en::gpu
} // en

#endif

// gpu     - pointer to class storing pointer to called function.
//           Should be VulkanDevice*, or VulkanGPU*.
// command - Vulkan API function call to execute.
//
// Result of function call is stored per GPU, per Thread.
//
#if defined(Validate)
#undef Validate
#endif 
#if defined(ValidateNoRet)
#undef ValidateNoRet
#endif 

#ifdef EN_DEBUG
   #ifdef EN_PROFILER_TRACE_GRAPHICS_API

   #define Validate( _gpu, command )                                                          \
           {                                                                                  \
           uint32 threadId = currentThreadId();                                               \
           assert( threadId < MaxSupportedThreads );                                          \
           Log << "[" << std::setw(2) << threadId << "] ";                                    \
           Log << "Vulkan GPU " << std::setbase(16) << _gpu << ": " << #command << std::endl; \
           _gpu->lastResult[threadId] = _gpu->command;                                        \
           if (en::gpu::IsError(_gpu->lastResult[threadId]))                                  \
              { assert( 0 ); }                                                                \
           en::gpu::IsWarning(_gpu->lastResult[threadId]);                                    \
           }

   #define ValidateNoRet( _gpu, command )                                                     \
           {                                                                                  \
           uint32 threadId = currentThreadId();                                               \
           assert( threadId < MaxSupportedThreads );                                          \
           Log << "[" << std::setw(2) << threadId << "] ";                                    \
           Log << "Vulkan GPU " << std::setbase(16) << _gpu << ": " << #command << std::endl; \
           _gpu->command;                                                                     \
           }

   #else 

   #define Validate( _gpu, command )                                   \
           {                                                           \
           uint32 threadId = currentThreadId();                        \
           assert( threadId < MaxSupportedThreads );                   \
           _gpu->lastResult[threadId] = _gpu->command;                 \
           if (en::gpu::IsError(_gpu->lastResult[threadId]))           \
              { assert( 0 ); }                                         \
           en::gpu::IsWarning(_gpu->lastResult[threadId]);             \
           }

   #define ValidateNoRet( _gpu, command )                              \
           _gpu->command;

   #endif
   
#else // Release

   #define Validate( _gpu, command )                                   \
           _gpu->lastResult[en::currentThreadId()] = _gpu->command;

   #define ValidateNoRet( _gpu, command )                              \
           _gpu->command;

#endif

#endif