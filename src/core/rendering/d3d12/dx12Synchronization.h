/*

 Ngine v5.0
 
 Module      : D3D12 Synchronization.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_D3D12_SYNCHRONIZATION
#define ENG_CORE_RENDERING_D3D12_SYNCHRONIZATION

#include "core/defines.h"

#if defined(EN_MODULE_RENDERER_DIRECT3D12)

#include "core/rendering/synchronization.h"
#include "core/rendering/d3d12/dx12.h"

namespace en
{
namespace gpu
{

class Direct3D12Device;

// TODO: Is there a way for D3D12 equivalent of Vulkan functionality? 
 
// It's currently just a handle to synchronize command buffers execution.
// Similar to MTLFence in Metal, but works between CB's submissions.
class SemaphoreD3D12 : public Semaphore
{
    public:
    Direct3D12Device* gpu;
    ID3D12Fence*      fence;        // Direct pointer to one of queue's fences
    uint64            waitForValue; // Moment in time represented by unique value

    SemaphoreD3D12(Direct3D12Device* _gpu);
   ~SemaphoreD3D12();
};

} // en::gpu
} // en
#endif

#endif