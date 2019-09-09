/*

 Ngine v5.0
 
 Module      : D3D12 Validation.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_D3D12_VALIDATE
#define ENG_CORE_RENDERING_D3D12_VALIDATE

#include "core/defines.h"
#include "core/rendering/d3d12/dx12.h"

#if defined(Validate)
#undef Validate
#endif 
#if defined(ValidateCom)
#undef ValidateCom
#endif 
#if defined(ValidateNoRet)
#undef ValidateNoRet
#endif 
#if defined(ValidateComNoRet)
#undef ValidateComNoRet
#endif 

#ifdef EN_DEBUG
namespace en
{
namespace gpu
{
extern bool IsError(ID3D12Device* device, const HRESULT result);
extern bool IsWarning(const HRESULT result);
}
}

//
// gpu     - pointer to class storing pointer to called function.
//           Should be VulkanDevice*, or VulkanGPU*.
// command - Vulkan API function call to execute.
//
// Result of function call is stored per GPU, per Thread.
// threading/scheduler.h needs to be included.
//

    #ifdef EN_PROFILER_TRACE_GRAPHICS_API
 
    #define Validate( _gpu, command )                                                         \
            {                                                                                 \
            uint32 threadId = currentThreadId();                                              \
            assert( threadId < MaxSupportedThreads );                                         \
            Log << "[" << std::setw(2) << threadId << "] ";                                   \
            Log << "D3D12 GPU " << std::setbase(16) << _gpu << ": " << #command << std::endl; \
            _gpu->lastResult[threadId] = _gpu->device->command;                               \
            if (en::gpu::IsError(_gpu->lastResult[threadId]))                                 \
               assert( 0 );                                                                   \
            en::gpu::IsWarning(_gpu->lastResult[threadId]);                                   \
            }
 
    #define ValidateCom( command )                                                            \
            {                                                                                 \
            uint32 threadId = currentThreadId();                                              \
            assert( threadId < MaxSupportedThreads );                                         \
            Log << "[" << std::setw(2) << threadId << "] ";                                   \
            Log << "D3D12 GPU 0xXXXXXXXX: " << #command << std::endl;                         \
            HRESULT hr = command;                                                             \
            assert( SUCCEEDED(hr) );                                                          \
            }
 
    #define ValidateNoRet( _gpu, command )                                                    \
            {                                                                                 \
            uint32 threadId = currentThreadId();                                              \
            assert( threadId < MaxSupportedThreads );                                         \
            Log << "[" << std::setw(2) << threadId << "] ";                                   \
            Log << "D3D12 GPU " << std::setbase(16) << _gpu << ": " << #command << std::endl; \
            _gpu->device->command;                                                            \
            }
 
    #define ValidateComNoRet( command )                                                       \
            {                                                                                 \
            uint32 threadId = currentThreadId();                                              \
            assert( threadId < MaxSupportedThreads );                                         \
            Log << "[" << std::setw(2) << threadId << "] ";                                   \
            Log << "D3D12 GPU 0xXXXXXXXX: " << #command << std::endl;                         \
            command;                                                                          \
            }
    #else 
 
    #define Validate( _gpu, command )                                                         \
            {                                                                                 \
            uint32 threadId = currentThreadId();                                              \
            assert( threadId < MaxSupportedThreads );                                         \
            _gpu->lastResult[threadId] = _gpu->device->command;                               \
            if (en::gpu::IsError(_gpu->device, _gpu->lastResult[threadId]))                   \
               assert( 0 );                                                                   \
            en::gpu::IsWarning(_gpu->lastResult[threadId]);                                   \
            }
 
    #define ValidateCom( command )                                      \
            {                                                           \
            HRESULT hr = command;                                       \
            assert( SUCCEEDED(hr) );                                    \
            }
 
    #define ValidateNoRet( _gpu, command )                              \
            _gpu->device->command;
 
    #define ValidateComNoRet( command )                                 \
            command;
 
 
    #endif
   
#else // Release

    #define Validate( _gpu, command )                                   \
            _gpu->lastResult[currentThreadId()] = _gpu->device->command;
 
    #define ValidateCom( command )                                     \
            command;
 
    #define ValidateNoRet( _gpu, command )                             \
            _gpu->device->command;
 
    #define ValidateComNoRet( command )                                \
            command;

#endif

#endif