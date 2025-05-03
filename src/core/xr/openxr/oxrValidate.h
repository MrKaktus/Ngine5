/*

 Ngine v5.0
 
 Module      : OpenXR Validation.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_XR_OPENXR_VALIDATE
#define ENG_CORE_XR_OPENXR_VALIDATE

#include "core/defines.h"

#include "OpenXR/openxr.h"

namespace en
{
namespace xr
{

extern bool IsError(const XrResult result);
extern bool IsWarning(const XrResult result);

} // en::gpu
} // en

#ifdef EN_DEBUG

#ifdef EN_PROFILER_TRACE_XR_API

#define Validate( instance, command )                                 \
           {                                                          \
           uint32 threadId = currentThreadId();                       \
           assert( threadId < MaxSupportedThreads );                  \
           enLog << "[" << std::setw(2) << threadId << "] ";          \
           enLog << "OpenXR: " << #command << std::endl;              \
           instance->lastResult[threadId] = command;                  \
           if (en::xr::IsError(instance->lastResult[threadId]))       \
              { assert( 0 ); }                                        \
           en::xr::IsWarning(instance->lastResult[threadId]);         \
           }

#define ValidateNoRet( instance, command )                            \
           {                                                          \
           uint32 threadId = currentThreadId();                       \
           assert( threadId < MaxSupportedThreads );                  \
           enLog << "[" << std::setw(2) << threadId << "] ";          \
           enLog << "OpenXR: " << #command << std::endl;              \
           command;                                                   \
           }

#else 

#define Validate( instance, command )                                 \
           {                                                          \
           uint32 threadId = currentThreadId();                       \
           assert( threadId < MaxSupportedThreads );                  \
           instance->lastResult[threadId] = command;                  \
           if (en::xr::IsError(instance->lastResult[threadId]))       \
              { assert( 0 ); }                                        \
           en::xr::IsWarning(instance->lastResult[threadId]);         \
           }

#define ValidateNoRet( command )                                      \
           command;

#endif

#else // Release

#define Validate( instance, command )                                 \
           instance->lastResult[en::currentThreadId()] = command;

#define ValidateNoRet( command )                                      \
           command;

#endif

#endif